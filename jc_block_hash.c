/* jody_hash stub */

#include <sys/types.h>
#include "jody_hash.h"

extern int jc_block_hash(jodyhash_t *data, jodyhash_t *hash, const size_t count)
{
	return jody_block_hash(data, hash, count);
}
