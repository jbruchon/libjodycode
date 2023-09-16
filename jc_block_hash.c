/* Wrapper stub for jody_hash
 *
 * Copyright (C) 2023 by Jody Bruchon <jody@jodybruchon.com>
 * Released under The MIT License
 */

#include <sys/types.h>
#include "libjodycode.h"
#include "jody_hash.h"

extern int jc_block_hash(jodyhash_t *data, jodyhash_t *hash, const size_t count)
{
	return jody_block_hash(data, hash, count);
}
