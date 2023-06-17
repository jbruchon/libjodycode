/* libjodycode version function
 *
 * Copyright (C) 2023 by Jody Bruchon <jody@jodybruchon.com>
 * Released under The MIT License
 */

#include "libjodycode.h"

const char *jc_version = LIBJODYCODE_VER;
const char *jc_verdate = LIBJODYCODE_VERDATE;
const int jc_api_version = LIBJODYCODE_API_VERSION;
const int jc_api_featurelevel = LIBJODYCODE_API_FEATURE_LEVEL;
const int jc_jodyhash_version = JODY_HASH_VERSION;

/* API sub-version info array, terminated with 0
 * Valid versions are 1-254. New API sections MUST be added to the end. The
 * maximum value of 255 is used by programs to terminate their check array. */
const unsigned char jc_api_versiontable[] = {
	LIBJODYCODE_CACHEINFO_VER,
	LIBJODYCODE_JODY_HASH_VER,
	LIBJODYCODE_OOM_VER,
	LIBJODYCODE_PATHS_VER,
	LIBJODYCODE_SIZE_SUFFIX_VER,
	LIBJODYCODE_SORT_VER,
	LIBJODYCODE_STRING_VER,
	LIBJODYCODE_STRTOEPOCH_VER,
	LIBJODYCODE_WIN_STAT_VER,
	LIBJODYCODE_WIN_UNICODE_VER,
	LIBJODYCODE_ERROR_VER,
	LIBJODYCODE_ALARM_VER,
	0
};
