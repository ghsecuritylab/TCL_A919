
#include <linux/module.h>
#include <linux/ctype.h>
#include <linux/errno.h>
#include <linux/bitmap.h>
#include <linux/bitops.h>
#include <asm/uaccess.h>


int __bitmap_empty(const unsigned long *bitmap, int bits)
{
	int k, lim = bits/BITS_PER_LONG;
	for (k = 0; k < lim; ++k)
		if (bitmap[k])
			return 0;

	if (bits % BITS_PER_LONG)
		if (bitmap[k] & BITMAP_LAST_WORD_MASK(bits))
			return 0;

	return 1;
}
EXPORT_SYMBOL(__bitmap_empty);

int __bitmap_full(const unsigned long *bitmap, int bits)
{
	int k, lim = bits/BITS_PER_LONG;
	for (k = 0; k < lim; ++k)
		if (~bitmap[k])
			return 0;

	if (bits % BITS_PER_LONG)
		if (~bitmap[k] & BITMAP_LAST_WORD_MASK(bits))
			return 0;

	return 1;
}
EXPORT_SYMBOL(__bitmap_full);

int __bitmap_equal(const unsigned long *bitmap1,
		const unsigned long *bitmap2, int bits)
{
	int k, lim = bits/BITS_PER_LONG;
	for (k = 0; k < lim; ++k)
		if (bitmap1[k] != bitmap2[k])
			return 0;

	if (bits % BITS_PER_LONG)
		if ((bitmap1[k] ^ bitmap2[k]) & BITMAP_LAST_WORD_MASK(bits))
			return 0;

	return 1;
}
EXPORT_SYMBOL(__bitmap_equal);

void __bitmap_complement(unsigned long *dst, const unsigned long *src, int bits)
{
	int k, lim = bits/BITS_PER_LONG;
	for (k = 0; k < lim; ++k)
		dst[k] = ~src[k];

	if (bits % BITS_PER_LONG)
		dst[k] = ~src[k] & BITMAP_LAST_WORD_MASK(bits);
}
EXPORT_SYMBOL(__bitmap_complement);

void __bitmap_shift_right(unsigned long *dst,
			const unsigned long *src, int shift, int bits)
{
	int k, lim = BITS_TO_LONGS(bits), left = bits % BITS_PER_LONG;
	int off = shift/BITS_PER_LONG, rem = shift % BITS_PER_LONG;
	unsigned long mask = (1UL << left) - 1;
	for (k = 0; off + k < lim; ++k) {
		unsigned long upper, lower;

		/*
		 * If shift is not word aligned, take lower rem bits of
		 * word above and make them the top rem bits of result.
		 */
		if (!rem || off + k + 1 >= lim)
			upper = 0;
		else {
			upper = src[off + k + 1];
			if (off + k + 1 == lim - 1 && left)
				upper &= mask;
		}
		lower = src[off + k];
		if (left && off + k == lim - 1)
			lower &= mask;
		dst[k] = upper << (BITS_PER_LONG - rem) | lower >> rem;
		if (left && k == lim - 1)
			dst[k] &= mask;
	}
	if (off)
		memset(&dst[lim - off], 0, off*sizeof(unsigned long));
}
EXPORT_SYMBOL(__bitmap_shift_right);



void __bitmap_shift_left(unsigned long *dst,
			const unsigned long *src, int shift, int bits)
{
	int k, lim = BITS_TO_LONGS(bits), left = bits % BITS_PER_LONG;
	int off = shift/BITS_PER_LONG, rem = shift % BITS_PER_LONG;
	for (k = lim - off - 1; k >= 0; --k) {
		unsigned long upper, lower;

		/*
		 * If shift is not word aligned, take upper rem bits of
		 * word below and make them the bottom rem bits of result.
		 */
		if (rem && k > 0)
			lower = src[k - 1];
		else
			lower = 0;
		upper = src[k];
		if (left && k == lim - 1)
			upper &= (1UL << left) - 1;
		dst[k + off] = lower  >> (BITS_PER_LONG - rem) | upper << rem;
		if (left && k + off == lim - 1)
			dst[k + off] &= (1UL << left) - 1;
	}
	if (off)
		memset(dst, 0, off*sizeof(unsigned long));
}
EXPORT_SYMBOL(__bitmap_shift_left);

