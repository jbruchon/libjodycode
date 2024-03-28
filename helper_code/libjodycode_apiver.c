/* Prints linked libjodycode API version/sub-version information
 * Useful for detecting the API that will be linked to by the compiler
 * Released into the public domain by Jody Bruchon <jody@jodybruchon.com */

#include <stdio.h>
#include "libjodycode.h"

int main(void)
{
	printf("Version: %s\n", LIBJODYCODE_VER);
	printf("Date: %s\n", LIBJODYCODE_VERDATE);
	printf("API version: %d\n", LIBJODYCODE_API_VERSION);
	printf("API feature level: %d\n", LIBJODYCODE_API_FEATURE_LEVEL);
	printf("Unicode: %d\n", LIBJODYCODE_WINDOWS_UNICODE);
	return 0;
}
