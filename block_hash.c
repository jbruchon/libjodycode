/* libjodycode: wrapper stub for jody_hash
 *
 * Copyright (C) 2023-2025 by Jody Bruchon <jody@jodybruchon.com>
 * Released under The MIT License
 */

#include <sys/types.h>
#include "jody_hash.h"
#include "libjodycode.h"

int jc_block_hash(const enum jc_e_hash type, jodyhash_t *data, jodyhash_t *hash, const size_t count)
{
	switch (type) {
	default:
	case NORMAL:
		return jody_block_hash(data, hash, count);
	case ROLLING:
		return jody_rolling_block_hash(data, hash, count);
	}
}