int __bitmap_and(unsigned long *dst, const unsigned long *bitmap1,
				const unsigned long *bitmap2, int bits)
{
	int k;
	int nr = BITS_TO_LONGS(bits);
	unsigned long result = 0;

	for (k = 0; k < nr; k++)
		result |= (dst[k] = bitmap1[k] & bitmap2[k]);
	return result != 0;
}
EXPORT_SYMBOL(__bitmap_and);

void __bitmap_or(unsigned long *dst, const unsigned long *bitmap1,
				const unsigned long *bitmap2, int bits)
{
	int k;
	int nr = BITS_TO_LONGS(bits);

	for (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] | bitmap2[k];
}
EXPORT_SYMBOL(__bitmap_or);

void __bitmap_xor(unsigned long *dst, const unsigned long *bitmap1,
				const unsigned long *bitmap2, int bits)
{
	int k;
	int nr = BITS_TO_LONGS(bits);

	for (k = 0; k < nr; k++)
		dst[k] = bitmap1[k] ^ bitmap2[k];
}
EXPORT_SYMBOL(__bitmap_xor);

int __bitmap_andnot(unsigned long *dst, const unsigned long *bitmap1,
				const unsigned long *bitmap2, int bits)
{
	int k;
	int nr = BITS_TO_LONGS(bits);
	unsigned long result = 0;

	for (k = 0; k < nr; k++)
		result |= (dst[k] = bitmap1[k] & ~bitmap2[k]);
	return result != 0;
}
EXPORT_SYMBOL(__bitmap_andnot);

int __bitmap_intersects(const unsigned long *bitmap1,
				const unsigned long *bitmap2, int bits)
{
	int k, lim = bits/BITS_PER_LONG;
	for (k = 0; k < lim; ++k)
		if (bitmap1[k] & bitmap2[k])
			return 1;

	if (bits % BITS_PER_LONG)
		if ((bitmap1[k] & bitmap2[k]) & BITMAP_LAST_WORD_MASK(bits))
			return 1;
	return 0;
}
EXPORT_SYMBOL(__bitmap_intersects);

int __bitmap_subset(const unsigned long *bitmap1,
				const unsigned long *bitmap2, int bits)
{
	int k, lim = bits/BITS_PER_LONG;
	for (k = 0; k < lim; ++k)
		if (bitmap1[k] & ~bitmap2[k])
			return 0;

	if (bits % BITS_PER_LONG)
		if ((bitmap1[k] & ~bitmap2[k]) & BITMAP_LAST_WORD_MASK(bits))
			return 0;
	return 1;
}
EXPORT_SYMBOL(__bitmap_subset);

int __bitmap_weight(const unsigned long *bitmap, int bits)
{
	int k, w = 0, lim = bits/BITS_PER_LONG;

	for (k = 0; k < lim; k++)
		w += hweight_long(bitmap[k]);

	if (bits % BITS_PER_LONG)
		w += hweight_long(bitmap[k] & BITMAP_LAST_WORD_MASK(bits));

	return w;
}
EXPORT_SYMBOL(__bitmap_weight);

#define BITMAP_FIRST_WORD_MASK(start) (~0UL << ((start) % BITS_PER_LONG))

void bitmap_set(unsigned long *map, int start, int nr)
{
	unsigned long *p = map + BIT_WORD(start);
	const int size = start + nr;
	int bits_to_set = BITS_PER_LONG - (start % BITS_PER_LONG);
	unsigned long mask_to_set = BITMAP_FIRST_WORD_MASK(start);

	while (nr - bits_to_set >= 0) {
		*p |= mask_to_set;
		nr -= bits_to_set;
		bits_to_set = BITS_PER_LONG;
		mask_to_set = ~0UL;
		p++;
	}
	if (nr) {
		mask_to_set &= BITMAP_LAST_WORD_MASK(size);
		*p |= mask_to_set;
	}
}
EXPORT_SYMBOL(bitmap_set);

void bitmap_clear(unsigned long *map, int start, int nr)
{
	unsigned long *p = map + BIT_WORD(start);
	const int size = start + nr;
	int bits_to_clear = BITS_PER_LONG - (start % BITS_PER_LONG);
	unsigned long mask_to_clear = BITMAP_FIRST_WORD_MASK(start);

	while (nr - bits_to_clear >= 0) {
		*p &= ~mask_to_clear;
		nr -= bits_to_clear;
		bits_to_clear = BITS_PER_LONG;
		mask_to_clear = ~0UL;
		p++;
	}
	if (nr) {
		mask_to_clear &= BITMAP_LAST_WORD_MASK(size);
		*p &= ~mask_to_clear;
	}
}
EXPORT_SYMBOL(bitmap_clear);

