
#include "unicode/utypes.h"

#if !UCONFIG_NO_NORMALIZATION

#include "unicode/uset.h"
#include "unicode/ustring.h"
#include "hash.h"
#include "unormimp.h"
#include "unicode/caniter.h"
#include "unicode/normlzr.h"
#include "unicode/uchar.h"
#include "cmemory.h"


// public

U_NAMESPACE_BEGIN

// TODO: add boilerplate methods.

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(CanonicalIterator)

CanonicalIterator::CanonicalIterator(const UnicodeString &sourceStr, UErrorCode &status) :
    pieces(NULL),
    pieces_length(0),
    pieces_lengths(NULL),
    current(NULL),
    current_length(0)
{
    if(U_SUCCESS(status)) {
      setSource(sourceStr, status);
    }
}

CanonicalIterator::~CanonicalIterator() {
  cleanPieces();
}

void CanonicalIterator::cleanPieces() {
    int32_t i = 0;
    if(pieces != NULL) {
        for(i = 0; i < pieces_length; i++) {
            if(pieces[i] != NULL) {
                delete[] pieces[i];
            }
        }
        uprv_free(pieces);
        pieces = NULL;
        pieces_length = 0;
    }
    if(pieces_lengths != NULL) {
        uprv_free(pieces_lengths);
        pieces_lengths = NULL;
    }
    if(current != NULL) {
        uprv_free(current);
        current = NULL;
        current_length = 0;
    }
}

UnicodeString CanonicalIterator::getSource() {
  return source;
}

void CanonicalIterator::reset() {
    done = FALSE;
    for (int i = 0; i < current_length; ++i) {
        current[i] = 0;
    }
}

UnicodeString CanonicalIterator::next() {
    int32_t i = 0;

    if (done) {
      buffer.setToBogus();
      return buffer;
    }

    // delete old contents
    buffer.remove();

    // construct return value

    for (i = 0; i < pieces_length; ++i) {
        buffer.append(pieces[i][current[i]]);
    }
    //String result = buffer.toString(); // not needed

    // find next value for next time

    for (i = current_length - 1; ; --i) {
        if (i < 0) {
            done = TRUE;
            break;
        }
        current[i]++;
        if (current[i] < pieces_lengths[i]) break; // got sequence
        current[i] = 0;
    }
    return buffer;
}

void CanonicalIterator::setSource(const UnicodeString &newSource, UErrorCode &status) {
    int32_t list_length = 0;
    UChar32 cp = 0;
    int32_t start = 0;
    int32_t i = 0;
    UnicodeString *list = NULL;

    Normalizer::normalize(newSource, UNORM_NFD, 0, source, status);
    if(U_FAILURE(status)) {
      return;
    }
    done = FALSE;

    cleanPieces();

    // catch degenerate case
    if (newSource.length() == 0) {
        pieces = (UnicodeString **)uprv_malloc(sizeof(UnicodeString *));
        pieces_lengths = (int32_t*)uprv_malloc(1 * sizeof(int32_t));
        pieces_length = 1;
        current = (int32_t*)uprv_malloc(1 * sizeof(int32_t));
        current_length = 1;
        if (pieces == NULL || pieces_lengths == NULL || current == NULL) {
            status = U_MEMORY_ALLOCATION_ERROR;
            goto CleanPartialInitialization;
        }
        current[0] = 0;
        pieces[0] = new UnicodeString[1];
        pieces_lengths[0] = 1;
        if (pieces[0] == 0) {
            status = U_MEMORY_ALLOCATION_ERROR;
            goto CleanPartialInitialization;
        }
        return;
    }


    list = new UnicodeString[source.length()];
    if (list == 0) {
        status = U_MEMORY_ALLOCATION_ERROR;
        goto CleanPartialInitialization;
    }

    // i should initialy be the number of code units at the 
    // start of the string
    i = UTF16_CHAR_LENGTH(source.char32At(0));
    //int32_t i = 1;
    // find the segments
    // This code iterates through the source string and 
    // extracts segments that end up on a codepoint that
    // doesn't start any decompositions. (Analysis is done
    // on the NFD form - see above).
    for (; i < source.length(); i += UTF16_CHAR_LENGTH(cp)) {
        cp = source.char32At(i);
        if (unorm_isCanonSafeStart(cp)) {
            source.extract(start, i-start, list[list_length++]); // add up to i
            start = i;
        }
    }
    source.extract(start, i-start, list[list_length++]); // add last one


    // allocate the arrays, and find the strings that are CE to each segment
    pieces = (UnicodeString **)uprv_malloc(list_length * sizeof(UnicodeString *));
    pieces_length = list_length;
    pieces_lengths = (int32_t*)uprv_malloc(list_length * sizeof(int32_t));
    current = (int32_t*)uprv_malloc(list_length * sizeof(int32_t));
    current_length = list_length;
    if (pieces == NULL || pieces_lengths == NULL || current == NULL) {
        status = U_MEMORY_ALLOCATION_ERROR;
        goto CleanPartialInitialization;
    }

    for (i = 0; i < current_length; i++) {
        current[i] = 0;
    }
    // for each segment, get all the combinations that can produce 
    // it after NFD normalization
    for (i = 0; i < pieces_length; ++i) {
        //if (PROGRESS) printf("SEGMENT\n");
        pieces[i] = getEquivalents(list[i], pieces_lengths[i], status);
    }

    delete[] list;
    return;
// Common section to cleanup all local variables and reset object variables.
CleanPartialInitialization:
    if (list != NULL) {
        delete[] list;
    }
    cleanPieces();
}

