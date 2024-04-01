/* libjodycode: rename() stdio call
 *
 * Copyright (C) 2014-2024 by Jody Bruchon <jody@jodybruchon.com>
 * Released under The MIT License
 */

#include <errno.h>
#include <stdio.h>
#include "likely_unlikely.h"
#include "libjodycode.h"

#ifdef UNICODE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <io.h>
#endif


/* Rename a file, converting for Windows if necessary */
int jc_rename(const char *oldpath, const char *newpath)
{
	int retval;
#ifdef UNICODE
	JC_WCHAR_T *wideold, *widenew;
#endif

	if (unlikely(oldpath == NULL || newpath == NULL)) {
		jc_errno = EFAULT;
		return -1;
	}

#ifdef UNICODE
	if (unlikely(jc_string_to_wstring(oldpath, &wideold) != 0 || jc_string_to_wstring(newpath, &widenew) != 0)) {
		jc_errno = ENOMEM;
		return -1;
	}
	retval = MoveFileW(wideold, widenew) ? 0 : -1;
	free(wideold); free(widenew);
	if (retval != 0) jc_errno = jc_GetLastError();
#else
	retval = rename(oldpath, newpath);
	if (retval != 0) jc_errno = errno;
#endif
	return retval;
}
