/* a_ljc_program - insert a description here
 * Copyright (C) 2025 by Some Developer <email@address.com>
 * Distributed under The MIT License
 */

#include <stdio.h>

#include <libjodycode.h>
#include "libjodycode_check.h"
#include "version.h"

/* Detect Windows and modify as needed */
#if defined _WIN32 || defined __CYGWIN__
 #ifndef ON_WINDOWS
  #define ON_WINDOWS 1
 #endif
 #ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MEAN
 #endif
 #include <windows.h>
 #include <io.h>
#else /* Not Windows */
 #ifdef UNICODE
  #error Do not define UNICODE on non-Windows platforms.
  #undef UNICODE
 #endif
#endif /* _WIN32 || __CYGWIN__ */


#ifdef UNICODE
int wmain(int argc, wchar_t **wargv)
#else
int main(int argc, char **argv)
#endif
{
	int stdout_tty = 0;
	/* Insert main()-local variables here */

#ifdef ON_WINDOWS
	/* Windows buffers our std output; don't let it do that */
	if (setvbuf(stdout, NULL, _IONBF, 0) != 0 || setvbuf(stderr, NULL, _IONBF, 0) != 0)
		fprintf(stderr, "warning: setvbuf() failed\n");
	if (_isatty(_fileno(stdout))) stdout_tty = 1;
#else
	if (isatty(fileno(stdout))) stdout_tty = 1;
#endif /* ON_WINDOWS */

	/* Verify libjodycode compatibility before going further */
	if (libjodycode_version_check(1, 0) != 0) {
		/*** Insert version check failure actions here ***/
		exit(EXIT_FAILURE);
	}

/* On Windows, set up the terminal and un-wide argv */
#ifdef ON_WINDOWS
 #ifdef UNICODE
	static char **argv;
	int ut_err = jc_setup_unicode_terminal(argc, wargv, &argv, NULL);
 #else
	int ut_err = jc_setup_unicode_terminal(0, NULL, NULL, NULL);
 #endif /* UNICODE */
	if (ut_err != 0) {
		jc_print_error(ut_err);
		exit(EXIT_FAILURE);
	}
#endif /* ON_WINDOWS */


/***** INSERT YOUR PROGRAM HERE *****/


	exit(EXIT_SUCCESS);
}