void U_EXPORT2 CanonicalIterator::permute(UnicodeString &source, UBool skipZeros, Hashtable *result, UErrorCode &status) {
    if(U_FAILURE(status)) {
        return;
    }
    //if (PROGRESS) printf("Permute: %s\n", UToS(Tr(source)));
    int32_t i = 0;

    // optimization:
    // if zero or one character, just return a set with it
    // we check for length < 2 to keep from counting code points all the time
    if (source.length() <= 2 && source.countChar32() <= 1) {
        UnicodeString *toPut = new UnicodeString(source);
        /* test for NULL */
        if (toPut == 0) {
            status = U_MEMORY_ALLOCATION_ERROR;
            return;
        }
        result->put(source, toPut, status);
        return;
    }

    // otherwise iterate through the string, and recursively permute all the other characters
    UChar32 cp;
    Hashtable subpermute(status);
    if(U_FAILURE(status)) {
        return;
    }
    subpermute.setValueDeleter(uhash_deleteUnicodeString);

    for (i = 0; i < source.length(); i += UTF16_CHAR_LENGTH(cp)) {
        cp = source.char32At(i);
        const UHashElement *ne = NULL;
        int32_t el = -1;
        UnicodeString subPermuteString = source;

        // optimization:
        // if the character is canonical combining class zero,
        // don't permute it
        if (skipZeros && i != 0 && u_getCombiningClass(cp) == 0) {
            //System.out.println("Skipping " + Utility.hex(UTF16.valueOf(source, i)));
            continue;
        }

        subpermute.removeAll();

        // see what the permutations of the characters before and after this one are
        //Hashtable *subpermute = permute(source.substring(0,i) + source.substring(i + UTF16.getCharCount(cp)));
        permute(subPermuteString.replace(i, UTF16_CHAR_LENGTH(cp), NULL, 0), skipZeros, &subpermute, status);
        /* Test for buffer overflows */
        if(U_FAILURE(status)) {
            return;
        }
        // The upper replace is destructive. The question is do we have to make a copy, or we don't care about the contents 
        // of source at this point.

        // prefix this character to all of them
        ne = subpermute.nextElement(el);
        while (ne != NULL) {
            UnicodeString *permRes = (UnicodeString *)(ne->value.pointer);
            UnicodeString *chStr = new UnicodeString(cp);
            //test for  NULL
            if (chStr == NULL) {
                status = U_MEMORY_ALLOCATION_ERROR;
                return;
            }
            chStr->append(*permRes); //*((UnicodeString *)(ne->value.pointer));
            //if (PROGRESS) printf("  Piece: %s\n", UToS(*chStr));
            result->put(*chStr, chStr, status);
            ne = subpermute.nextElement(el);
        }
    }
    //return result;
}

// privates