unsigned long bitmap_find_next_zero_area(unsigned long *map,
					 unsigned long size,
					 unsigned long start,
					 unsigned int nr,
					 unsigned long align_mask)
{
	unsigned long index, end, i;
again:
	index = find_next_zero_bit(map, size, start);

	/* Align allocation */
	index = __ALIGN_MASK(index, align_mask);

	end = index + nr;
	if (end > size)
		return end;
	i = find_next_bit(map, end, index);
	if (i < end) {
		start = i + 1;
		goto again;
	}
	return index;
}
EXPORT_SYMBOL(bitmap_find_next_zero_area);


#define CHUNKSZ				32
#define nbits_to_hold_value(val)	fls(val)
#define unhex(c)			(isdigit(c) ? (c - '0') : (toupper(c) - 'A' + 10))
#define BASEDEC 10		/* fancier cpuset lists input in decimal */

int bitmap_scnprintf(char *buf, unsigned int buflen,
	const unsigned long *maskp, int nmaskbits)
{
	int i, word, bit, len = 0;
	unsigned long val;
	const char *sep = "";
	int chunksz;
	u32 chunkmask;

	chunksz = nmaskbits & (CHUNKSZ - 1);
	if (chunksz == 0)
		chunksz = CHUNKSZ;

	i = ALIGN(nmaskbits, CHUNKSZ) - CHUNKSZ;
	for (; i >= 0; i -= CHUNKSZ) {
		chunkmask = ((1ULL << chunksz) - 1);
		word = i / BITS_PER_LONG;
		bit = i % BITS_PER_LONG;
		val = (maskp[word] >> bit) & chunkmask;
		len += scnprintf(buf+len, buflen-len, "%s%0*lx", sep,
			(chunksz+3)/4, val);
		chunksz = CHUNKSZ;
		sep = ",";
	}
	return len;
}
EXPORT_SYMBOL(bitmap_scnprintf);

int __bitmap_parse(const char *buf, unsigned int buflen,
		int is_user, unsigned long *maskp,
		int nmaskbits)
{
	int c, old_c, totaldigits, ndigits, nchunks, nbits;
	u32 chunk;
	const char __user *ubuf = buf;

	bitmap_zero(maskp, nmaskbits);

	nchunks = nbits = totaldigits = c = 0;
	do {
		chunk = ndigits = 0;

		/* Get the next chunk of the bitmap */
		while (buflen) {
			old_c = c;
			if (is_user) {
				if (__get_user(c, ubuf++))
					return -EFAULT;
			}
			else
				c = *buf++;
			buflen--;
			if (isspace(c))
				continue;

			/*
			 * If the last character was a space and the current
			 * character isn't '\0', we've got embedded whitespace.
			 * This is a no-no, so throw an error.
			 */
			if (totaldigits && c && isspace(old_c))
				return -EINVAL;

			/* A '\0' or a ',' signal the end of the chunk */
			if (c == '\0' || c == ',')
				break;

			if (!isxdigit(c))
				return -EINVAL;

			/*
			 * Make sure there are at least 4 free bits in 'chunk'.
			 * If not, this hexdigit will overflow 'chunk', so
			 * throw an error.
			 */
			if (chunk & ~((1UL << (CHUNKSZ - 4)) - 1))
				return -EOVERFLOW;

			chunk = (chunk << 4) | unhex(c);
			ndigits++; totaldigits++;
		}
		if (ndigits == 0)
			return -EINVAL;
		if (nchunks == 0 && chunk == 0)
			continue;

		__bitmap_shift_left(maskp, maskp, CHUNKSZ, nmaskbits);
		*maskp |= chunk;
		nchunks++;
		nbits += (nchunks == 1) ? nbits_to_hold_value(chunk) : CHUNKSZ;
		if (nbits > nmaskbits)
			return -EOVERFLOW;
	} while (buflen && c == ',');

	return 0;
}
EXPORT_SYMBOL(__bitmap_parse);

int bitmap_parse_user(const char __user *ubuf,
			unsigned int ulen, unsigned long *maskp,
			int nmaskbits)
{
	if (!access_ok(VERIFY_READ, ubuf, ulen))
		return -EFAULT;
	return __bitmap_parse((const char *)ubuf, ulen, 1, maskp, nmaskbits);
}
EXPORT_SYMBOL(bitmap_parse_user);

