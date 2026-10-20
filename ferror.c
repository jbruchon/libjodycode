/* libjodycode: ferror() stdio call
 *
 * Copyright (C) 2026 by Jody Bruchon <jody@jodybruchon.com>
 * Released under The MIT License
 */

#include <errno.h>
#include <stdio.h>
#include "likely_unlikely.h"
#include "libjodycode.h"


/* Test for a stream error */
int jc_ferror(FILE *stream)
{
	if (unlikely(stream == NULL)) {
		jc_errno = EFAULT;
		return -1;
	}

	return ferror(stream);
}
