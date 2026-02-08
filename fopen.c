/* libjodycode: fopen() stdio call
 *
 * Copyright (C) 2014-2026 by Jody Bruchon <jody@jodybruchon.com>
 * Released under The MIT License
 */

#include <errno.h>
#include <stdio.h>
#include "likely_unlikely.h"
#include "libjodycode.h"

#ifdef UNICODE
 #define WIN32_LEAN_AND_MEAN
 #include <windows.h>
#endif


/* Open a file, converting the name for Unicode on Windows if necessary */
FILE *jc_fopen(const char *pathname, const JC_WCHAR_T *mode)
{
	FILE *fp;
#ifdef ON_WINDOWS
	errno_t retval;
#endif
#ifdef UNICODE
	JC_WCHAR_T *widename;
#endif

	if (unlikely(pathname == NULL || mode == NULL)) {
		jc_errno = EFAULT;
		return NULL;
	}

#ifdef ON_WINDOWS
 #ifdef UNICODE
	if (jc_string_to_wstring(pathname, &widename) != 0) {
		jc_errno = ENOMEM;
		return NULL;
	}
	retval = _wfopen_s(&fp, widename, mode);
	free(widename);
 #else
	retval = fopen_s(&fp, pathname, mode);
 #endif  /* UNICODE */
	if (retval != 0) jc_errno = errno;
#else
	fp = fopen(pathname, mode);
	if (fp == NULL) jc_errno = errno;
#endif  /* ON_WINDOWS */
	return fp;
}


int jc_fclose(FILE *stream)
{
	return fclose(stream);
}
