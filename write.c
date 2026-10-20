/* libjodycode: write() file descriptor call
 *
 * Copyright (C) 2026 by Jody Bruchon <jody@jodybruchon.com>
 * Released under The MIT License
 */

#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include "likely_unlikely.h"
#include "libjodycode.h"


/* Write to a file descriptor, capturing any error into jc_errno */
ssize_t jc_write(int fd, const void *buf, size_t count)
{
	ssize_t retval;

	if (unlikely(buf == NULL || fd < 0)) {
		jc_errno = EFAULT;
		return -1;
	}

#ifdef ON_WINDOWS
	if (unlikely(count > (size_t)UINT_MAX)) {
		jc_errno = EINVAL;
		return -1;
	}
#endif

	errno = 0;
#ifdef ON_WINDOWS
	retval = write(fd, buf, (unsigned int)count);
#else
	retval = write(fd, buf, count);
#endif
	if (unlikely(retval < 0)) jc_errno = errno;

	return retval;
}