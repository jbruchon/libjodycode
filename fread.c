/* libjodycode: fread() stdio call
 *
 * Copyright (C) 2026 by Jody Bruchon <jody@jodybruchon.com>
 * Released under The MIT License
 */

#include <errno.h>
#include <stdio.h>
#include "likely_unlikely.h"
#include "libjodycode.h"


/* Read from a stream, capturing any error into jc_errno */
size_t jc_fread(void * restrict ptr, size_t size, size_t nmemb, FILE * restrict stream)
{
	size_t retval;

	if (unlikely(ptr == NULL || stream == NULL)) {
		jc_errno = EFAULT;
		return 0;
	}

	errno = 0;
	retval = fread(ptr, size, nmemb, stream);
	if (unlikely(retval < nmemb)) {
		if (errno != 0) jc_errno = errno;
		else if (ferror(stream)) jc_errno = EIO;
	}

	return retval;
}