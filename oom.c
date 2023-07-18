/* Out-of-memory and NULL pointer error exit calls
 *
 * Copyright (C) 2021-2023 by Jody Bruchon <jody@jodybruchon.com>
 * Released under The MIT License
 */

#include <stdio.h>
#include <stdlib.h>
#include "libjodycode.h"

/* Out of memory failure */
extern void jc_oom(const char * const restrict msg)
{
  fprintf(stderr, "\nout of memory: %s\n", msg);
  exit(EXIT_FAILURE);
}

/* Null pointer failure */
extern void jc_nullptr(const char * restrict func)
{
  static const char n[] = "(NULL)";
  if (func == NULL) func = n;
  fprintf(stderr, "\ninternal error: NULL pointer caught at %s\n", func);
  exit(EXIT_FAILURE);
}
