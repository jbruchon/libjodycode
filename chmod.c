/* libjodycode: Windows-native code for setting file permissions
 *
 * Copyright (C) 2016-2026 by Jody Bruchon <jody@jodybruchon.com>
 * Released under The MIT License
 */


#include <errno.h>
#include <stdint.h>
#include <string.h>
#ifndef ON_WINDOWS
 #include <sys/stat.h>
#endif
#include "likely_unlikely.h"
#include "libjodycode.h"

#ifdef ON_WINDOWS
 #ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MEAN
 #endif
 #include <windows.h>
#endif


/* Set file attributes/permissions matching jc_stat() */
int jc_chmod(const char *filename, uint32_t mode)
{
	int retval;
#ifdef ON_WINDOWS
 #ifdef UNICODE
	JC_WCHAR_T *widename;
 #endif
#endif

	if (unlikely(!filename)) {
		jc_errno = EFAULT;
		return -1;
	}

#ifdef ON_WINDOWS
 #ifdef UNICODE
	if (jc_string_to_wstring(filename, &widename) != 0) {
		jc_errno = EFAULT;
		return -1;
	}
	retval = SetFileAttributesW(widename, (DWORD)mode) ? 0 : -1;
	free(widename);
 #else
	retval = SetFileAttributesA(filename, (DWORD)mode) ? 0 : -1;
 #endif
	if (unlikely(retval != 0)) {
		jc_errno = jc_GetLastError();
		return -1;
	}
#else
	retval = chmod(filename, (mode_t)mode);
	if (retval != 0) jc_errno = errno;
#endif
	return retval;
}
