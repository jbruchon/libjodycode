/* libjodycode: utime() POSIX standard call
 *
 * Copyright (C) 2020-2026 by Jody Bruchon <jody@jodybruchon.com>
 * Released under The MIT License
 */

#include <errno.h>
#include <string.h>
#include <time.h>
#include "likely_unlikely.h"
#include "libjodycode.h"


/* utime() but for Windows too */
int jc_utime(const char *filename, const struct JC_UTIMBUF *times)
{
#ifdef ON_WINDOWS
	HANDLE hFile = INVALID_HANDLE_VALUE;
	struct JC_TIMESPEC ts;
	FILETIME ft_ac, ft_mod;
	FILETIME *p_ac = NULL;
	FILETIME *p_mod = NULL;
 #ifdef UNICODE
	JC_WCHAR_T *widename;
 #endif
#else
	struct utimbuf tb;
#endif

	if (unlikely(!filename)) {
		jc_errno = EFAULT;
		return -1;
	}

#ifdef ON_WINDOWS
	if (times != NULL) {
		ts.tv_nsec = 0;
		ts.tv_sec = times->actime;
		if (jc_unixtime_to_nttime(&ts, &ft_ac) == 0) p_ac = &ft_ac;
		ts.tv_sec = times->modtime;
		if (jc_unixtime_to_nttime(&ts, &ft_mod) == 0) p_mod = &ft_mod;
	}

 #ifdef UNICODE
	if (jc_string_to_wstring(filename, &widename) != 0) {
		jc_errno = EFAULT;
		return -1;
	}
	hFile = CreateFileW(widename, FILE_WRITE_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	free(widename);
 #else
	hFile = CreateFileA(filename, FILE_WRITE_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
 #endif

	if (unlikely(hFile == INVALID_HANDLE_VALUE)) {
		jc_errno = jc_GetLastError();
		return -1;
	}

	if (unlikely(SetFileTime(hFile, NULL, p_ac, p_mod) == 0)) {
		jc_errno = jc_GetLastError();
		CloseHandle(hFile);
		return -1;
	}

	CloseHandle(hFile);
	return 0;
#else
	if (times != NULL) {
		tb.actime = times->actime;
		tb.modtime = times->modtime;
		if (utime(filename, &tb) != 0) {
			jc_errno = errno;
			return -1;
		}
	} else {
		if (utime(filename, NULL) != 0) {
			jc_errno = errno;
			return -1;
		}
	}
	return 0;
#endif
}
