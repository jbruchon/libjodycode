/* Jody Bruchon's helpful code library header
 * Copyright (C) 2014-2025 by Jody Bruchon <jody@jodybruchon.com>
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
#define LIBJODYCODE_API_FEATURE_LEVEL 5
#define LIBJODYCODE_VER               "4.1"
#define LIBJODYCODE_VERDATE           "2025-09-25"
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

#define JC_PATHBUF_SIZE 32768

#ifdef ON_WINDOWS
 #ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MAN
 #endif
 #include <windows.h>
 #include <direct.h>
 /* Unicode conversion on Windows */
 #ifndef M2W
  #define M2W(a,b) MultiByteToWideChar(CP_UTF8, 0, a, -1, (LPWSTR)b, JC_PATHBUF_SIZE)
 #endif
 #ifndef M2W_SIZED
  #define M2W_SIZED(a,b,c) MultiByteToWideChar(CP_UTF8, 0, a, -1, (LPWSTR)b, c)
 #endif
 #ifndef W2M
  #define W2M(a,b) WideCharToMultiByte(CP_UTF8, 0, a, -1, (LPSTR)b, JC_PATHBUF_SIZE, NULL, FALSE)
 #endif
 #ifndef W2M_SIZED
  #define W2M_SIZED(a,b,c) WideCharToMultiByte(CP_UTF8, 0, a, -1, (LPSTR)b, c, NULL, FALSE)
 #endif
 #define jc_GetLastError() (int32_t)GetLastError()
#else
 #include <dirent.h>
 #include <sys/stat.h>
 #include <unistd.h>
#endif /* ON_WINDOWS */


/* Extend an allocation length to the next 64-bit (8-byte) boundary */
#define JC_EXTEND64(a) (((a) & 0x7) > 0 ? (((a) & (~0x7)) + 8) : (a))


/* File modes and wchar_t type */
/* Uther things depend on this, keep it at the top */
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


/*** time ***/
/* This is out of order because other things depend on it */

#ifdef ON_WINDOWS
struct JC_TIMESPEC {
	time_t tv_sec;
	long tv_nsec;
};
 extern int jc_nttime_to_unixtime(const FILETIME * const restrict filetime, struct JC_TIMESPEC * const restrict unixtime);
 extern int jc_unixtime_to_nttime(const struct JC_TIMESPEC * const restrict unixtime, FILETIME * const restrict filetime);
#else
 #define JC_TIMESPEC timespec
#endif  /* ON_WINDOWS */


/*** stat ***/
/* This is out of order because other things depend on it */

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
 #define JC_S_IFMT FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_READONLY | \
	 FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | \
	 FILE_ATTRIBUTE_ENCRYPTED | FILE_ATTRIBUTE_DIRECTORY | \
	 FILE_ATTRIBUTE_COMPRESSED | FILE_ATTRIBUTE_REPARSE_POINT | \
	 FILE_ATTRIBUTE_SPARSE | FILE_ATTRIBUTE_TEMPORARY
 #define JC_S_IFARCHIVE FILE_ATTRIBUTE_ARCHIVE
 #define JC_S_IFRO FILE_ATTRIBUTE_READONLY
 #define JC_S_IFHIDDEN FILE_ATTRIBUTE_HIDDEN
 #define JC_S_IFSYSTEM FILE_ATTRIBUTE_SYSTEM
 #define JC_S_IFCRYPT FILE_ATTRIBUTE_ENCRYPTED
 #define JC_S_IFDIR FILE_ATTRIBUTE_DIRECTORY
 #define JC_S_IFCOMPR FILE_ATTRIBUTE_COMPRESSED
 #define JC_S_IFREPARSE FILE_ATTRIBUTE_REPARSE_POINT
 #define JC_S_IFSPARSE FILE_ATTRIBUTE_SPARSE
 #define JC_S_IFTEMP FILE_ATTRIBUTE_TEMPORARY
 #define JC_S_IFREG !(FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)
 #define JC_S_IFLNK FILE_ATTRIBUTE_REPARSE_POINT
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
 #define JC_S_IFMT S_IFMT
 #define JC_S_IFARCHIVE 0
 #define JC_S_IFRO 0
 #define JC_S_IFHIDDEN 0
 #define JC_S_IFSYSTEM 0
 #define JC_S_IFCRYPT 0
 #define JC_S_IFDIR S_IFDIR
 #define JC_S_IFCOMPR 0
 #define JC_S_IFREPARSE 0
 #define JC_S_IFSPARSE 0
 #define JC_S_IFTEMP 0
 #define JC_S_IFREG S_IFREG
 #define JC_S_IFBLK S_IFBLK
 #define JC_S_IFCHR S_IFCHR
 #define JC_S_IFIFO S_IFIFO
 #define JC_S_IFSOCK S_IFSOCK
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
extern int jc_stat(const char * const filename, struct JC_STAT * const restrict buf);


/*** access ***/

extern int jc_access(const char *pathname, int mode);


/*** alarm ***/

extern int jc_alarm_ring;
extern int jc_start_alarm(const unsigned int seconds, const int repeat);
extern int jc_stop_alarm(void);


/*** batch ***/

struct jc_fileinfo {
	struct JC_STAT *stat;
	struct JC_DIRENT *dirent;
	int status;
};

struct jc_fileinfo_batch {
	int count;
	struct jc_fileinfo files[];
};

extern struct jc_fileinfo_batch *jc_fileinfo_batch_alloc(const int filecnt, const int stat, const int namlen);
extern void jc_fileinfo_batch_free(struct jc_fileinfo_batch * const restrict batch);


