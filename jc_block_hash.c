/* jody_hash stub */

#include <sys/types.h>
#include "jody_hash.h"

extern jodyhash_t jc_block_hash(jodyhash_t *data, const jodyhash_t start_hash, const size_t count)
{
	return jody_block_hash(data, start_hash, count);
}
