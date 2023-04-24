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
#include <string.h>
#include "jody_hash.h"
#include "libjodycode.h"

#if (JODY_HASH_WIDTH != 64) || defined(NO_SIMD)
 #undef USE_SSE2
 #ifndef NO_SIMD
  #define NO_SIMD
 #endif
#endif

#if !defined NO_SIMD && defined __SSE2__
 #define USE_SSE2
 #if defined _MSC_VER || defined _WIN32 || defined __MINGW32__
  /* Microsoft C/C++-compatible compiler */
  #include <intrin.h>
  #define aligned_alloc(a,b) _aligned_malloc(b,a)
 #elif defined __GNUC__  && (defined __x86_64__  || defined __i386__ )
  /* GCC-compatible compiler, targeting x86/x86-64 */
  #include <x86intrin.h>
 #endif
#endif


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
	const jodyhash_t s_constant = JH_ROR2(JODY_HASH_CONSTANT);
	jodyhash_t hash = start_hash;
	jodyhash_t element, element2;
	size_t length = 0;

#ifdef USE_SSE2
	union UINT128 {
		__m128i  v128;
		uint64_t v64[2];
	};
	union UINT128 vec_constant, vec_constant_ror2;
	size_t vec_allocsize;
	__m128i *aligned_data, *aligned_data_e = NULL;
	/* Regs 1-12 used in groups of 3; 1=ROR/XOR work, 2=temp, 3=data+constant */
	__m128i v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12;
	__m128i vec_const, vec_ror2;
	uint64_t *ep1;
	uint64_t *ep2;
#endif /* USE_SSE2 */

	/* Don't bother trying to hash a zero-length block */
	if (count == 0) return hash;

#ifdef USE_SSE2
#if defined (__GNUC__)
	__builtin_cpu_init ();
	if (__builtin_cpu_supports ("sse2"))
