/* libjodycode: link() stdio call
 *
 * Copyright (C) 2014-2026 by Jody Bruchon <jody@jodybruchon.com>
 * Released under The MIT License
 */

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "likely_unlikely.h"
#include "libjodycode.h"

#ifdef UNICODE
 #define WIN32_LEAN_AND_MEAN
 #include <windows.h>
#endif


/* Hard link a file, converting for Windows if necessary */
int jc_link(const char *path1, const char *path2)
{
	int retval = 0;
#ifdef UNICODE
	JC_WCHAR_T *widename1, *widename2;
#endif

	if (unlikely(path1 == NULL || path2 == NULL)) {
		jc_errno = EFAULT;
		return -1;
	}

#ifdef ON_WINDOWS
 #ifdef UNICODE
	if (jc_string_to_wstring(path1, &widename1) != 0 || jc_string_to_wstring(path2, &widename2) != 0) {
		jc_errno = ENOMEM;
		return -1;
	}
	if (CreateHardLinkW((LPCWSTR)widename2, (LPCWSTR)widename1, NULL) == 0) retval = -1;
	free(widename1); free(widename2);
 #else
	if (CreateHardLink(path2, path1, NULL) == 0) retval = -1;
 #endif  /* UNICODE */
	if (retval != 0) jc_errno = jc_GetLastError();
#else
	retval = link(path1, path2);
	if (retval != 0) jc_errno = errno;
#endif  /* ON_WINDOWS */
	return retval;
}