static inline int bscnl_emit(char *buf, int buflen, int rbot, int rtop, int len)
{
	if (len > 0)
		len += scnprintf(buf + len, buflen - len, ",");
	if (rbot == rtop)
		len += scnprintf(buf + len, buflen - len, "%d", rbot);
	else
		len += scnprintf(buf + len, buflen - len, "%d-%d", rbot, rtop);
	return len;
}

int bitmap_scnlistprintf(char *buf, unsigned int buflen,
	const unsigned long *maskp, int nmaskbits)
{
	int len = 0;
	/* current bit is 'cur', most recently seen range is [rbot, rtop] */
	int cur, rbot, rtop;

	if (buflen == 0)
		return 0;
	buf[0] = 0;

	rbot = cur = find_first_bit(maskp, nmaskbits);
	while (cur < nmaskbits) {
		rtop = cur;
		cur = find_next_bit(maskp, nmaskbits, cur+1);
		if (cur >= nmaskbits || cur > rtop + 1) {
			len = bscnl_emit(buf, buflen, rbot, rtop, len);
			rbot = cur;
		}
	}
	return len;
}
EXPORT_SYMBOL(bitmap_scnlistprintf);

int bitmap_parselist(const char *bp, unsigned long *maskp, int nmaskbits)
{
	unsigned a, b;

	bitmap_zero(maskp, nmaskbits);
	do {
		if (!isdigit(*bp))
			return -EINVAL;
		b = a = simple_strtoul(bp, (char **)&bp, BASEDEC);
		if (*bp == '-') {
			bp++;
			if (!isdigit(*bp))
				return -EINVAL;
			b = simple_strtoul(bp, (char **)&bp, BASEDEC);
		}
		if (!(a <= b))
			return -EINVAL;
		if (b >= nmaskbits)
			return -ERANGE;
		while (a <= b) {
			set_bit(a, maskp);
			a++;
		}
		if (*bp == ',')
			bp++;
	} while (*bp != '\0' && *bp != '\n');
	return 0;
}
EXPORT_SYMBOL(bitmap_parselist);

static int bitmap_pos_to_ord(const unsigned long *buf, int pos, int bits)
{
	int i, ord;

	if (pos < 0 || pos >= bits || !test_bit(pos, buf))
		return -1;

	i = find_first_bit(buf, bits);
	ord = 0;
	while (i < pos) {
		i = find_next_bit(buf, bits, i + 1);
	     	ord++;
	}
	BUG_ON(i != pos);

	return ord;
}

static int bitmap_ord_to_pos(const unsigned long *buf, int ord, int bits)
{
	int pos = 0;

	if (ord >= 0 && ord < bits) {
		int i;

		for (i = find_first_bit(buf, bits);
		     i < bits && ord > 0;
		     i = find_next_bit(buf, bits, i + 1))
	     		ord--;
		if (i < bits && ord == 0)
			pos = i;
	}

	return pos;
}

void bitmap_remap(unsigned long *dst, const unsigned long *src,
		const unsigned long *old, const unsigned long *new,
		int bits)
{
	int oldbit, w;

	if (dst == src)		/* following doesn't handle inplace remaps */
		return;
	bitmap_zero(dst, bits);

	w = bitmap_weight(new, bits);
	for_each_set_bit(oldbit, src, bits) {
	     	int n = bitmap_pos_to_ord(old, oldbit, bits);

		if (n < 0 || w == 0)
			set_bit(oldbit, dst);	/* identity map */
		else
			set_bit(bitmap_ord_to_pos(new, n % w, bits), dst);
	}
}
EXPORT_SYMBOL(bitmap_remap);

int bitmap_bitremap(int oldbit, const unsigned long *old,
				const unsigned long *new, int bits)
{
	int w = bitmap_weight(new, bits);
	int n = bitmap_pos_to_ord(old, oldbit, bits);
	if (n < 0 || w == 0)
		return oldbit;
	else
		return bitmap_ord_to_pos(new, n % w, bits);
}
EXPORT_SYMBOL(bitmap_bitremap);

