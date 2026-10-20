/* libjodycode: feof() stdio call
 *
 * Copyright (C) 2026 by Jody Bruchon <jody@jodybruchon.com>
 * Released under The MIT License
 */

#include <errno.h>
#include <stdio.h>
#include "likely_unlikely.h"
#include "libjodycode.h"


/* Check for end-of-file on a stream, capturing any bad stream pointer into jc_errno */
int jc_feof(FILE *stream)
{
	if (unlikely(stream == NULL)) {
		jc_errno = EFAULT;
		return -1;
	}

	return feof(stream);
}
