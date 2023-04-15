/* Jody Bruchon's fast hashing function
 *
 * This function was written to generate a fast hash that also has a
 * fairly low collision rate. The collision rate is much higher than
 * a secure hash algorithm, but the calculation is drastically simpler
 * and faster.
 *
 * Copyright (C) 2014-2023 by Jody Bruchon <jody@jodybruchon.com>
 * Released under The MIT License
 */

#include <stdio.h>
#include <stdlib.h>
#include "libjodycode.h"

/* DO NOT modify the shift unless you know what you're doing.
 * This shift was decided upon after lots of testing and
 * changing it will likely cause lots of hash collisions. */
#ifndef JODY_HASH_SHIFT
#define JODY_HASH_SHIFT 14
#endif

#define JH_SHIFT2 ((JODY_HASH_SHIFT * 2) - (((JODY_HASH_SHIFT * 2) > JODY_HASH_WIDTH) * JODY_HASH_WIDTH))

/* The constant value's purpose is to cause each byte in the
 * jodyhash_t word to have a positionally dependent variation.
 * It is injected into the calculation to prevent a string of
 * identical bytes from easily producing an identical hash. */

/* The tail mask table is used for block sizes that are
 * indivisible by the width of a jodyhash_t. It is ANDed with the
 * final jodyhash_t-sized element to zero out data in the buffer
 * that is not part of the data to be hashed. */

/* Set hash parameters based on requested hash width */
#if JODY_HASH_WIDTH == 64
#define JODY_HASH_CONSTANT 0x1f3d5b79ULL
static const jodyhash_t tail_mask[] = {
	0x0000000000000000,
	0x00000000000000ff,
	0x000000000000ffff,
	0x0000000000ffffff,
	0x00000000ffffffff,
	0x000000ffffffffff,
	0x0000ffffffffffff,
	0x00ffffffffffffff,
	0xffffffffffffffff
};
#endif /* JODY_HASH_WIDTH == 64 */
#if JODY_HASH_WIDTH == 32
#define JODY_HASH_CONSTANT 0x1f3d5b79U
static const jodyhash_t tail_mask[] = {
	0x00000000,
	0x000000ff,
	0x0000ffff,
	0x00ffffff,
	0xffffffff
};
#endif /* JODY_HASH_WIDTH == 32 */
#if JODY_HASH_WIDTH == 16
#define JODY_HASH_CONSTANT 0x1f5bU
static const jodyhash_t tail_mask[] = {
	0x0000,
	0x00ff,
	0xffff
};
#endif /* JODY_HASH_WIDTH == 16 */


/* Macros for bitwise rotation */
#define ROL(a)  (jodyhash_t)((a << JODY_HASH_SHIFT) | (a >> ((sizeof(jodyhash_t) * 8) - JODY_HASH_SHIFT)))
#define ROR(a)  (jodyhash_t)((a >> JODY_HASH_SHIFT) | (a << ((sizeof(jodyhash_t) * 8) - JODY_HASH_SHIFT)))
#define ROL2(a) (jodyhash_t)(a << JH_SHIFT2 | (a >> ((sizeof(jodyhash_t) * 8) - JH_SHIFT2)))
#define ROR2(a) (jodyhash_t)(a >> JH_SHIFT2 | (a << ((sizeof(jodyhash_t) * 8) - JH_SHIFT2)))

/* Hash a block of arbitrary size; must be divisible by sizeof(jodyhash_t)
 * The first block should pass a start_hash of zero.
 * All blocks after the first should pass start_hash as the value
 * returned by the last call to this function. This allows hashing
 * of any amount of data. If data is not divisible by the size of
 * jodyhash_t, it is MANDATORY that the caller provide a data buffer
 * which is divisible by sizeof(jodyhash_t). */
extern jodyhash_t jc_block_hash(const jodyhash_t * restrict data,
		const jodyhash_t start_hash, const size_t count)
{
	const jodyhash_t s_constant = ROR2(JODY_HASH_CONSTANT);
	jodyhash_t hash = start_hash;
	jodyhash_t element, partial_constant;
	size_t len;

	/* Don't bother trying to hash a zero-length block */
	if (count == 0) return hash;

	len = count / sizeof(jodyhash_t);
	for (; len > 0; len--) {
		element = *data;
		hash += element;
		hash += JODY_HASH_CONSTANT;
		hash ^= ROR(element);
		hash ^= s_constant;
		hash = ROL2(hash);
		hash += element;
		data++;
	}

	/* Handle data tail (for blocks indivisible by sizeof(jodyhash_t)) */
	len = count & (sizeof(jodyhash_t) - 1);
	if (len) {
		partial_constant = JODY_HASH_CONSTANT & tail_mask[len];
		element = *data & tail_mask[len];
		hash += element;
		hash += partial_constant;
		hash = ROL(hash);
		hash ^= element;
		hash = ROL(hash);
		hash ^= partial_constant;
		hash += element;
	}

	return hash;
}
