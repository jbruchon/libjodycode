/* a_ljc_program - (insert program description here)
 *
 * NOTE: REMOVE THE BELOW PARAGRAPH WHEN MAKING YOUR OWN PROGRAM!
 * This libjodycode skeleton copy of this file is distributed by Jody Bruchon
 * <jody@jodybruchon.com> under The MIT License, with the exception that you
 * may re-license it to any other license once you
 * modify it for use in your own program.
 *
 * Distributed under (insert license info here)
 * Copyright (C) 20xx by Some Developer <email@address.com>
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
	/* Sample */
	printf("Your libjodycode project '%s' (version %s, %s) works!\n", argv[0], VER, VERDATE);


	exit(EXIT_SUCCESS);
}
