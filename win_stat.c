/*
 * Windows-native code for getting stat()-like information
 *
 * Copyright (C) 2016-2023 by Jody Bruchon <jody@jodybruchon.com>
 * Released under The MIT License
 */

/* This code is only useful on Windows */
#ifdef ON_WINDOWS

#ifndef WIN32_LEAN_AND_MEAN
 #define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <limits.h>
#include <stdint.h>
#include "likely_unlikely.h"
#include "libjodycode.h"

/* Convert NT epoch to UNIX epoch */
extern time_t jc_nttime_to_unixtime(const uint64_t * const restrict timestamp)
{
	uint64_t newstamp;

	memcpy(&newstamp, timestamp, sizeof(uint64_t));
	newstamp /= 10000000LL;
	if (unlikely(newstamp <= 11644473600LL)) return 0;
	newstamp -= 11644473600LL;
	return (time_t)newstamp;
}

/* Convert UNIX epoch to NT epoch */
extern time_t jc_unixtime_to_nttime(const uint64_t * const restrict timestamp)
{
	uint64_t newstamp;

	memcpy(&newstamp, timestamp, sizeof(uint64_t));
	newstamp += 11644473600LL;
	newstamp *= 10000000LL;
	if (unlikely(newstamp <= 11644473600LL)) return 0;
	return (time_t)newstamp;
}

/* Get stat()-like extra information for a file on Windows */
extern int jc_win_stat(const char * const filename, struct jc_winstat * const restrict buf)
{
  HANDLE hFile;
  BY_HANDLE_FILE_INFORMATION bhfi;
  uint64_t timetemp;

#ifdef UNICODE
  static wchar_t wname2[WPATH_MAX];

  if (unlikely(!buf)) return -127;
  if (!M2W(filename,wname2)) return -126;
  hFile = CreateFileW(wname2, 0, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		  FILE_FLAG_BACKUP_SEMANTICS, NULL);
#else
  if (unlikely(!buf)) return -127;
  hFile = CreateFile(filename, 0, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		  FILE_FLAG_BACKUP_SEMANTICS, NULL);
#endif

  if (unlikely(hFile == INVALID_HANDLE_VALUE)) goto failure;
  if (unlikely(!GetFileInformationByHandle(hFile, &bhfi))) goto failure2;

  buf->st_ino = ((uint64_t)(bhfi.nFileIndexHigh) << 32) + (uint64_t)bhfi.nFileIndexLow;
  buf->st_size = ((int64_t)(bhfi.nFileSizeHigh) << 32) + (int64_t)bhfi.nFileSizeLow;
  timetemp = ((uint64_t)(bhfi.ftCreationTime.dwHighDateTime) << 32) + bhfi.ftCreationTime.dwLowDateTime;
  buf->st_ctime = jc_nttime_to_unixtime(&timetemp);
  timetemp = ((uint64_t)(bhfi.ftLastWriteTime.dwHighDateTime) << 32) + bhfi.ftLastWriteTime.dwLowDateTime;
  buf->st_mtime = jc_nttime_to_unixtime(&timetemp);
  timetemp = ((uint64_t)(bhfi.ftLastAccessTime.dwHighDateTime) << 32) + bhfi.ftLastAccessTime.dwLowDateTime;
  buf->st_atime = jc_nttime_to_unixtime(&timetemp);
  buf->st_dev = (uint32_t)bhfi.dwVolumeSerialNumber;
  buf->st_nlink = (uint32_t)bhfi.nNumberOfLinks;
  buf->st_mode = (uint32_t)bhfi.dwFileAttributes;

  CloseHandle(hFile);
  return 0;

failure:
  CloseHandle(hFile);
  return -1;
failure2:
  CloseHandle(hFile);
  return -2;
}

#endif /* ON_WINDOWS */
