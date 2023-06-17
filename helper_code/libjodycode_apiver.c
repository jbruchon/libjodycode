/* Prints linked libjodycode API version/sub-version information
 * Useful for detecting the API that will be linked to by the compiler
 * Released into the public domain by Jody Bruchon <jody@jodybruchon.com */

#include <stdio.h>
#include "libjodycode.h"

int main(void)
{
	printf("API: %d\n", LIBJODYCODE_API_VERSION);
	printf("FEATURE: %d\n", LIBJODYCODE_API_FEATURE_LEVEL);
	printf("VER: %s\n", LIBJODYCODE_VER);
	printf("VERDATE: %s\n", LIBJODYCODE_VERDATE);
	printf("CACHEINFO: %d\n", LIBJODYCODE_CACHEINFO_VER);
	printf("JODY_HASH: %d\n", LIBJODYCODE_JODY_HASH_VER);
	printf("OOM: %d\n", LIBJODYCODE_OOM_VER);
	printf("PATHS: %d\n", LIBJODYCODE_PATHS_VER);
	printf("SIZE_SUFFIX: %d\n", LIBJODYCODE_SIZE_SUFFIX_VER);
	printf("SORT: %d\n", LIBJODYCODE_SORT_VER);
	printf("STRING: %d\n", LIBJODYCODE_STRING_VER);
	printf("STRTOEPOCH: %d\n", LIBJODYCODE_STRTOEPOCH_VER);
	printf("WIN_STAT: %d\n", LIBJODYCODE_WIN_STAT_VER);
	printf("WIN_UNICODE: %d\n", LIBJODYCODE_WIN_UNICODE_VER);
	printf("ERROR: %d\n", LIBJODYCODE_ERROR_VER);
	printf("ALARM: %d\n", LIBJODYCODE_ALARM_VER);
	return 0;
}
