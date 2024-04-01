/* libjodycode: error strings and printing functions
 *
 * Copyright (C) 2023-2024 by Jody Bruchon <jody@jodybruchon.com>
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
	{ "null",        "function got a bad NULL parameter" },  // 1
	{ "getcwd",      "couldn't get the current directory" },  // 2
	{ "cdotdot",     "jc_collapse_dotdot() call failed" },  // 3
	{ "grn_dir_end", "get_relative_name() result has directory at end" },  // 4
	{ "bad_errnum",  "invalid error number" },  // 5
	{ "bad_argv",    "bad argv pointer" },  // 6
	{ "mb_wc_fail",  "a multibyte/wide char conversion failed" },  // 7
	{ "alarm_fail",  "alarm call failed" },  // 8
	{ "alloc_fail",  "memory allocation failed" },  // 9
	{ "numstrcmp",   "jc_numeric_strcmp() was passed a NULL pointer" },  // 10
	{ "datetime",    "date/time string is invalid" },  // 11
	{ "win32api",    "a Win32 API call failed" },  // 12
	{ "kernelver",   "kernel version is too old" },  // 13
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
	static int i;

	for (i = 0; i < errcnt; i++) printf("[%d] %s: %s\n", i, jc_get_errname(i), jc_get_errdesc(i));
	for (i = 0; i < errcnt; i++) jc_print_error(i);
}
#endif
