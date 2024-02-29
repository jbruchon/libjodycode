/* Jody Bruchon's helpful code library header
 * Copyright (C) 2014-2023 by Jody Bruchon <jody@jodybruchon.com>
 * Licensed under The MIT License
 * Source code: https://codeberg.org/jbruchon/libjodycode
 */

#ifndef LIBJODYCODE_H
#define LIBJODYCODE_H

#ifdef __cplusplus
extern "C" {
#endif

/* libjodycode version information
 * The major/minor version number and API version/revision MUST match!
 * Major version must change whenever an interface incompatibly changes
 * Minor version must change when new interfaces are added
 * Revision version must not change interfaces in any way
 * Revision is optional in version string, so "2.0" is identical to 2.0.0
 * Feature level is incremented whenever the available interfaces change
 * regardless of compatibility; the lowest feature level possible that
 * supports the used interfaces should be chosen by programs that check
 * version information for compatibility. See README for more information. */
#define LIBJODYCODE_API_VERSION       3
#define LIBJODYCODE_API_FEATURE_LEVEL 2
#define LIBJODYCODE_VER               "3.1"
#define LIBJODYCODE_VERDATE           "2023-07-02"

/* API sub-version table
 * This table tells programs about API changes so that programs can detect
 * an incompatible change and warn gracefully instead of crashing or risking
 * damage to user data.
 *
 * REMINDER: additions must be added to version.c and libjodycode_check.c */
#define LIBJODYCODE_CACHEINFO_VER   1
#define LIBJODYCODE_JODY_HASH_VER   2
#define LIBJODYCODE_OOM_VER         1
#define LIBJODYCODE_PATHS_VER       1
#define LIBJODYCODE_SIZE_SUFFIX_VER 1
#define LIBJODYCODE_SORT_VER        1
#define LIBJODYCODE_STRING_VER      1
#define LIBJODYCODE_STRTOEPOCH_VER  1
#define LIBJODYCODE_WIN_STAT_VER    1
#define LIBJODYCODE_WIN_UNICODE_VER 2
#define LIBJODYCODE_ERROR_VER       1
#define LIBJODYCODE_ALARM_VER       1


#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#ifdef ON_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
 #define WIN32_LEAN_AND_MAN
#endif
#include <windows.h>
 /* Unicode conversion on Windows */
 #ifndef M2W
  #define M2W(a,b) MultiByteToWideChar(CP_UTF8, 0, a, -1, (LPWSTR)b, WPATH_MAX)
 #endif
 #ifndef W2M
  #define W2M(a,b) WideCharToMultiByte(CP_UTF8, 0, a, -1, (LPSTR)b, WPATH_MAX, NULL, NULL)
 #endif
#else
 #include <sys/stat.h>
#endif /* ON_WINDOWS */

#ifdef UNICODE
 #ifndef PATHBUF_SIZE
  #define PATHBUF_SIZE 8192
 #endif
#else
 #ifndef PATHBUF_SIZE
  #define PATHBUF_SIZE 4096
 #endif
#endif
#ifndef WPATH_MAX
 #define WPATH_MAX PATHBUF_SIZE
#endif


/*** alarm ***/

extern int jc_alarm_ring;
extern int jc_start_alarm(const unsigned int seconds, const int repeat);
extern int jc_stop_alarm(void);


/*** cacheinfo ***/

/* Don't use cacheinfo on anything but Linux for now */
#ifdef __linux__

/* Cache information structure
 * Split caches populate i/d, unified caches populate non-i/d */
struct jc_proc_cacheinfo {
	size_t l1;
	size_t l1i;
	size_t l1d;
	size_t l2;
	size_t l2i;
	size_t l2d;
	size_t l3;
	size_t l3i;
	size_t l3d;
};

extern void jc_get_proc_cacheinfo(struct jc_proc_cacheinfo *pci);

#else
 #define jc_get_proc_cacheinfo(a)
#endif /* __linux__ */


/*** error ***/

extern const char *jc_get_errname(int errnum);
extern const char *jc_get_errdesc(int errnum);
extern int jc_print_error(int errnum);


/*** jody_hash ***/

/* Version increments when algorithm changes incompatibly */
#ifndef JODY_HASH_VERSION
#define JODY_HASH_VERSION 7
#endif

/* Width of a jody_hash */
#define JODY_HASH_WIDTH 64
typedef uint64_t jodyhash_t;

extern int jc_block_hash(jodyhash_t *data, jodyhash_t *hash, const size_t count);


/*** oom ***/

/* Out-of-memory and null pointer error-exit functions */
extern void jc_oom(const char * restrict msg);
extern void jc_nullptr(const char * restrict func);


/*** paths ***/

/* Remove "middle" '..' components in a path: 'foo/../bar/baz' => 'bar/baz' */
extern int jc_collapse_dotdot(char * const path);
/* Given a src and dest path, create a relative path name from src to dest */
extern int jc_make_relative_link_name(const char * const src, const char * const dest, char * rel_path);


/*** size_suffix ***/
/* Suffix definitions (treat as case-insensitive) */
struct jc_size_suffix {
  const char * const suffix;
  const int64_t multiplier;
  const int shift;
};

extern const struct jc_size_suffix jc_size_suffix[];


/*** sort ***/

/* Numerically-correct string sort with a little extra intelligence */
extern int jc_numeric_sort(char * restrict c1, char * restrict c2, int sort_direction);


/*** string ***/

/* Same as str[n]cmp/str[n]casecmp but only checks for equality */
extern int jc_strncaseeq(const char *s1, const char *s2, size_t len);
extern int jc_strcaseeq(const char *s1, const char *s2);
extern int jc_strneq(const char *s1, const char *s2, size_t len);
extern int jc_streq(const char *s1, const char *s2);


/*** strtoepoch ***/

/* Convert a date/time string to seconds since the epoch
 * Format must be "YYYY-MM-DD" or "YYYY-MM-DD HH:MM:SS" */
time_t jc_strtoepoch(const char * const datetime);


/*** version ***/

/* libjodycode version information */
extern const char *jc_version;
extern const char *jc_verdate;
extern const int jc_api_version;
extern const int jc_api_featurelevel;
extern const int jc_jodyhash_version;
/* This table is used for API compatibility checks */
extern const unsigned char jc_api_versiontable[];


/*** win_stat ***/

/* For Windows: provide stat-style functionality */
#ifdef ON_WINDOWS
struct jc_winstat {
	uint64_t st_ino;
	int64_t st_size;
	uint32_t st_dev;
	uint32_t st_nlink;
	uint32_t st_mode;
	time_t st_ctime;
	time_t st_mtime;
	time_t st_atime;
};

/* stat() macros for Windows "mode" flags (file attributes) */
#define S_ISARCHIVE(st_mode) ((st_mode & FILE_ATTRIBUTE_ARCHIVE) ? 1 : 0)
#define S_ISRO(st_mode) ((st_mode & FILE_ATTRIBUTE_READONLY) ? 1 : 0)
#define S_ISHIDDEN(st_mode) ((st_mode & FILE_ATTRIBUTE_HIDDEN) ? 1 : 0)
#define S_ISSYSTEM(st_mode) ((st_mode & FILE_ATTRIBUTE_SYSTEM) ? 1 : 0)
#define S_ISCRYPT(st_mode) ((st_mode & FILE_ATTRIBUTE_ENCRYPTED) ? 1 : 0)
#define S_ISDIR(st_mode) ((st_mode & FILE_ATTRIBUTE_DIRECTORY) ? 1 : 0)
#define S_ISCOMPR(st_mode) ((st_mode & FILE_ATTRIBUTE_COMPRESSED) ? 1 : 0)
#define S_ISREPARSE(st_mode) ((st_mode & FILE_ATTRIBUTE_REPARSE_POINT) ? 1 : 0)
#define S_ISSPARSE(st_mode) ((st_mode & FILE_ATTRIBUTE_SPARSE) ? 1 : 0)
#define S_ISTEMP(st_mode) ((st_mode & FILE_ATTRIBUTE_TEMPORARY) ? 1 : 0)
#define S_ISREG(st_mode) ((st_mode & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)) ? 0 : 1)

extern time_t jc_nttime_to_unixtime(const uint64_t * const restrict timestamp);
extern time_t jc_unixtime_to_nttime(const uint64_t * const restrict timestamp);
extern int jc_win_stat(const char * const filename, struct jc_winstat * const restrict buf);
#endif /* ON_WINDOWS */


/*** win_unicode ***/

/* Print strings in Unicode mode on Windows */
extern int jc_fwprint(FILE * const restrict stream, const char * const restrict str, const int cr);

#ifdef UNICODE
 extern void jc_slash_convert(char *path);
 extern void jc_set_output_modes(unsigned int modes);
 extern int jc_widearg_to_argv(int argc, wchar_t **wargv, char **argv);
#else
 #define jc_slash_convert(a)
#endif /* UNICODE */

#ifdef __cplusplus
}
#endif

#endif /* LIBJODYCODE_H */