#endif /* __GNUC__ */
	{
		if (count >= 32) {
			/* Use SSE2 if possible */
			vec_constant.v64[0]      = JODY_HASH_CONSTANT;
			vec_constant.v64[1]      = JODY_HASH_CONSTANT;
			vec_constant_ror2.v64[0] = JODY_HASH_CONSTANT_ROR2;
			vec_constant_ror2.v64[1] = JODY_HASH_CONSTANT_ROR2;
			/* Constants preload */
			vec_const = _mm_load_si128(&vec_constant.v128);
			vec_ror2  = _mm_load_si128(&vec_constant_ror2.v128);
		} else goto skip_sse;

		/* SSE2 for 64 or larger byte chunks */
		if (count >= 64) {
			vec_allocsize =  count & 0xffffffffffffffc0U;
			/* Only handle 64-byte sized chunks and leave the rest */
			aligned_data_e = (__m128i *)aligned_alloc(32, vec_allocsize);
			aligned_data  = (__m128i *)aligned_alloc(32, vec_allocsize);
			if (!aligned_data_e || !aligned_data) goto oom;
			memcpy(aligned_data, data, vec_allocsize);

			ep1 = (uint64_t *)(aligned_data_e);
			ep2 = (uint64_t *)(aligned_data);
			for (size_t i = 0; i < (vec_allocsize / 16); i += 4) {
				v1  = _mm_load_si128(&aligned_data[i]);
				v3  = _mm_load_si128(&aligned_data[i]);
				v4  = _mm_load_si128(&aligned_data[i+1]);
				v6  = _mm_load_si128(&aligned_data[i+1]);
				v7  = _mm_load_si128(&aligned_data[i+2]);
				v9  = _mm_load_si128(&aligned_data[i+2]);
				v10 = _mm_load_si128(&aligned_data[i+3]);
				v12 = _mm_load_si128(&aligned_data[i+3]);

				/* "element2" gets RORed (two logical shifts ORed together) */
				v1  = _mm_srli_epi64(v1, JODY_HASH_SHIFT);
				v2  = _mm_slli_epi64(v3, (64 - JODY_HASH_SHIFT));
				v1  = _mm_or_si128(v1, v2);
				v1  = _mm_xor_si128(v1, vec_ror2);  // XOR against the ROR2 constant
				v4  = _mm_srli_epi64(v4, JODY_HASH_SHIFT);  // Repeat for all vectors
				v5  = _mm_slli_epi64(v6, (64 - JODY_HASH_SHIFT));
				v4  = _mm_or_si128(v4, v5);
				v4  = _mm_xor_si128(v4, vec_ror2);
				v7  = _mm_srli_epi64(v7, JODY_HASH_SHIFT);
				v8  = _mm_slli_epi64(v9, (64 - JODY_HASH_SHIFT));
				v7  = _mm_or_si128(v7, v8);
				v7  = _mm_xor_si128(v7, vec_ror2);
				v10 = _mm_srli_epi64(v10, JODY_HASH_SHIFT);
				v11 = _mm_slli_epi64(v12, (64 - JODY_HASH_SHIFT));
				v10 = _mm_or_si128(v10, v11);
				v10 = _mm_xor_si128(v10, vec_ror2);

				/* Add the constant to "element" */
				v3  = _mm_add_epi64(v3,  vec_const);
				v6  = _mm_add_epi64(v6,  vec_const);
				v9  = _mm_add_epi64(v9,  vec_const);
				v12 = _mm_add_epi64(v12, vec_const);

				/* Store everything */
				_mm_store_si128(&aligned_data[i], v1);
				_mm_store_si128(&aligned_data_e[i], v3);
				_mm_store_si128(&aligned_data[i+1], v4);
				_mm_store_si128(&aligned_data_e[i+1], v6);
				_mm_store_si128(&aligned_data[i+2], v7);
				_mm_store_si128(&aligned_data_e[i+2], v9);
				_mm_store_si128(&aligned_data[i+3], v10);
				_mm_store_si128(&aligned_data_e[i+3], v12);

				/* Perform the rest of the hash normally */
				for (size_t j = 0; j < 8; j++) {
					hash += *ep1;
					hash ^= *ep2;
					hash = JH_ROL2(hash);
					hash += *ep1;
					ep1++;
					ep2++;
				}
			}
			data += vec_allocsize / sizeof(jodyhash_t);
			length = (count - vec_allocsize) / sizeof(jodyhash_t);
			/* Reuse allocations in 32/48 section */
			if (length < (32 / sizeof(jodyhash_t))) {
				free(aligned_data_e);
				free(aligned_data);
			}
		} else {
skip_sse:
			length = count / sizeof(jodyhash_t);
		}

#ifdef USE_SSE_TAIL
		/* SSE2 for 32-byte or 48-byte sized tail */
		if (length >= 32) {
			vec_allocsize = length & 48; // already know it's less than 64
			size_t dqwords = vec_allocsize / 16;
			/* Alloc only if there isn't a leftover block from 64-byte work */
			if (aligned_data_e == NULL) {
				aligned_data_e = (__m128i *)aligned_alloc(32, vec_allocsize);
				aligned_data  = (__m128i *)aligned_alloc(32, vec_allocsize);
				if (!aligned_data_e || !aligned_data) goto oom;
			}
			memcpy(aligned_data, data, vec_allocsize);

			ep1 = (uint64_t *)(aligned_data_e);
			ep2 = (uint64_t *)(aligned_data);
			v1  = _mm_load_si128(&aligned_data[0]);
			v3  = _mm_load_si128(&aligned_data[0]);
			v4  = _mm_load_si128(&aligned_data[1]);
			v6  = _mm_load_si128(&aligned_data[1]);

			/* "element2" gets RORed (two logical shifts ORed together) */
			v1  = _mm_srli_epi64(v1, JODY_HASH_SHIFT);
			v2  = _mm_slli_epi64(v3, (64 - JODY_HASH_SHIFT));
			v1  = _mm_or_si128(v1, v2);
			v1  = _mm_xor_si128(v1, vec_ror2);  // XOR against the ROR2 constant
			v4  = _mm_srli_epi64(v4, JODY_HASH_SHIFT);  // Repeat for all vectors
			v5  = _mm_slli_epi64(v6, (64 - JODY_HASH_SHIFT));
			v4  = _mm_or_si128(v4, v5);
			v4  = _mm_xor_si128(v4, vec_ror2);

			/* Add the constant to "element" */
			v3  = _mm_add_epi64(v3,  vec_const);
			v6  = _mm_add_epi64(v6,  vec_const);

			/* 48 bytes wide? do the last 16 bytes */
			if (dqwords == 3) {
				v7  = _mm_load_si128(&aligned_data[2]);
				v9  = _mm_load_si128(&aligned_data[2]);
				v7  = _mm_srli_epi64(v7, JODY_HASH_SHIFT);
				v8  = _mm_slli_epi64(v9, (64 - JODY_HASH_SHIFT));
				v7  = _mm_or_si128(v7, v8);
				v7  = _mm_xor_si128(v7, vec_ror2);
				v9  = _mm_add_epi64(v9,  vec_const);
				_mm_store_si128(&aligned_data[2], v7);
				_mm_store_si128(&aligned_data_e[2], v9);
			}

			/* Store everything */
			_mm_store_si128(&aligned_data[0], v1);
			_mm_store_si128(&aligned_data_e[0], v3);
			_mm_store_si128(&aligned_data[1], v4);
			_mm_store_si128(&aligned_data_e[1], v6);

			/* Perform the rest of the hash normally */
			for (size_t j = 0; j < dqwords; j++) {
				hash += *ep1;
				hash ^= *ep2;
				hash = JH_ROL2(hash);
				hash += *ep1;
				ep1++;
				ep2++;
			}

			free(aligned_data_e);
			free(aligned_data);
			data += vec_allocsize / sizeof(jodyhash_t);
			length -= dqwords;
		}
#endif /* USE_SSE_TAIL */
	}
#else
	length = count / sizeof(jodyhash_t);
#endif /* USE_SSE2 */

	/* Hash everything (normal) or remaining small tails (SSE2) */
	for (; length > 0; length--) {
		element = *data;
		element2 = JH_ROR(element);
		element2 ^= s_constant;
		element += JODY_HASH_CONSTANT;
		hash += element;
		hash ^= element2;
		hash = JH_ROL2(hash);
		hash += element;
		data++;
	}

	/* Handle data tail (for blocks indivisible by sizeof(jodyhash_t)) */
	length = count & (sizeof(jodyhash_t) - 1);
	if (length) {
		element = *data & tail_mask[length];
		element2 = JH_ROR(element);
		element2 ^= s_constant;
		element += JODY_HASH_CONSTANT;
		hash += element;
		hash ^= element2;
		hash = JH_ROL2(hash);
		hash += element2;
	}

	return hash;
#ifdef USE_SSE2
oom:
#endif
	fprintf(stderr, "out of memory\n");
	exit(EXIT_FAILURE);
}