// we have a segment, in NFD. Find all the strings that are canonically equivalent to it.
UnicodeString* CanonicalIterator::getEquivalents(const UnicodeString &segment, int32_t &result_len, UErrorCode &status) {
    Hashtable result(status);
    Hashtable permutations(status);
    Hashtable basic(status);
    if (U_FAILURE(status)) {
        return 0;
    }
    result.setValueDeleter(uhash_deleteUnicodeString);
    permutations.setValueDeleter(uhash_deleteUnicodeString);
    basic.setValueDeleter(uhash_deleteUnicodeString);

    UChar USeg[256];
    int32_t segLen = segment.extract(USeg, 256, status);
    getEquivalents2(&basic, USeg, segLen, status);

    // now get all the permutations
    // add only the ones that are canonically equivalent
    // TODO: optimize by not permuting any class zero.

    const UHashElement *ne = NULL;
    int32_t el = -1;
    //Iterator it = basic.iterator();
    ne = basic.nextElement(el);
    //while (it.hasNext())
    while (ne != NULL) {
        //String item = (String) it.next();
        UnicodeString item = *((UnicodeString *)(ne->value.pointer));

        permutations.removeAll();
        permute(item, CANITER_SKIP_ZEROES, &permutations, status);
        const UHashElement *ne2 = NULL;
        int32_t el2 = -1;
        //Iterator it2 = permutations.iterator();
        ne2 = permutations.nextElement(el2);
        //while (it2.hasNext())
        while (ne2 != NULL) {
            //String possible = (String) it2.next();
            //UnicodeString *possible = new UnicodeString(*((UnicodeString *)(ne2->value.pointer)));
            UnicodeString possible(*((UnicodeString *)(ne2->value.pointer)));
            UnicodeString attempt;
            Normalizer::normalize(possible, UNORM_NFD, 0, attempt, status);

            // TODO: check if operator == is semanticaly the same as attempt.equals(segment)
            if (attempt==segment) {
                //if (PROGRESS) printf("Adding Permutation: %s\n", UToS(Tr(*possible)));
                // TODO: use the hashtable just to catch duplicates - store strings directly (somehow).
                result.put(possible, new UnicodeString(possible), status); //add(possible);
            } else {
                //if (PROGRESS) printf("-Skipping Permutation: %s\n", UToS(Tr(*possible)));
            }

            ne2 = permutations.nextElement(el2);
        }
        ne = basic.nextElement(el);
    }

    /* Test for buffer overflows */
    if(U_FAILURE(status)) {
        return 0;
    }
    // convert into a String[] to clean up storage
    //String[] finalResult = new String[result.size()];
    UnicodeString *finalResult = NULL;
    int32_t resultCount;
    if((resultCount = result.count())) {
        finalResult = new UnicodeString[resultCount];
        if (finalResult == 0) {
            status = U_MEMORY_ALLOCATION_ERROR;
            return NULL;
        }
    }
    else {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        return NULL;
    }
    //result.toArray(finalResult);
    result_len = 0;
    el = -1;
    ne = result.nextElement(el);
    while(ne != NULL) {
        finalResult[result_len++] = *((UnicodeString *)(ne->value.pointer));
        ne = result.nextElement(el);
    }


    return finalResult;
}

Hashtable *CanonicalIterator::getEquivalents2(Hashtable *fillinResult, const UChar *segment, int32_t segLen, UErrorCode &status) {

    if (U_FAILURE(status)) {
        return NULL;
    }

    //if (PROGRESS) printf("Adding: %s\n", UToS(Tr(segment)));

    UnicodeString toPut(segment, segLen);

    fillinResult->put(toPut, new UnicodeString(toPut), status);

    USerializedSet starts;

    // cycle through all the characters
    UChar32 cp, end = 0;
    int32_t i = 0, j;
    for (i = 0; i < segLen; i += UTF16_CHAR_LENGTH(cp)) {
        // see if any character is at the start of some decomposition
        UTF_GET_CHAR(segment, 0, i, segLen, cp);
        if (!unorm_getCanonStartSet(cp, &starts)) {
            continue;
        }
        // if so, see which decompositions match 
        for(j = 0, cp = end+1; cp <= end || uset_getSerializedRange(&starts, j++, &cp, &end); ++cp) {
            Hashtable remainder(status);
            remainder.setValueDeleter(uhash_deleteUnicodeString);
            if (extract(&remainder, cp, segment, segLen, i, status) == NULL) {
                continue;
            }

            // there were some matches, so add all the possibilities to the set.
            UnicodeString prefix(segment, i);
            prefix += cp;

            int32_t el = -1;
            const UHashElement *ne = remainder.nextElement(el);
            while (ne != NULL) {
                UnicodeString item = *((UnicodeString *)(ne->value.pointer));
                UnicodeString *toAdd = new UnicodeString(prefix);
                /* test for NULL */
                if (toAdd == 0) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    return NULL;
                }
                *toAdd += item;
                fillinResult->put(*toAdd, toAdd, status);

                //if (PROGRESS) printf("Adding: %s\n", UToS(Tr(*toAdd)));

                ne = remainder.nextElement(el);
            }
        }
    }

    /* Test for buffer overflows */
    if(U_FAILURE(status)) {
        return NULL;
    }
    return fillinResult;
}

