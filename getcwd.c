/* libjodycode: getcwd() stdio call
 *
 * Copyright (C) 2014-2026 by Jody Bruchon <jody@jodybruchon.com>
 * Released under The MIT License
 */

#include <errno.h>
#ifndef ON_WINDOWS
 #include <unistd.h>
#endif
#include "likely_unlikely.h"
#include "libjodycode.h"

#ifdef UNICODE
 #define WIN32_LEAN_AND_MEAN
 #include <windows.h>
#endif


/* Check file exist/read/write, converting for Windows if necessary */
char *jc_getcwd(char * const restrict pathname, const size_t size)
{
	char *retval;
#ifdef UNICODE
	const size_t wsize = size * 2;
	JC_WCHAR_T *widename;
	int i;
#endif

	if (unlikely(pathname == NULL)) {
		jc_errno = EFAULT;
		return NULL;
	}

#ifdef ON_WINDOWS
 #ifdef UNICODE
	widename = (JC_WCHAR_T *)calloc(1, wsize);
	if (widename == NULL) {
		jc_errno = ENOMEM;
		return NULL;
	}
	retval = (char *)_wgetcwd(widename, (int)wsize);
	i = W2M_SIZED(widename, pathname, (int)size);
	free(widename);
	if (unlikely(i == 0)) {
		jc_errno = jc_GetLastError();
		return NULL;
	}
 #else
	retval = _getcwd(pathname, (int)size);
 #endif  /* UNICODE */
#else  /* Not Windows */
	retval = getcwd(pathname, size);
#endif  /* ON_WINDOWS */
	if (retval == NULL) jc_errno = errno;
	return retval;
}
