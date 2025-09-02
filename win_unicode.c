/* libjodycode: Windows Unicode support utility code
 *
 * Copyright (C) 2014-2025 by Jody Bruchon <jody@jodybruchon.com>
 * Released under The MIT License
 */

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "likely_unlikely.h"
#include "libjodycode.h"

#ifdef ON_WINDOWS
 #define WIN32_LEAN_AND_MEAN
 #include <windows.h>
 #include <io.h>
 JC_DIR *dirp_head = NULL;
#endif  /* ON_WINDOWS */

#ifdef ON_WINDOWS
/* Convert slashes to backslashes in a file path */
void jc_slash_convert(char *path)
{
	while (*path != '\0') {
		if (*path == '/') *path = '\\';
		path++;
	}
	return;
}
#endif


#ifdef UNICODE
/* Copy a string to a wide string - wstring must be freed by the caller */
int jc_string_to_wstring(const char * const restrict string, JC_WCHAR_T **wstring)
{
	if (unlikely(wstring == NULL)) return JC_ENULL;
	*wstring = (JC_WCHAR_T *)malloc(JC_PATHBUF_SIZE + 4);
	if (unlikely(*wstring == NULL)) return JC_EALLOC;
	if (unlikely(!M2W(string, *wstring))) {
		free(*wstring);
		return JC_EMBWC;
	}
	return 0;
}


/* Copy Windows wide character arguments to UTF-8 */
int jc_widearg_to_argv(int argc, JC_WCHAR_T **wargv, char ***cargv)
{
	char *temp;
	int len;
	char **new_argv;

	if (unlikely(cargv == NULL || wargv == NULL)) return JC_ENULL;

	new_argv = (char **)calloc(1, sizeof(char *) * (size_t)argc);
	if (new_argv == NULL) return JC_ENOMEM;

	temp = (char *)calloc(1, JC_PATHBUF_SIZE + 2);
	if (temp == NULL) return JC_ENOMEM;

	for (int counter = 0; counter < argc; counter++) {
		len = W2M_SIZED(wargv[counter], temp, JC_PATHBUF_SIZE);
		if (unlikely(len < 1)) {
			jc_errno = jc_GetLastError();
			free(temp);
			return JC_EBADARGV;
		}

		new_argv[counter] = (char *)malloc((size_t)len + 2);
		if (unlikely(new_argv[counter] == NULL)) {
			free(temp);
			return JC_EALLOC;
		}
		strncpy_s(new_argv[counter], (size_t)len + 1, temp, _TRUNCATE);
	}
	/* fix up __argv so getopt etc. don't crash */
	__argv = new_argv;
	*cargv = new_argv;
	return 0;
}
#endif /* UNICODE */


/* Set up a Unicode terminal on Windows and detect if stdout is a terminal
 * On non-Windows, only detects if stdout is TTY, other arguments ignored */
int jc_setup_unicode_terminal(int argc, JC_WCHAR_T **wargv, char ***argv, int *stdout_tty)
{
#ifdef ON_WINDOWS
 #ifndef UNICODE
	(void)argc; (void)wargv; (void)argv;
 #endif
	/* Windows buffers our std output; don't let it do that */
	if (setvbuf(stdout, NULL, _IONBF, 0) != 0 || setvbuf(stderr, NULL, _IONBF, 0) != 0)
		return JC_ESETVBUF;
	if (stdout_tty != NULL &&_isatty(_fileno(stdout))) *stdout_tty = 1;
#else
	if (stdout_tty != NULL && isatty(fileno(stdout))) *stdout_tty = 1;
#endif /* ON_WINDOWS */

#ifdef UNICODE
	int wa_err;
	if (wargv == NULL || argv == NULL) return JC_ENULL;
	/* Create a UTF-8 **argv from the wide version */
	*argv = (char **)malloc(sizeof(char *) * (size_t)argc);
	if (!*argv) return JC_ENOMEM;
	wa_err = jc_widearg_to_argv(argc, wargv, argv);
	if (wa_err != 0) return wa_err;
	jc_set_output_modes(JC_MODE_UTF16_TTY, JC_MODE_UTF16_TTY);
#endif /* UNICODE */
	return 0;
}


#ifdef ON_WINDOWS
/* Copy WIN32_FIND_FILE data to DIR data for a JC_DIR
 * The first call will allocate a JC_DIR and copy into it
 * Set hFind/ffd to NULL for subsequent calls on the same dirp */
int jc_ffd_to_dirent(JC_DIR **dirp, HANDLE hFind, WIN32_FIND_DATA *ffd)
{
#ifdef UNICODE
	char *tempname;
#endif
	size_t len;

	if (unlikely(dirp == NULL)) goto error_null;

	if (hFind == NULL) {
		if (unlikely(*dirp == NULL)) goto error_null;
		ffd = &((*dirp)->ffd);
	}

 #ifdef UNICODE
	/* Must count bytes after conversion to allocate correct size */
	tempname = (char *)malloc(JC_PATHBUF_SIZE + 4);
	if (unlikely(tempname == NULL)) goto error_nomem;
	if (unlikely(!W2M(ffd->cFileName, tempname))) goto error_name;
	len = strlen(tempname) + 1;
	*dirp = (JC_DIR *)calloc(1, sizeof(JC_DIR) + len);
	if (unlikely(*dirp == NULL)) goto error_nomem;
	strcpy_s((*dirp)->dirent.d_name, len, tempname);
	free(tempname);
 #else
	len = strlen(ffd->cFileName) + 1;
	*dirp = (JC_DIR *)calloc(1, sizeof(JC_DIR) + len);
	if (unlikely(*dirp == NULL)) goto error_nomem;
	strcpy_s((*dirp)->dirent.d_name, len, ffd->cFileName);
 #endif
	/* (*dirp)->dirent.ino = 0; // implicit via calloc() - Windows Find*File() doesn't return inode numbers */
	(*dirp)->dirent.d_namlen = (uint32_t)len;
	if (unlikely(ffd->dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)) (*dirp)->dirent.d_type = JC_DT_LNK;
	else if (ffd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) (*dirp)->dirent.d_type = JC_DT_DIR;
	else (*dirp)->dirent.d_type = JC_DT_REG;

	/* First call: init ffd/hFind + mark cached FindFirstFile() dirent */
	if (hFind != NULL) {
		memcpy(&((*dirp)->ffd), ffd, sizeof(WIN32_FIND_DATA));
		(*dirp)->hFind = hFind;
		(*dirp)->cached = 1;
	}

	return 0;

#ifdef UNICODE
error_name:
	jc_errno = jc_GetLastError();
#endif
error_nomem:
#ifdef UNICODE
	if (tempname != NULL) free(tempname);
#endif
	jc_errno = ENOMEM;
	return -1;
error_null:
	jc_errno = EFAULT;
	return -1;
}
#endif  /* ON_WINDOWS */