/*** cacheinfo ***/

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

extern struct jc_proc_cacheinfo *jc_get_proc_cacheinfo(int cleanup);


/*** dir ***/

/* Directory stream type
 * Must be hijacked because FindFirstFileW() does one readdir() equivalent too
 * When the first file is returned, this entry is removed from the linked list */
#ifdef ON_WINDOWS
 struct JC_DIRENT {
	uint64_t d_ino;
	uint32_t d_namlen;  /* we already do a strlen() so may as well pass it on */
	unsigned char d_type;
	char d_name[];
 };

 typedef struct _JC_DIR_T {
	struct _JC_DIR_T *next;
	int cached;
	HANDLE hFind;
	WIN32_FIND_DATA ffd;
	struct JC_DIRENT dirent;
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
 #define JC_DIR DIR
 #define JC_DIRENT dirent
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
extern JC_DIR *jc_opendir(const char * restrict path);
extern size_t jc_get_d_namlen(const struct JC_DIRENT * const restrict dirent);
extern struct JC_DIRENT *jc_readdir(JC_DIR * const restrict dirp);
extern int jc_closedir(JC_DIR * const restrict dirp);


/*** error ***/

extern int32_t jc_errno;
extern const char *jc_get_errname(int errnum);
extern const char *jc_get_errdesc(int errnum);
extern int jc_print_error(int errnum);

#define JC_ERRORCODE_START 1024

#define JC_ENOERROR   1024  // 0
#define JC_ENULL      1025
#define JC_ECDOTDOT   1026
#define JC_EGRNEND    1027
#define JC_EBADERR    1028
#define JC_EBADARGV   1029
#define JC_EMBWC      1030
#define JC_EALARM     1031
#define JC_EALLOC     1032
#define JC_ENUMSTRCMP 1033
#define JC_EDATETIME  1034  // 10
#define JC_EWIN32API  1035
#define JC_EKERNVER   1036
#define JC_ENOMEM     1037
#define JC_ESETVBUF   1038


/*** fopen ***/

extern FILE *jc_fopen(const char *pathname, const JC_WCHAR_T *mode);
extern int jc_fclose(FILE *stream);


/*** jc_fwprint ***/

extern int jc_fwprint(FILE * const restrict stream, const char * const restrict str, const int cr);


/*** getcwd ***/

extern char *jc_getcwd(char * const restrict pathname, const size_t size);


/*** jody_hash ***/

#ifndef JODY_HASH_H
 #define JODY_HASH_VERSION 7
 #define JODY_HASH_WIDTH 64
 typedef uint64_t jodyhash_t;
#endif

enum jc_e_hash { NORMAL = 0, ROLLING = 1 };
extern int jc_block_hash(const enum jc_e_hash type, jodyhash_t *data, jodyhash_t *hash, const size_t count);


/*** link ***/

extern int jc_link(const char *path1, const char *path2);


/*** linkfiles ***/

enum jc_e_link { SYMLINK = 0, HARDLINK = 1, REFLINK = 2 };
extern int jc_linkfiles(struct jc_fileinfo_batch * const restrict batch, const enum jc_e_link linktype);


/*** oom ***/

/* Out-of-memory and null pointer error-exit functions */
extern void jc_oom(const char * restrict msg);
extern void jc_nullptr(const char * restrict func);


/*** paths ***/

/* Remove "middle" '..' components in a path: 'foo/../bar/baz' => 'bar/baz' */
extern int jc_collapse_dotdot(char * const path);
/* Given a src and dest path, create a relative path name from src to dest */
extern int jc_make_relative_link_name(const char * const src, const char * const dest, char * rel_path);


/*** rename ***/

extern int jc_rename(const char *oldpath, const char *newpath);


/*** remove ***/

extern int jc_remove(const char *pathname);


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

extern const char *jc_emptystring;

/* string type with length prefix */
typedef struct _JC_STR_T {
	uint64_t len;
	char str[1];
} JC_STR_T;

extern JC_STR_T *jc_str_init(const char *string, uint32_t len);
extern int jc_strncaseeq(const char *s1, const char *s2, const size_t len);
extern int jc_strcaseeq(const char *s1, const char *s2);
extern int jc_strneq(const char *s1, const char *s2, const size_t len);
extern int jc_streq(const char *s1, const char *s2);
extern int jc_strteq(const JC_STR_T *s1, const JC_STR_T *s2);
extern int jc_strtneq(const JC_STR_T *s1, const JC_STR_T *s2, const size_t len);
extern int jc_strtcaseeq(const JC_STR_T *s1, const JC_STR_T *s2);
extern int jc_strtncaseeq(const JC_STR_T *s1, const JC_STR_T *s2, const size_t len);
extern int jc_numeric_strtcmp(const JC_STR_T *c1, const JC_STR_T *c2, const int insensitive);


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
 extern void jc_set_output_modes(const int out, const int err);

 /* These are used for Unicode output and string work on Windows only */
 #ifdef UNICODE
  extern int jc_string_to_wstring(const char * const restrict string, JC_WCHAR_T **wstring);
  extern int jc_widearg_to_argv(int argc, JC_WCHAR_T **wargv, char ***cargv);
 #endif /* UNICODE */
#else
 #define jc_slash_convert(a)
 #define jc_set_output_modes(a,b)
#endif
extern int jc_setup_unicode_terminal(int argc, JC_WCHAR_T **wargv, char ***argv, int *stdout_tty);


#ifdef __cplusplus
}
#endif

#endif /* LIBJODYCODE_H */
