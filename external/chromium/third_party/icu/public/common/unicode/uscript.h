
#ifndef USCRIPT_H
#define USCRIPT_H
#include "unicode/utypes.h"

 
typedef enum UScriptCode {
      USCRIPT_INVALID_CODE = -1,
      USCRIPT_COMMON       =  0 , /* Zyyy */
      USCRIPT_INHERITED    =  1,  /* Qaai */
      USCRIPT_ARABIC       =  2,  /* Arab */
      USCRIPT_ARMENIAN     =  3,  /* Armn */
      USCRIPT_BENGALI      =  4,  /* Beng */
      USCRIPT_BOPOMOFO     =  5,  /* Bopo */
      USCRIPT_CHEROKEE     =  6,  /* Cher */
      USCRIPT_COPTIC       =  7,  /* Copt */
      USCRIPT_CYRILLIC     =  8,  /* Cyrl */
      USCRIPT_DESERET      =  9,  /* Dsrt */
      USCRIPT_DEVANAGARI   = 10,  /* Deva */
      USCRIPT_ETHIOPIC     = 11,  /* Ethi */
      USCRIPT_GEORGIAN     = 12,  /* Geor */
      USCRIPT_GOTHIC       = 13,  /* Goth */
      USCRIPT_GREEK        = 14,  /* Grek */
      USCRIPT_GUJARATI     = 15,  /* Gujr */
      USCRIPT_GURMUKHI     = 16,  /* Guru */
      USCRIPT_HAN          = 17,  /* Hani */
      USCRIPT_HANGUL       = 18,  /* Hang */
      USCRIPT_HEBREW       = 19,  /* Hebr */
      USCRIPT_HIRAGANA     = 20,  /* Hira */
      USCRIPT_KANNADA      = 21,  /* Knda */
      USCRIPT_KATAKANA     = 22,  /* Kana */
      USCRIPT_KHMER        = 23,  /* Khmr */
      USCRIPT_LAO          = 24,  /* Laoo */
      USCRIPT_LATIN        = 25,  /* Latn */
      USCRIPT_MALAYALAM    = 26,  /* Mlym */
      USCRIPT_MONGOLIAN    = 27,  /* Mong */
      USCRIPT_MYANMAR      = 28,  /* Mymr */
      USCRIPT_OGHAM        = 29,  /* Ogam */
      USCRIPT_OLD_ITALIC   = 30,  /* Ital */
      USCRIPT_ORIYA        = 31,  /* Orya */
      USCRIPT_RUNIC        = 32,  /* Runr */
      USCRIPT_SINHALA      = 33,  /* Sinh */
      USCRIPT_SYRIAC       = 34,  /* Syrc */
      USCRIPT_TAMIL        = 35,  /* Taml */
      USCRIPT_TELUGU       = 36,  /* Telu */
      USCRIPT_THAANA       = 37,  /* Thaa */
      USCRIPT_THAI         = 38,  /* Thai */
      USCRIPT_TIBETAN      = 39,  /* Tibt */
      /** Canadian_Aboriginal script. @stable ICU 2.6 */
      USCRIPT_CANADIAN_ABORIGINAL = 40,  /* Cans */
      /** Canadian_Aboriginal script (alias). @stable ICU 2.2 */
      USCRIPT_UCAS         = USCRIPT_CANADIAN_ABORIGINAL,
      USCRIPT_YI           = 41,  /* Yiii */
      USCRIPT_TAGALOG      = 42,  /* Tglg */
      USCRIPT_HANUNOO      = 43,  /* Hano */
      USCRIPT_BUHID        = 44,  /* Buhd */
      USCRIPT_TAGBANWA     = 45,  /* Tagb */

      /* New scripts in Unicode 4 @stable ICU 2.6 */
      USCRIPT_BRAILLE      = 46,  /* Brai */
      USCRIPT_CYPRIOT      = 47,  /* Cprt */
      USCRIPT_LIMBU        = 48,  /* Limb */
      USCRIPT_LINEAR_B     = 49,  /* Linb */
      USCRIPT_OSMANYA      = 50,  /* Osma */
      USCRIPT_SHAVIAN      = 51,  /* Shaw */
      USCRIPT_TAI_LE       = 52,  /* Tale */
      USCRIPT_UGARITIC     = 53,  /* Ugar */

      /** New script code in Unicode 4.0.1 @stable ICU 3.0 */
      USCRIPT_KATAKANA_OR_HIRAGANA = 54,/*Hrkt */
      
      /* New scripts in Unicode 4.1 @stable ICU 3.4 */
      USCRIPT_BUGINESE      = 55, /* Bugi */
      USCRIPT_GLAGOLITIC    = 56, /* Glag */
      USCRIPT_KHAROSHTHI    = 57, /* Khar */
      USCRIPT_SYLOTI_NAGRI  = 58, /* Sylo */
      USCRIPT_NEW_TAI_LUE   = 59, /* Talu */
      USCRIPT_TIFINAGH      = 60, /* Tfng */
      USCRIPT_OLD_PERSIAN   = 61, /* Xpeo */

      /* New script codes from ISO 15924 @stable ICU 3.6 */
      USCRIPT_BALINESE                      = 62, /* Bali */
      USCRIPT_BATAK                         = 63, /* Batk */
      USCRIPT_BLISSYMBOLS                   = 64, /* Blis */
      USCRIPT_BRAHMI                        = 65, /* Brah */
      USCRIPT_CHAM                          = 66, /* Cham */
      USCRIPT_CIRTH                         = 67, /* Cirt */
      USCRIPT_OLD_CHURCH_SLAVONIC_CYRILLIC  = 68, /* Cyrs */
      USCRIPT_DEMOTIC_EGYPTIAN              = 69, /* Egyd */
      USCRIPT_HIERATIC_EGYPTIAN             = 70, /* Egyh */
      USCRIPT_EGYPTIAN_HIEROGLYPHS          = 71, /* Egyp */
      USCRIPT_KHUTSURI                      = 72, /* Geok */
      USCRIPT_SIMPLIFIED_HAN                = 73, /* Hans */
      USCRIPT_TRADITIONAL_HAN               = 74, /* Hant */
      USCRIPT_PAHAWH_HMONG                  = 75, /* Hmng */
      USCRIPT_OLD_HUNGARIAN                 = 76, /* Hung */
      USCRIPT_HARAPPAN_INDUS                = 77, /* Inds */
      USCRIPT_JAVANESE                      = 78, /* Java */
      USCRIPT_KAYAH_LI                      = 79, /* Kali */
      USCRIPT_LATIN_FRAKTUR                 = 80, /* Latf */
      USCRIPT_LATIN_GAELIC                  = 81, /* Latg */
      USCRIPT_LEPCHA                        = 82, /* Lepc */
      USCRIPT_LINEAR_A                      = 83, /* Lina */
      USCRIPT_MANDAEAN                      = 84, /* Mand */
      USCRIPT_MAYAN_HIEROGLYPHS             = 85, /* Maya */
      USCRIPT_MEROITIC                      = 86, /* Mero */
      USCRIPT_NKO                           = 87, /* Nkoo */
      USCRIPT_ORKHON                        = 88, /* Orkh */
      USCRIPT_OLD_PERMIC                    = 89, /* Perm */
      USCRIPT_PHAGS_PA                      = 90, /* Phag */
      USCRIPT_PHOENICIAN                    = 91, /* Phnx */
      USCRIPT_PHONETIC_POLLARD              = 92, /* Plrd */
      USCRIPT_RONGORONGO                    = 93, /* Roro */
      USCRIPT_SARATI                        = 94, /* Sara */
      USCRIPT_ESTRANGELO_SYRIAC             = 95, /* Syre */
      USCRIPT_WESTERN_SYRIAC                = 96, /* Syrj */
      USCRIPT_EASTERN_SYRIAC                = 97, /* Syrn */
      USCRIPT_TENGWAR                       = 98, /* Teng */
      USCRIPT_VAI                           = 99, /* Vaii */
      USCRIPT_VISIBLE_SPEECH                = 100, /* Visp */
      USCRIPT_CUNEIFORM                     = 101,/* Xsux */
      USCRIPT_UNWRITTEN_LANGUAGES           = 102,/* Zxxx */
      USCRIPT_UNKNOWN                       = 103,/* Zzzz */ /* Unknown="Code for uncoded script", for unassigned code points */

      /* New script codes from ISO 15924 @stable ICU 3.8 */
      USCRIPT_CARIAN                        = 104,/* Cari */
      USCRIPT_JAPANESE                      = 105,/* Jpan */
      USCRIPT_LANNA                         = 106,/* Lana */
      USCRIPT_LYCIAN                        = 107,/* Lyci */
      USCRIPT_LYDIAN                        = 108,/* Lydi */
      USCRIPT_OL_CHIKI                      = 109,/* Olck */
      USCRIPT_REJANG                        = 110,/* Rjng */
      USCRIPT_SAURASHTRA                    = 111,/* Saur */
      USCRIPT_SIGN_WRITING                  = 112,/* Sgnw */
      USCRIPT_SUNDANESE                     = 113,/* Sund */
      USCRIPT_MOON                          = 114,/* Moon */
      USCRIPT_MEITEI_MAYEK                  = 115,/* Mtei */

      /* New script codes from ISO 15924 @stable ICU 4.0 */
      USCRIPT_IMPERIAL_ARAMAIC              = 116,/* Armi */
      USCRIPT_AVESTAN                       = 117,/* Avst */
      USCRIPT_CHAKMA                        = 118,/* Cakm */
      USCRIPT_KOREAN                        = 119,/* Kore */
      USCRIPT_KAITHI                        = 120,/* Kthi */
      USCRIPT_MANICHAEAN                    = 121,/* Mani */
      USCRIPT_INSCRIPTIONAL_PAHLAVI         = 122,/* Phli */
      USCRIPT_PSALTER_PAHLAVI               = 123,/* Phlp */
      USCRIPT_BOOK_PAHLAVI                  = 124,/* Phlv */
      USCRIPT_INSCRIPTIONAL_PARTHIAN        = 125,/* Prti */
      USCRIPT_SAMARITAN                     = 126,/* Samr */
      USCRIPT_TAI_VIET                      = 127,/* Tavt */
      USCRIPT_MATHEMATICAL_NOTATION         = 128,/* Zmth */
      USCRIPT_SYMBOLS                       = 129,/* Zsym */

      /* Private use codes from Qaaa - Qabx are not supported*/
      USCRIPT_CODE_LIMIT    = 130
} UScriptCode;

U_STABLE int32_t  U_EXPORT2 
uscript_getCode(const char* nameOrAbbrOrLocale,UScriptCode* fillIn,int32_t capacity,UErrorCode *err);

U_STABLE const char*  U_EXPORT2 
uscript_getName(UScriptCode scriptCode);

U_STABLE const char*  U_EXPORT2 
uscript_getShortName(UScriptCode scriptCode);

U_STABLE UScriptCode  U_EXPORT2 
uscript_getScript(UChar32 codepoint, UErrorCode *err);

#endif


