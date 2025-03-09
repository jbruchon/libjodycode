/* libjodycode: access() stdio call
 *
 * Copyright (C) 2014-2025 by Jody Bruchon <jody@jodybruchon.com>
 * Released under The MIT License
 */

#include <errno.h>
#include <stdio.h>
#ifndef ON_WINDOWS
 #include <unistd.h>
#endif
#include "likely_unlikely.h"
#include "libjodycode.h"

#ifdef UNICODE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <io.h>
#endif


/* Check file exist/read/write, converting for Windows if necessary */
int jc_access(const char *pathname, int mode)
{
	int retval;
#ifdef UNICODE
	JC_WCHAR_T *widename;
#endif

	if (unlikely(pathname == NULL)) {
		jc_errno = EFAULT;
		return -1;
	}

#ifdef ON_WINDOWS
 #ifdef UNICODE
	if (jc_string_to_wstring(pathname, &widename) != 0) {
		jc_errno = ENOMEM;
		return -1;
	}
	retval = _waccess(widename, mode);
	free(widename);
 #else
	retval = _access(pathname, mode);
 #endif
#else
	retval = access(pathname, mode);
#endif
	if (retval != 0) jc_errno = errno;
	return retval;
}
