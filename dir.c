/* libjodycode: directory calls
 *
 * Copyright (C) 2014-2025 by Jody Bruchon <jody@jodybruchon.com>
 * Released under The MIT License
 */

#ifdef ON_WINDOWS
 #define WIN32_LEAN_AND_MEAN
 #include <windows.h>
#else
 #include <dirent.h>
#endif  /* ON_WINDOWS */
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <VersionHelpers.h>
#include "likely_unlikely.h"
#include "libjodycode.h"
#ifdef ON_WINDOWS
 JC_DIR *dirp_head = NULL;
#endif


/* Open a directory; handle Windows doing readdir() equivalent too */
JC_DIR *jc_opendir(const char * restrict path)
{
#ifdef ON_WINDOWS
	JC_DIR *dirp;
	char *tempname, *p;
	WIN32_FIND_DATA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD fffxw_flags = 0xffffffff;
	FINDEX_INFO_LEVELS fffxw_info = 0xff;
 #ifdef UNICODE
	JC_WCHAR_T *widename = NULL;
 #endif

	if (unlikely(path == NULL)) {
		jc_errno = EFAULT;
		return NULL;
	}

	/* Windows XP doesn't support the large fetch option */
	if (unlikely(fffxw_flags > FIND_FIRST_EX_LARGE_FETCH)) {
		fffxw_flags = 0;
		if (!IsWindowsVistaOrGreater()) fffxw_flags = FIND_FIRST_EX_LARGE_FETCH;
		fffxw_info = FindExInfoStandard;
		if (IsWindows7OrGreater()) fffxw_info = FindExInfoBasic;
	}

	tempname = (char *)malloc(JC_PATHBUF_SIZE + 4);
	if (unlikely(tempname == NULL)) goto error_nomem;

	/* Windows requires \* at the end of directory names */
	strncpy_s(tempname, JC_PATHBUF_SIZE, path, JC_PATHBUF_SIZE - 1);
	p = tempname + strlen(tempname) - 1;
	if (*p == '/' || *p == '\\') *p = '\0';
	strncat_s(tempname, JC_PATHBUF_SIZE, "\\*", JC_PATHBUF_SIZE - 1);

 #ifdef UNICODE
	widename = (wchar_t *)malloc(JC_PATHBUF_SIZE + 4);
	if (unlikely(widename == NULL)) goto error_nomem;
	if (unlikely(jc_string_to_wstring(tempname, &widename) != 0)) goto error_nomem;
 #endif  /* UNICODE */

 #ifdef UNICODE
	hFind = FindFirstFileExW(widename, fffxw_info, &ffd, FindExSearchNameMatch, NULL, fffxw_flags);
	free(widename);
 #else
	hFind = FindFirstFileExA(tempname, fffxw_info, &ffd, FindExSearchNameMatch, NULL, fffxw_flags);
 #endif
	free(tempname);
	if (unlikely(hFind == INVALID_HANDLE_VALUE)) goto error_fff;
	if (jc_ffd_to_dirent(&dirp, hFind, &ffd) != 0) return NULL;

	/* attach dirp to a linked list of them */
	dirp->next = dirp_head;
	dirp_head = dirp;

	return dirp;

error_fff:
	jc_errno = jc_GetLastError();
	return NULL;
error_nomem:
	if (tempname != NULL) free(tempname);
#ifdef UNICODE
	if (widename != NULL) free(widename);
#endif
	jc_errno = ENOMEM;
	return NULL;

#else

	JC_DIR *retval;

	retval = opendir(path);
	if (retval == NULL) jc_errno = errno;
	return retval;
#endif /* ON_WINDOWS */
}


/* Extract d_namlen from struct dirent */
size_t jc_get_d_namlen(const struct JC_DIRENT * const restrict dirent)
{
	if (unlikely(dirent == NULL)) goto error_bad_dirent;
#ifdef _DIRENT_HAVE_D_NAMLEN
	return dirent->d_namlen;
#elif defined _DIRENT_HAVE_D_RECLEN
	const size_t base = (sizeof(struct dirent) - sizeof(((struct dirent *)0)->d_name)) - offsetof(struct dirent, d_name) - 1;
	size_t skip;

	skip = dirent->d_reclen - (sizeof(struct dirent) - sizeof(((struct dirent *)0)->d_name));
	if (skip > 0) skip -= base;
	return skip + strlen(dirent->d_name + skip);
#else
	return strlen(dirent->d_name);
#endif
error_bad_dirent:
	jc_errno = EFAULT;
	return 0;
}


/* Open a directory; handle Windows doing readdir() equivalent too */
struct JC_DIRENT *jc_readdir(JC_DIR *dirp)
{
#ifdef ON_WINDOWS
	int i;

	if (unlikely(dirp == NULL)) goto error_bad_dirp;
	
	if (dirp->cached == 1) {
		dirp->cached = 0;
		goto skip_fnf;
	}

	i = FindNextFile(dirp->hFind, &(dirp->ffd));
	if (i == 0) goto error_fnf;
	if (jc_ffd_to_dirent(&dirp, NULL, NULL) != 0) return NULL;

skip_fnf:
	return &(dirp->dirent);

error_bad_dirp:
	jc_errno = EFAULT;
	return NULL;
error_fnf:
	jc_errno = jc_GetLastError();
	return NULL;

#else  /* Non-Windows */

	struct JC_DIRENT *retval;

	errno = 0;
	retval = readdir(dirp);
	if (retval == NULL) jc_errno = errno;
	return retval;
#endif /* ON_WINDOWS */
}



#ifdef ON_WINDOWS
/* De-allocate a directory struct and remove from the list */
static void jc_destroy_dirp(JC_DIR * const restrict dirp)
{
	JC_DIR *prev, *cur;

	if (dirp == NULL) return;

	for (prev = NULL, cur = dirp_head; cur != dirp && cur != NULL; prev = cur, cur = cur->next);
	if (cur != dirp) return;
	if (dirp == dirp_head) {
		dirp_head = cur->next;
		free(dirp);
		return;
	}
	prev->next = cur->next;
	free(dirp);
	return;
}
#endif /* ON_WINDOWS */


/* Close a directory */
int jc_closedir(JC_DIR * const restrict dirp)
{
	int retval;
#ifdef ON_WINDOWS
	if (unlikely(dirp == NULL)) {
		jc_errno = EBADF;
		return -1;
	}

	retval = FindClose(dirp->hFind);
	jc_destroy_dirp(dirp);
	if (retval != 0) jc_errno = jc_GetLastError();
	return -1;

#else

	retval = closedir(dirp);
	if (retval != 0) jc_errno = errno;
	return retval;
#endif /* ON_WINDOWS */
}
