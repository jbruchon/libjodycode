/* libjodycode: fgets() stdio call
 *
 * Copyright (C) 2026 by Jody Bruchon <jody@jodybruchon.com>
 * Released under The MIT License
 */

#include <errno.h>
#include <stdio.h>
#include "likely_unlikely.h"
#include "libjodycode.h"


/* Read a string from a stream, capturing any error into jc_errno */
char *jc_fgets(char * restrict s, int size, FILE * restrict stream)
{
	char *retval;

	if (unlikely(s == NULL || stream == NULL || size <= 0)) {
		jc_errno = EFAULT;
		return NULL;
	}

	errno = 0;
	retval = fgets(s, size, stream);
	if (unlikely(retval == NULL && errno != 0)) jc_errno = errno;

	return retval;
}
