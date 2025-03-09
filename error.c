/* libjodycode: error strings and printing functions
 *
 * Copyright (C) 2023-2025 by Jody Bruchon <jody@jodybruchon.com>
 * Released under The MIT License
 */

#include <stdint.h>
#include <stdio.h>
#include "libjodycode.h"

int32_t jc_errno;

struct jc_error {
	const char *name;
	const char *desc;
};


#define JC_ERRCNT 14
static const int errcnt = JC_ERRCNT;
static const struct jc_error jc_error_list[JC_ERRCNT + 1] = {
	{ "no_error",    "success" },  // 0 - not a real error
	{ "null",        "function received a bad NULL parameter" },  // 1
	{ "cdotdot",     "jc_collapse_dotdot() call failed" },  // 2
	{ "grn_end_dir", "get_relative_name() result has directory at end" },  // 3
	{ "bad_errnum",  "invalid error number" },  // 4
	{ "bad_argv",    "bad argv pointer" },  // 5
	{ "mb_wc_fail",  "a multibyte/wide char conversion failed" },  // 6
	{ "alarm_fail",  "alarm call failed" },  // 7
	{ "alloc_fail",  "memory allocation failed" },  // 8
	{ "numstrcmp",   "jc_numeric_strcmp() was passed a NULL pointer" },  // 9
	{ "datetime",    "date/time string is invalid" },  // 10
	{ "win32api",    "a Win32 API call failed" },  // 11
	{ "kernelver",   "kernel version is too old" },  // 12
	{ "no_memory",   "memory allocation failed (out of memory)" }, // 13
	{ NULL, NULL },  // 14
};


const char *jc_get_errname(int errnum)
{
	if (errnum > errcnt) return NULL;
	if (errnum < 0) errnum = -errnum;
	return jc_error_list[errnum].name;
}


const char *jc_get_errdesc(int errnum)
{
	if (errnum > errcnt) return NULL;
	if (errnum < 0) errnum = -errnum;
	return jc_error_list[errnum].desc;
}


int jc_print_error(int errnum)
{
	if (errnum > errcnt) return JC_EBADERR;
	if (errnum < 0) errnum = -errnum;
	fprintf(stderr, "error: %s (%s)\n", jc_error_list[errnum].desc, jc_error_list[errnum].name);
	return 0;
}


#ifdef JC_TEST
int main(void)
{
	int i;

	for (i = 0; i < errcnt; i++) printf("[%d] %s: %s\n", i, jc_get_errname(i), jc_get_errdesc(i));
	for (i = 0; i < errcnt; i++) jc_print_error(i);
}
#endif
