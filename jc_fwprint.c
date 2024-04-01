/* libjodycode: easy wide single-string fprintf()
 *
 * Copyright (C) 2014-2024 by Jody Bruchon <jody@jodybruchon.com>
 * Released under The MIT License
 */

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "likely_unlikely.h"
#include "libjodycode.h"

#ifdef ON_WINDOWS
 #define WIN32_LEAN_AND_MEAN
 #include <windows.h>
 #include <io.h>
 static int jc_out_mode = _O_TEXT;
 static int jc_err_mode = _O_TEXT;
#endif  /* ON_WINDOWS */


#ifdef ON_WINDOWS
/* Set jc_fwprint output modes for stdout/stderr to TEXT, BINARY, or UTF-16
 * 0: no change
 * 1: set to text mode
 * 2: set to binary (UTF-8) mode
 * 3: set to UTF-16 mode
 * 4: set to UTF-16 mode for terminals, otherwise set to binary mode
 */
void jc_set_output_modes(const int out, const int err)
{
	switch (out) {
		default:
		case 0:
			break;
		case 1:
			jc_out_mode = _O_TEXT;
			break;
		case 2:
			jc_out_mode = _O_BINARY;
			break;
		case 3:
			jc_out_mode = _O_U16TEXT;
			break;
		case 4:
			if (!_isatty(_fileno(stdout))) jc_out_mode = _O_BINARY;
			else jc_out_mode = _O_U16TEXT;
			break;
	}

	switch (err) {
		default:
		case 0:
			break;
		case 1:
			jc_err_mode = _O_TEXT;
			break;
		case 2:
			jc_err_mode = _O_BINARY;
			break;
		case 3:
			jc_err_mode = _O_U16TEXT;
			break;
		case 4:
			if (!_isatty(_fileno(stderr))) jc_err_mode = _O_BINARY;
			else jc_err_mode = _O_U16TEXT;
			break;
	}

	return;
}
#endif /* ON_WINDOWS */


/* Print a string that is wide on Windows but normal on POSIX */
int jc_fwprint(FILE * const restrict stream, const char * const restrict str, const int cr)
{
#ifdef UNICODE
	int retval;
	int stream_mode;
	JC_WCHAR_T *wstr;

	stream_mode = (stream == stderr) ? jc_err_mode : jc_out_mode;

	if (stream_mode == _O_U16TEXT) {
		/* Convert to wide string and send to wide console output */
		if (jc_string_to_wstring(str, &wstr) != 0) return JC_EALLOC;
		fflush(stream);
		_setmode(_fileno(stream), stream_mode);
		if (cr == 2) retval = fwprintf(stream, L"%ls%C", wstr, 0);
		else retval = fwprintf(stream, L"%ls%ls", wstr, cr == 1 ? L"\n" : L"");
		fflush(stream);
		_setmode(_fileno(stream), _O_TEXT);
		free(wstr);
		return retval;
	} else {
#endif
		if (cr == 2) return fprintf(stream, "%s%c", str, 0);
		else return fprintf(stream, "%s%s", str, cr == 1 ? "\n" : "");
#ifdef UNICODE
	}
#endif
}