void bitmap_onto(unsigned long *dst, const unsigned long *orig,
			const unsigned long *relmap, int bits)
{
	int n, m;       	/* same meaning as in above comment */

	if (dst == orig)	/* following doesn't handle inplace mappings */
		return;
	bitmap_zero(dst, bits);

	/*
	 * The following code is a more efficient, but less
	 * obvious, equivalent to the loop:
	 *	for (m = 0; m < bitmap_weight(relmap, bits); m++) {
	 *		n = bitmap_ord_to_pos(orig, m, bits);
	 *		if (test_bit(m, orig))
	 *			set_bit(n, dst);
	 *	}
	 */

	m = 0;
	for_each_set_bit(n, relmap, bits) {
		/* m == bitmap_pos_to_ord(relmap, n, bits) */
		if (test_bit(m, orig))
			set_bit(n, dst);
		m++;
	}
}
EXPORT_SYMBOL(bitmap_onto);

void bitmap_fold(unsigned long *dst, const unsigned long *orig,
			int sz, int bits)
{
	int oldbit;

	if (dst == orig)	/* following doesn't handle inplace mappings */
		return;
	bitmap_zero(dst, bits);

	for_each_set_bit(oldbit, orig, bits)
		set_bit(oldbit % sz, dst);
}
EXPORT_SYMBOL(bitmap_fold);


enum {
	REG_OP_ISFREE,		/* true if region is all zero bits */
	REG_OP_ALLOC,		/* set all bits in region */
	REG_OP_RELEASE,		/* clear all bits in region */
};

static int __reg_op(unsigned long *bitmap, int pos, int order, int reg_op)
{
	int nbits_reg;		/* number of bits in region */
	int index;		/* index first long of region in bitmap */
	int offset;		/* bit offset region in bitmap[index] */
	int nlongs_reg;		/* num longs spanned by region in bitmap */
	int nbitsinlong;	/* num bits of region in each spanned long */
	unsigned long mask;	/* bitmask for one long of region */
	int i;			/* scans bitmap by longs */
	int ret = 0;		/* return value */

	/*
	 * Either nlongs_reg == 1 (for small orders that fit in one long)
	 * or (offset == 0 && mask == ~0UL) (for larger multiword orders.)
	 */
	nbits_reg = 1 << order;
	index = pos / BITS_PER_LONG;
	offset = pos - (index * BITS_PER_LONG);
	nlongs_reg = BITS_TO_LONGS(nbits_reg);
	nbitsinlong = min(nbits_reg,  BITS_PER_LONG);

	/*
	 * Can't do "mask = (1UL << nbitsinlong) - 1", as that
	 * overflows if nbitsinlong == BITS_PER_LONG.
	 */
	mask = (1UL << (nbitsinlong - 1));
	mask += mask - 1;
	mask <<= offset;

	switch (reg_op) {
	case REG_OP_ISFREE:
		for (i = 0; i < nlongs_reg; i++) {
			if (bitmap[index + i] & mask)
				goto done;
		}
		ret = 1;	/* all bits in region free (zero) */
		break;

	case REG_OP_ALLOC:
		for (i = 0; i < nlongs_reg; i++)
			bitmap[index + i] |= mask;
		break;

	case REG_OP_RELEASE:
		for (i = 0; i < nlongs_reg; i++)
			bitmap[index + i] &= ~mask;
		break;
	}
done:
	return ret;
}

int bitmap_find_free_region(unsigned long *bitmap, int bits, int order)
{
	int pos, end;		/* scans bitmap by regions of size order */

	for (pos = 0 ; (end = pos + (1 << order)) <= bits; pos = end) {
		if (!__reg_op(bitmap, pos, order, REG_OP_ISFREE))
			continue;
		__reg_op(bitmap, pos, order, REG_OP_ALLOC);
		return pos;
	}
	return -ENOMEM;
}
EXPORT_SYMBOL(bitmap_find_free_region);

void bitmap_release_region(unsigned long *bitmap, int pos, int order)
{
	__reg_op(bitmap, pos, order, REG_OP_RELEASE);
}
EXPORT_SYMBOL(bitmap_release_region);

int bitmap_allocate_region(unsigned long *bitmap, int pos, int order)
{
	if (!__reg_op(bitmap, pos, order, REG_OP_ISFREE))
		return -EBUSY;
	__reg_op(bitmap, pos, order, REG_OP_ALLOC);
	return 0;
}
EXPORT_SYMBOL(bitmap_allocate_region);

void bitmap_copy_le(void *dst, const unsigned long *src, int nbits)
{
	unsigned long *d = dst;
	int i;

	for (i = 0; i < nbits/BITS_PER_LONG; i++) {
		if (BITS_PER_LONG == 64)
			d[i] = cpu_to_le64(src[i]);
		else
			d[i] = cpu_to_le32(src[i]);
	}
}
EXPORT_SYMBOL(bitmap_copy_le);
