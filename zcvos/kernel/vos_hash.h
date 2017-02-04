/*
 * vos_hash.h
 *
 * copied this file from Linux source, and made some modifications.
 *
 *
 */

#ifndef VOS_HASH_H_
#define VOS_HASH_H_
/* Fast hashing routine for ints,  longs and pointers.
   (C) 2002 William Lee Irwin III, IBM */

/*
 * Knuth recommends primes in approximately golden ratio to the maximum
 * integer representable by a machine word for multiplicative hashing.
 * Chuck Lever verified the effectiveness of this technique:
 * http://www.citi.umich.edu/techreports/reports/citi-tr-00-1.pdf
 *
 * These primes are chosen to be bit-sparse, that is operations on
 * them can use shifts and additions instead of multiplications for
 * machines where multiplications are slow.
 */

//#include <asm/types.h>
#include "vos_types.h"

/* 2^31 + 2^29 - 2^25 + 2^22 - 2^19 - 2^16 + 1 */
#define GOLDEN_RATIO_PRIME_32 0x9e370001UL
/*  2^63 + 2^61 - 2^57 + 2^54 - 2^51 - 2^18 + 1 */
#define GOLDEN_RATIO_PRIME_64 0x9e37fffffffc0001UL

#if __WORDSIZE == 32 //#if BITS_PER_LONG == 32   //#if BITS_PER_LONG == 32
#define GOLDEN_RATIO_PRIME GOLDEN_RATIO_PRIME_32
#define hash_long(val, bits) hash_32(val, bits)
#elif __WORDSIZE == 64 //#elif  BITS_PER_LONG == 64  //#elif BITS_PER_LONG == 64
#define hash_long(val, bits) hash_64(val, bits)
#define GOLDEN_RATIO_PRIME GOLDEN_RATIO_PRIME_64
#else
#error __WORDSIZE not 32 or 64 //#error BITS_PER_LONG not 32 or 64
#endif /* __WORDSIZE == 32 */

static inline vos_u64 hash_64(vos_u64 val, vos_uint bits)
{
	vos_u64 hash = val;

	/*  Sigh, gcc can't optimise this alone like it does for 32 bits. */
	vos_u64 n = hash;
	n <<= 18;
	hash -= n;
	n <<= 33;
	hash -= n;
	n <<= 3;
	hash += n;
	n <<= 3;
	hash -= n;
	n <<= 4;
	hash += n;
	n <<= 2;
	hash += n;

	/* High bits are more random, so use them. */
	return hash >> (64 - bits);
}

static inline vos_uint hash_32(vos_uint val, vos_uint bits)
{
	/* On some cpus multiply is faster, on others gcc will do shifts */
	vos_uint hash = val * GOLDEN_RATIO_PRIME_32;

	/* High bits are more random, so use them. */
	return hash >> (32 - bits);
}

static inline unsigned long hash_ptr(const void *ptr, unsigned int bits)
{
	return hash_long((unsigned long)ptr, bits);
}
#endif /* VOS_HASH_H_ */
