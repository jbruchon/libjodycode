/* List of data size suffixes
 *
 * Copyright (C) 2023 by Jody Bruchon <jody@jodybruchon.com>
 * Released under The MIT License
 */

#include <stddef.h>
#include <stdint.h>
#include "libjodycode.h"

const struct jc_size_suffix jc_size_suffix[] = {
	/* Binary suffixes */
	{ "B", 1, 0 },
	{ "K", 1024, 10 },
	{ "KiB", 1024, 10 },
	{ "M", 1048576, 20 },
	{ "MiB", 1048576, 20 },
	{ "G", (uint64_t)1048576 * 1024, 30 },
	{ "GiB", (uint64_t)1048576 * 1024, 30 },
	{ "T", (uint64_t)1048576 * 1048576, 40 },
	{ "TiB", (uint64_t)1048576 * 1048576, 40 },
	{ "P", (uint64_t)1048576 * 1048576 * 1024, 50},
	{ "PiB", (uint64_t)1048576 * 1048576 * 1024, 50},
	{ "E", (uint64_t)1048576 * 1048576 * 1048576, 60},
	{ "EiB", (uint64_t)1048576 * 1048576 * 1048576, 60},
	/* Decimal suffixes */
	{ "KB", 1000, -1 },
	{ "MB", 1000000, -1 },
	{ "GB", 1000000000, -1 },
	{ "TB", 1000000000000, -1 },
	{ "PB", 1000000000000000, -1 },
	{ "EB", 1000000000000000000, -1 },
	{ NULL, 0, -1 },
};
