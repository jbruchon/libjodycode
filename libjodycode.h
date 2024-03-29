/* Jody Bruchon's helpful code library header
 * Copyright (C) 2014-2024 by Jody Bruchon <jody@jodybruchon.com>
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
#define LIBJODYCODE_API_VERSION       4
#define LIBJODYCODE_API_FEATURE_LEVEL 4
#define LIBJODYCODE_VER               "4.0"
#define LIBJODYCODE_VERDATE           "2024-02-09"
#ifdef UNICODE
 #define LIBJODYCODE_WINDOWS_UNICODE  1
#else
 #define LIBJODYCODE_WINDOWS_UNICODE  0
#endif


/* Define ON_WINDOWS if not otherwise defined and building on Windows */
#if defined _WIN32 || defined __WIN32 || defined WIN64 || defined __WIN64
 #ifndef ON_WINDOWS
  #define ON_WINDOWS
 #endif
#endif

/* Silence a MSVC++ warning */
#ifdef _MSC_VER
 #pragma warning(disable : 4200)
#endif

#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>

#ifdef UNICODE
 #ifndef JC_PATHBUF_SIZE
  #define JC_PATHBUF_SIZE 65536
 #endif
#else
 #ifndef JC_PATHBUF_SIZE
  #define JC_PATHBUF_SIZE 32768
 #endif
#endif
#ifndef WPATH_MAX
 #define WPATH_MAX JC_PATHBUF_SIZE
#endif

#ifdef ON_WINDOWS
 #ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MAN
 #endif
 #include <windows.h>
 #include <direct.h>
 /* Unicode conversion on Windows */
 #ifndef M2W
  #define M2W(a,b) MultiByteToWideChar(CP_UTF8, 0, a, -1, (LPWSTR)b, WPATH_MAX)
 #endif
 #ifndef M2W_SIZED
  #define M2W_SIZED(a,b,c) MultiByteToWideChar(CP_UTF8, 0, a, -1, (LPWSTR)b, c)
 #endif
 #ifndef W2M
  #define W2M(a,b) WideCharToMultiByte(CP_UTF8, 0, a, -1, (LPSTR)b, WPATH_MAX, NULL, NULL)
 #endif
 #ifndef W2M_SIZED
  #define W2M_SIZED(a,b,c) WideCharToMultiByte(CP_UTF8, 0, a, -1, (LPSTR)b, c, NULL, NULL)
 #endif
 #define jc_GetLastError() (int32_t)GetLastError()
#else
#include <dirent.h>
#include <unistd.h>
#endif /* ON_WINDOWS */


/*** time ***/
#ifdef ON_WINDOWS
struct JC_TIMESPEC {
	time_t tv_sec;
	long tv_nsec;
};
 extern int jc_nttime_to_unixtime(FILETIME *filetime, struct JC_TIMESPEC *unixtime);
 extern int jc_unixtime_to_nttime(struct JC_TIMESPEC *unixtime, FILETIME *filetime);
#else
 #define JC_TIMESPEC timespec
#endif  /* ON_WINDOWS */


/*** stat ***/

#ifdef ON_WINDOWS
struct JC_STAT {
	uint64_t st_ino;
	int64_t st_size;
	uint32_t st_dev;
	uint32_t st_nlink;
	uint32_t st_mode;
	struct JC_TIMESPEC st_atim;
	struct JC_TIMESPEC st_mtim;
	struct JC_TIMESPEC st_ctim;
	/* legacy st_*time should be avoided. Use st_*tim.tv_sec instead. */
};

