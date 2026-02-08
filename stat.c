/* libjodycode: Windows-native code for getting stat()-like information
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


/* Get stat()-like extra information for a file on Windows */
int jc_stat(const char *filename, struct JC_STAT *buf)
{
	int retval;
#ifdef ON_WINDOWS
	HANDLE hFile = INVALID_HANDLE_VALUE;
	BY_HANDLE_FILE_INFORMATION bhfi;
#endif

	if (unlikely(!buf)) goto error_null_buffer;

#ifdef ON_WINDOWS
 #ifdef UNICODE
	JC_WCHAR_T *widename;

	if (jc_string_to_wstring(filename, &widename) != 0) goto error_string;
	hFile = CreateFileW(widename, 0, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	free(widename);
 #else
	hFile = CreateFileA(filename, 0, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
 #endif
	if (unlikely(hFile == INVALID_HANDLE_VALUE)) goto error_file;
	if (unlikely(GetFileInformationByHandle(hFile, &bhfi) == 0)) goto error_file;

	buf->st_ino = ((uint64_t)(bhfi.nFileIndexHigh) << 32) + (uint64_t)bhfi.nFileIndexLow;
	buf->st_size = ((int64_t)(bhfi.nFileSizeHigh) << 32) + (int64_t)bhfi.nFileSizeLow;
	if (unlikely(jc_nttime_to_unixtime(&(bhfi.ftCreationTime), &(buf->st_ctim)) != 0)) goto error_nttime;
	if (unlikely(jc_nttime_to_unixtime(&(bhfi.ftLastWriteTime), &(buf->st_mtim)) != 0)) goto error_nttime;
	if (unlikely(jc_nttime_to_unixtime(&(bhfi.ftLastAccessTime), &(buf->st_atim)) != 0)) goto error_nttime;
	buf->st_dev = (uint32_t)bhfi.dwVolumeSerialNumber;
	buf->st_nlink = (uint32_t)bhfi.nNumberOfLinks;
	buf->st_mode = (uint32_t)bhfi.dwFileAttributes;

	CloseHandle(hFile);
	retval = 0;
#else
	retval = stat(filename, buf);
	if (retval != 0) jc_errno = errno;
#endif
	return retval;

#ifdef UNICODE
error_string:
#endif
error_null_buffer:
	jc_errno = EFAULT;
	return -1;

#ifdef ON_WINDOWS
error_file:
error_nttime:
	jc_errno = jc_GetLastError();
	if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
	return -1;
#endif
}
