/* Detect size of CPU data caches
 * See jody_cacheinfo.c for license information */

#ifndef LIBJODYCODE_H
#define LIBJODYCODE_H

#ifdef __cplusplus
extern "C" {
#endif

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

#ifndef PATHBUF_SIZE
 #define PATHBUF_SIZE 4096
#endif
#ifndef WPATH_MAX
 #define WPATH_MAX 8192
#endif


/* Don't use cacheinfo on Windows */
#ifndef ON_WINDOWS

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
#endif /* ON_WINDOWS */

/* Width of a jody_hash. Changing this will also require
 * changing the width of tail masks to match. */
#ifndef JODY_HASH_WIDTH
#define JODY_HASH_WIDTH 64
#endif

#if JODY_HASH_WIDTH == 64
typedef uint64_t jodyhash_t;
#endif
#if JODY_HASH_WIDTH == 32
typedef uint32_t jodyhash_t;
#endif
#if JODY_HASH_WIDTH == 16
typedef uint16_t jodyhash_t;
#endif

/* Version increments when algorithm changes incompatibly */
#define JODY_HASH_VERSION 5

extern jodyhash_t jc_block_hash(const jodyhash_t * restrict data,
		const jodyhash_t start_hash, const size_t count);

extern void jc_oom(const char * const restrict msg);
extern void jc_nullptr(const char * restrict func);

extern int jc_collapse_dotdot(char * const path);
extern int jc_make_relative_link_name(const char * const src,
                const char * const dest, char * rel_path);

extern int jc_numeric_sort(char * restrict c1,
                char * restrict c2, int sort_direction);


/*** jody_string_malloc ***/

#ifdef DEBUG
extern uintmax_t sma_allocs;
extern uintmax_t sma_free_ignored;
extern uintmax_t sma_free_good;
extern uintmax_t sma_free_merged;
extern uintmax_t sma_free_replaced;
extern uintmax_t sma_free_scanned;
extern uintmax_t sma_free_reclaimed;
extern uintmax_t sma_free_tails;
#endif

extern void *jc_string_malloc(size_t len);
extern void jc_string_free(void * const addr);
extern void jc_string_malloc_destroy(void);


/*** jody_strtoepoch ***/

time_t jc_strtoepoch(const char * const datetime);


/*** jody_win_stat ***/

#ifdef ON_WINDOWS
struct winstat {
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

extern time_t nttime_to_unixtime(const uint64_t * const restrict timestamp);
extern time_t unixtime_to_nttime(const uint64_t * const restrict timestamp);
extern int win_stat(const char * const filename, struct winstat * const restrict buf);
#endif /* ON_WINDOWS */


/*** jody_win_unicode ***/

extern int jc_fwprint(FILE * const restrict stream, const char * const restrict str, const int cr);

#ifdef UNICODE
 extern void jc_slash_convert(char *path);
 extern void jc_widearg_to_argv(int argc, wchar_t **wargv, char **argv);
#else
 #define jc_slash_convert(a)
#endif /* UNICODE */

#endif /* LIBJODYCODE_H */
