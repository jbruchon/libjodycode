/* libjodycode: strtoul() wrapper call
 *
 * Copyright (C) 2026 by Jody Bruchon <jody@jodybruchon.com>
 * Released under The MIT License
 */

#include <errno.h>
#include <stdlib.h>
#include "likely_unlikely.h"
#include "libjodycode.h"


/* Convert a string to a long integer, capturing any error into jc_errno */
unsigned long jc_strtoul(const char * restrict nptr, char ** restrict endptr, int base)
{
	unsigned long val;

	if (unlikely(nptr == NULL)) {
		jc_errno = EFAULT;
		return 0;
	}

	errno = 0;
	val = strtoul(nptr, endptr, base);
	if (unlikely(errno != 0)) jc_errno = errno;

	return val;
}