Hashtable *CanonicalIterator::extract(Hashtable *fillinResult, UChar32 comp, const UChar *segment, int32_t segLen, int32_t segmentPos, UErrorCode &status) {
//Hashtable *CanonicalIterator::extract(UChar32 comp, const UnicodeString &segment, int32_t segLen, int32_t segmentPos, UErrorCode &status) {
    //if (PROGRESS) printf(" extract: %s, ", UToS(Tr(UnicodeString(comp))));
    //if (PROGRESS) printf("%s, %i\n", UToS(Tr(segment)), segmentPos);

    if (U_FAILURE(status)) {
        return NULL;
    }

    const int32_t bufSize = 256;
    int32_t bufLen = 0;
    UChar temp[bufSize];

    int32_t inputLen = 0, decompLen;
    UChar stackBuffer[4];
    const UChar *decomp;

    U16_APPEND_UNSAFE(temp, inputLen, comp);
    decomp = unorm_getCanonicalDecomposition(comp, stackBuffer, &decompLen);
    if(decomp == NULL) {
        /* copy temp */
        stackBuffer[0] = temp[0];
        if(inputLen > 1) {
            stackBuffer[1] = temp[1];
        }
        decomp = stackBuffer;
        decompLen = inputLen;
    }

    UChar *buff = temp+inputLen;

    // See if it matches the start of segment (at segmentPos)
    UBool ok = FALSE;
    UChar32 cp;
    int32_t decompPos = 0;
    UChar32 decompCp;
    UTF_NEXT_CHAR(decomp, decompPos, decompLen, decompCp);

    int32_t i;
    UBool overflow = FALSE;

    i = segmentPos;
    while(i < segLen) {
        UTF_NEXT_CHAR(segment, i, segLen, cp);

        if (cp == decompCp) { // if equal, eat another cp from decomp

            //if (PROGRESS) printf("  matches: %s\n", UToS(Tr(UnicodeString(cp))));

            if (decompPos == decompLen) { // done, have all decomp characters!
                //u_strcat(buff+bufLen, segment+i);
                uprv_memcpy(buff+bufLen, segment+i, (segLen-i)*sizeof(UChar));
                bufLen+=segLen-i;

                ok = TRUE;
                break;
            }
            UTF_NEXT_CHAR(decomp, decompPos, decompLen, decompCp);
        } else {
            //if (PROGRESS) printf("  buffer: %s\n", UToS(Tr(UnicodeString(cp))));

            // brute force approach

            U16_APPEND(buff, bufLen, bufSize, cp, overflow);

            if(overflow) {
                /*
                 * ### TODO handle buffer overflow
                 * The buffer is large, but an overflow may still happen with
                 * unusual input (many combining marks?).
                 * Reallocate buffer and continue.
                 * markus 20020929
                 */

                overflow = FALSE;
            }

            /* TODO: optimize
            // since we know that the classes are monotonically increasing, after zero
            // e.g. 0 5 7 9 0 3
            // we can do an optimization
            // there are only a few cases that work: zero, less, same, greater
            // if both classes are the same, we fail
            // if the decomp class < the segment class, we fail

            segClass = getClass(cp);
            if (decompClass <= segClass) return null;
            */
        }
    }
    if (!ok)
        return NULL; // we failed, characters left over

    //if (PROGRESS) printf("Matches\n");

    if (bufLen == 0) {
        fillinResult->put(UnicodeString(), new UnicodeString(), status);
        return fillinResult; // succeed, but no remainder
    }

    // brute force approach
    // check to make sure result is canonically equivalent
    int32_t tempLen = inputLen + bufLen;

    UChar trial[bufSize];
    unorm_decompose(trial, bufSize, temp, tempLen, FALSE, 0, &status);

    if(U_FAILURE(status)
        || uprv_memcmp(segment+segmentPos, trial, (segLen - segmentPos)*sizeof(UChar)) != 0)
    {
        return NULL;
    }

    return getEquivalents2(fillinResult, buff, bufLen, status);
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_NORMALIZATION */