/* stat() macros for Windows "mode" flags (file attributes) */
 #define JC_S_ISARCHIVE(st_mode) ((st_mode & FILE_ATTRIBUTE_ARCHIVE) ? 1 : 0)
 #define JC_S_ISRO(st_mode) ((st_mode & FILE_ATTRIBUTE_READONLY) ? 1 : 0)
 #define JC_S_ISHIDDEN(st_mode) ((st_mode & FILE_ATTRIBUTE_HIDDEN) ? 1 : 0)
 #define JC_S_ISSYSTEM(st_mode) ((st_mode & FILE_ATTRIBUTE_SYSTEM) ? 1 : 0)
 #define JC_S_ISCRYPT(st_mode) ((st_mode & FILE_ATTRIBUTE_ENCRYPTED) ? 1 : 0)
 #define JC_S_ISDIR(st_mode) ((st_mode & FILE_ATTRIBUTE_DIRECTORY) ? 1 : 0)
 #define JC_S_ISCOMPR(st_mode) ((st_mode & FILE_ATTRIBUTE_COMPRESSED) ? 1 : 0)
 #define JC_S_ISREPARSE(st_mode) ((st_mode & FILE_ATTRIBUTE_REPARSE_POINT) ? 1 : 0)
 #define JC_S_ISSPARSE(st_mode) ((st_mode & FILE_ATTRIBUTE_SPARSE) ? 1 : 0)
 #define JC_S_ISTEMP(st_mode) ((st_mode & FILE_ATTRIBUTE_TEMPORARY) ? 1 : 0)
 #define JC_S_ISREG(st_mode) ((st_mode & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)) ? 0 : 1)
 #define JC_S_ISLNK(st_mode) ((st_mode & FILE_ATTRIBUTE_REPARSE_POINT) ? 1 : 0)
#else
 #include <sys/stat.h>
 #define JC_STAT stat
 #define JC_S_ISARCHIVE(st_mode) 0
 #define JC_S_ISRO(st_mode) 0
 #define JC_S_ISHIDDEN(st_mode) 0
 #define JC_S_ISSYSTEM(st_mode) 0
 #define JC_S_ISCRYPT(st_mode) 0
 #define JC_S_ISDIR(st_mode) S_ISDIR(st_mode)
 #define JC_S_ISCOMPR(st_mode) 0
 #define JC_S_ISREPARSE(st_mode) 0
 #define JC_S_ISSPARSE(st_mode) 0
 #define JC_S_ISTEMP(st_mode) 0
 #define JC_S_ISREG(st_mode) S_ISREG(st_mode)
 #define JC_S_ISLNK(st_mode) S_ISLNK(st_mode)
#endif /* ON_WINDOWS */

#if defined _WIN32 || defined __WIN32 || defined ON_WINDOWS
 #ifdef UNICODE
  #define JC_WCHAR_T wchar_t
  #define JC_FILE_MODE_RDONLY L"rb"
  #define JC_FILE_MODE_WRONLY L"wb"
  #define JC_FILE_MODE_RW L"w+b"
  #define JC_FILE_MODE_RW_EXISTING L"r+b"
  #define JC_FILE_MODE_WRONLY_APPEND L"ab"
  #define JC_FILE_MODE_RW_APPEND L"a+b"
  #define JC_FILE_MODE_RDONLY_SEQ L"rbS"
  #define JC_FILE_MODE_WRONLY_SEQ L"wbS"
  #define JC_FILE_MODE_RW_SEQ L"w+bS"
  #define JC_FILE_MODE_RW_EXISTING_SEQ L"r+bS"
  #define JC_FILE_MODE_WRONLY_APPEND_SEQ L"abS"
  #define JC_FILE_MODE_RW_APPEND_SEQ L"a+bS"
 #else /* Windows, not UNICODE */
  #define JC_WCHAR_T char
  #define JC_FILE_MODE_RDONLY "rb"
  #define JC_FILE_MODE_WRONLY "wb"
  #define JC_FILE_MODE_RW "w+b"
  #define JC_FILE_MODE_RW_EXISTING "r+b"
  #define JC_FILE_MODE_WRONLY_APPEND "ab"
  #define JC_FILE_MODE_RW_APPEND "a+b"
  #define JC_FILE_MODE_RDONLY_SEQ "rbS"
  #define JC_FILE_MODE_WRONLY_SEQ "wbS"
  #define JC_FILE_MODE_RW_SEQ "w+bS"
  #define JC_FILE_MODE_RW_EXISTING_SEQ "r+bS"
  #define JC_FILE_MODE_WRONLY_APPEND_SEQ "abS"
  #define JC_FILE_MODE_RW_APPEND_SEQ "a+bS"
 #endif
 #define JC_F_OK 0
 #define JC_R_OK 4
 #define JC_W_OK 2
 #define JC_X_OK 6
