/* libjodycode: versioning information
 *
 * Copyright (C) 2023-2025 by Jody Bruchon <jody@jodybruchon.com>
 * Released under The MIT License
 */

#include "libjodycode.h"
#ifdef __linux__
 #include <stdlib.h>
 #include <stdint.h>
 #include <sys/utsname.h>
#endif

/* Static version info constants embedded in the library */
const char *jc_version = LIBJODYCODE_VER;
const char *jc_verdate = LIBJODYCODE_VERDATE;
const int jc_api_version = LIBJODYCODE_API_VERSION;
const int jc_api_featurelevel = LIBJODYCODE_API_FEATURE_LEVEL;
const int jc_jodyhash_version = JODY_HASH_VERSION;
const int jc_windows_unicode = LIBJODYCODE_WINDOWS_UNICODE;


#ifdef __linux__
int jc_get_kernel_version(void)
{
	const int mult[3] = { 1000000, 1000, 1 };
	struct utsname utsname;
	char *p, *endptr;
	int i;
	static int kernel_version = -1;
	int m;

	if (kernel_version != -1) return kernel_version;
	if (uname(&utsname)) return -1;
	kernel_version = 0;
	i = 0; endptr = utsname.release;

	for (m = 0; m < 3; m++) {
		for (p = endptr; (*p < '0' || *p > '9') && *p != '\0'; p++) {}
		i = (int)strtol(p, &endptr, 10);
		if (i < 0 || i > 999) goto error_failed;
		kernel_version += i * mult[m];
	}

	return kernel_version;

error_failed:
	kernel_version = -1;
	return -1;
}		
#endif /* __linux__ */
