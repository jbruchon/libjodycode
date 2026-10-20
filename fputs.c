/* libjodycode: fputs() stdio call
 *
 * Copyright (C) 2026 by Jody Bruchon <jody@jodybruchon.com>
 * Released under The MIT License
 */

#include <errno.h>
#include <stdio.h>
#include "likely_unlikely.h"
#include "libjodycode.h"


/* Write a string to a stream, capturing any error into jc_errno */
int jc_fputs(const char * restrict s, FILE * restrict stream)
{
	int retval;

	if (unlikely(s == NULL || stream == NULL)) {
		jc_errno = EFAULT;
		return EOF;
	}

	errno = 0;
	retval = fputs(s, stream);
	if (unlikely(retval == EOF)) jc_errno = errno;

	return retval;
}