#else /* Not Windows */
 #define JC_WCHAR_T char
 #define JC_FILE_MODE_RDONLY "rb"
 #define JC_FILE_MODE_WRONLY "wb"
 #define JC_FILE_MODE_RW "w+b"
 #define JC_FILE_MODE_RW_EXISTING "r+b"
 #define JC_FILE_MODE_WRONLY_APPEND "ab"
 #define JC_FILE_MODE_RW_APPEND "a+b"
 #define JC_FILE_MODE_RDONLY_SEQ "rb"
 #define JC_FILE_MODE_WRONLY_SEQ "wb"
 #define JC_FILE_MODE_RW_SEQ "w+b"
 #define JC_FILE_MODE_RW_EXISTING_SEQ "r+b"
 #define JC_FILE_MODE_WRONLY_APPEND_SEQ "ab"
 #define JC_FILE_MODE_RW_APPEND_SEQ "a+b"
 #define JC_F_OK F_OK
 #define JC_R_OK R_OK
 #define JC_W_OK W_OK
 #define JC_X_OK X_OK
#endif /* Windows */
extern int jc_stat(const char * const filename, struct JC_STAT * const restrict buf);


/*** dir ***/

/* Directory stream type
 * Must be hijacked because FindFirstFileW() does one readdir() equivalent too
 * When the first file is returned, this entry is removed from the linked list */
#ifdef ON_WINDOWS
typedef struct _JC_DIRENT_T {
	uint64_t d_ino;
	uint32_t d_namlen;  /* we already do a strlen() so may as well pass it on */
	unsigned char d_type;
	char d_name[];
} JC_DIRENT;

typedef struct _JC_DIR_T {
	struct _JC_DIR_T *next;
	int cached;
	HANDLE hFind;
	WIN32_FIND_DATA ffd;
	JC_DIRENT dirent;
} JC_DIR;
 #define JC_DIRENT_HAVE_D_NAMLEN
 #define JC_DIRENT_HAVE_D_TYPE
 #define JC_DT_BLK 6
 #define JC_DT_CHR 2
 #define JC_DT_DIR 4
 #define JC_DT_FIFO 1
 #define JC_DT_LNK 10
 #define JC_DT_REG 8
 #define JC_DT_SOCK 12
 #define JC_DT_UNKNOWN 0
 #define JC_DT_WHT 14
#else
 #ifdef _DIRENT_HAVE_D_NAMLEN
  #define JC_DIRENT_HAVE_D_NAMLEN
 #endif
 #ifdef _DIRENT_HAVE_D_TYPE
  #define JC_DIRENT_HAVE_D_TYPE
 #endif
 #ifdef _DIRENT_HAVE_D_RECLEN
  #define JC_DIRENT_HAVE_D_RECLEN
 #endif
 #ifdef _DIRENT_HAVE_D_OFF
  #define JC_DIRENT_HAVE_D_OFF
 #endif
 #define JC_DIR DIR
 #define JC_DIRENT struct dirent
 #ifdef DT_UNKNOWN  /* Cheap way to detect d_type support in the preprocessor */
  #define JC_DT_BLK DT_BLK
  #define JC_DT_CHR DT_CHR
  #define JC_DT_DIR DT_DIR
  #define JC_DT_FIFO DT_FIFO
  #define JC_DT_LNK DT_LNK
  #define JC_DT_REG DT_REG
  #define JC_DT_SOCK DT_SOCK
  #define JC_DT_UNKNOWN DT_UNKNOWN
  #define JC_DT_WHT DT_WHT
 #endif /* DT_UNKNOWN */
#endif /* ON_WINDOWS */
extern int jc_closedir(JC_DIR *dirp);
extern JC_DIR *jc_opendir(const char * restrict path);
extern JC_DIRENT *jc_readdir(JC_DIR *dirp);


extern int        jc_access(const char *pathname, int mode);
extern char      *jc_getcwd(char *pathname, size_t size);
extern FILE      *jc_fopen(const char *pathname, const JC_WCHAR_T *mode);
extern int        jc_link(const char *path1, const char *path2);
extern size_t     jc_get_d_namlen(JC_DIRENT *dirent);
extern int        jc_rename(const char *oldpath, const char *newpath);
extern int        jc_remove(const char *pathname);


/*** alarm ***/

extern int jc_alarm_ring;
extern int jc_start_alarm(const unsigned int seconds, const int repeat);
extern int jc_stop_alarm(void);


/*** batch ***/

struct jc_fileinfo {
	struct JC_STAT *stat;
	JC_DIRENT *dirent;
	int status;
};

struct jc_fileinfo_batch {
	int count;
	struct jc_fileinfo files[];
};

