/* Out-of-memory and NULL pointer error exit calls
 *
 * Copyright (C) 2021-2023 by Jody Bruchon <jody@jodybruchon.com>
 * Released under The MIT License
 */

#include <stdio.h>
#include <stdlib.h>
#include "libjodycode.h"

static const char msg_unknown[] = "(unknown)";

/* Out of memory failure */
extern void jc_oom(const char * restrict msg)
{
	if (msg == NULL) msg = msg_unknown;
	fprintf(stderr, "\nout of memory: %s\n", msg);
	exit(EXIT_FAILURE);
}

/* Null pointer failure */
extern void jc_nullptr(const char * restrict func)
{
	if (func == NULL) func = msg_unknown;
	fprintf(stderr, "\ninternal error: NULL pointer caught at %s\n", func);
	exit(EXIT_FAILURE);
}
