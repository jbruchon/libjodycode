#include <stddef.h>
#include <stdint.h>
#include "libjodycode.h"

const struct jc_size_suffix jc_size_suffix[] = {
	/* Binary suffixes */
	{ "b", 1 },
	{ "k", 1024 },
	{ "kib", 1024 },
	{ "m", 1048576 },
	{ "mib", 1048576 },
	{ "g", (uint64_t)1048576 * 1024 },
	{ "gib", (uint64_t)1048576 * 1024 },
	{ "t", (uint64_t)1048576 * 1048576 },
	{ "tib", (uint64_t)1048576 * 1048576 },
	{ "p", (uint64_t)1048576 * 1048576 * 1024},
	{ "pib", (uint64_t)1048576 * 1048576 * 1024},
	{ "e", (uint64_t)1048576 * 1048576 * 1048576},
	{ "eib", (uint64_t)1048576 * 1048576 * 1048576},
	/* Decimal suffixes */
	{ "kb", 1000 },
	{ "mb", 1000000 },
	{ "gb", 1000000000 },
	{ "tb", 1000000000000 },
	{ "pb", 1000000000000000 },
	{ "eb", 1000000000000000000 },
	{ NULL, 0 },
};