extern struct jc_fileinfo_batch *jc_fileinfo_batch_alloc(const int filecnt, int stat, int namlen);
extern void jc_fileinfo_batch_free(struct jc_fileinfo_batch *batch);


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

extern int32_t jc_errno;
extern const char *jc_get_errname(int errnum);
extern const char *jc_get_errdesc(int errnum);
extern int jc_print_error(int errnum);

#define JC_ERRORCODE_START 1024

#define JC_ENOERROR   1024
#define JC_ENULL      1025
#define JC_EGETCWD    1026
#define JC_ECDOTDOT   1027
#define JC_EGRNEND    1028
#define JC_EBADERR    1029
#define JC_EBADARGV   1030
#define JC_EMBWC      1031
#define JC_EALARM     1032
#define JC_EALLOC     1033
#define JC_ENUMSTRCMP 1034
#define JC_EDATETIME  1035
#define JC_EWIN32API  1036
#define JC_EKERNVER  1037


/*** jc_fwprint ***/
extern int jc_fwprint(FILE * const restrict stream, const char * const restrict str, const int cr);


/*** jody_hash ***/

/* Version increments when algorithm changes incompatibly */
#ifndef JODY_HASH_VERSION
 #define JODY_HASH_VERSION 7
#endif

/* Width of a jody_hash */
#define JODY_HASH_WIDTH 64
typedef uint64_t jodyhash_t;

enum jc_e_hash { NORMAL, ROLLING };

extern int jc_block_hash(enum jc_e_hash type, jodyhash_t *data, jodyhash_t *hash, const size_t count);


/*** linkfiles ***/

#ifdef __linux__
extern int jc_dedupe(struct jc_fileinfo_batch *batch);
#endif /* __linux__ */


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

/* Numerically-correct string sort with a little extra intelligence
   insensitive: 0 = case-sensitive, 1 = case-insensitive */
extern int jc_numeric_strcmp(const char * restrict c1, const char * restrict c2, const int insensitive);


/*** string ***/

/* Same as str[n]cmp/str[n]casecmp but only checks for equality */
extern int jc_strncaseeq(const char *s1, const char *s2, size_t len);
extern int jc_strcaseeq(const char *s1, const char *s2);
extern int jc_strneq(const char *s1, const char *s2, size_t len);
extern int jc_streq(const char *s1, const char *s2);


/*** strtoepoch ***/

/* Convert a date/time string to seconds since the epoch
 * Format must be "YYYY-MM-DD" or "YYYY-MM-DD HH:MM:SS" */
extern time_t jc_strtoepoch(const char * const datetime);


/*** version ***/

/* libjodycode version information */
extern const char *jc_version;
extern const char *jc_verdate;
extern const int jc_api_version;
extern const int jc_api_featurelevel;
extern const int jc_jodyhash_version;
extern const int jc_windows_unicode;

#ifdef __linux__
 extern int jc_get_kernel_version(void);
#endif

/*** win_unicode ***/

/* Cross-platform help for strings in Unicode mode on Windows
 * On non-Windows platforms a lot of these are just wrappers */

#ifdef ON_WINDOWS
 #define JC_MODE_NO_CHANGE 0  /* Don't change the output mode */
 #define JC_MODE_TEXT      1  /* Set output mode to _O_TEXT */
 #define JC_MODE_BINARY    2  /* Set output mode to _O_BINARY (UTF-8) */
 #define JC_MODE_UTF16     3  /* Set output mode to _O_U16TEXT (UTF-16) */
 #define JC_MODE_UTF16_TTY 4  /* Set non-_O_TEXT output mode based on if it's a terminal or not */

 extern JC_DIR *dirp_head;

 extern int jc_ffd_to_dirent(JC_DIR **dirp, HANDLE hFind, WIN32_FIND_DATA *ffd);
 extern void jc_slash_convert(char *path);
 extern void jc_set_output_modes(int out, int err);

 /* These are used for Unicode output and string work on Windows only */
 #ifdef UNICODE
  extern int jc_string_to_wstring(const char * const restrict string, JC_WCHAR_T **wstring);
  extern int jc_widearg_to_argv(int argc, JC_WCHAR_T **wargv, char **argv);
 #endif /* UNICODE */
#else
 #define jc_slash_convert(a)
 #define jc_set_output_modes(a,b)
#endif

#ifdef __cplusplus
}
#endif

#endif /* LIBJODYCODE_H */
