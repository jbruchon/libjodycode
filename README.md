Introduction
-------------------------------------------------------------------------------
In a nutshell: libjodycode helps you to write a cross-platform C console
program that works on Windows and handles some Unicode complications properly.

libjodycode is a combination of a C code portability helper library and a set
of useful extras such as a simple hash function. Most of the calls provided by
libjodycode are drop-in replacements for C, SuS, or POSIX calls that don't
work well or don't exist on Windows, mostly because Windows natively works in
UTF-16 rather than UTF-8 like  nearly all other operating systems. Several
minor quirks are abstracted away, such as using CreateFile and FindFirstFile
on Windows to read file information and open directories, or using fopen()
with 'S' in mode strings to indicate an intended sequential access pattern.

It started as a pile of code shared among several programs written by Jody
Bruchon such as imagepile, jdupes, winregfs, and zeromerge. These shared
pieces of code were copied between each program as they were updated. As the
number of programs grew, the copied code was eventually combined into a single
reusable shared library.

Please consider financially supporting continued development of libjodycode
using the links on my home page (Ko-fi, PayPal, SubscribeStar, etc.):

https://www.jodybruchon.com/



Version compatibility
-------------------------------------------------------------------------------
As of libjodycode 3.0, a "feature level" is provided. Whenever anything is
added to the public API, this number increases. Programs can integrate the code
under helper_code/libjodycode_check.{c,h} to dynamically check the version of
libjodycode linked at run time and exit with an error if the feature level is
too low or the major version is not correct. In general, you should check for
the lowest feature level that your program will work with. This can be a bit
difficult to determine, so the feature level in the version of libjodycode you
are building against is a safe choice

To use the `libjodycode_check.c/.h` helper code, copy the files into the
program's code somewhere, add `#include "libjodycode_check.h"` to the main C
file, and add

`if (libjodycode_version_check(verbose, bail) != 0) failure_action();`

somewhere early in `main()`. Set verbose to 1 to output detailed error info
via `fprintf(stderr, ...)` if a bad version is found. Set bail to 1 to have
the check code immediately exit if a bad version is found instead of returning
to the caller with a non-zero return value.



Building information
-------------------------------------------------------------------------------
Some parts of libjodycode use C11 aligned_alloc(). Some users have ancient
toolchains only supporting C99 properly. libjodycode now includes support code
to allow C99 compilation. To build using C99 instead of C11, use USE_C99:

`make USE_C99=1`

You can use link-time optimization (LTO) if your compiler supports it:

`make USE_LTO=1`



Getting started quickly with the libjodycode skeleton
-------------------------------------------------------------------------------
If you want to start coding with libjodycode from scratch, copy everything in
the "skeleton" directory to your program directory. Make sure you place your
program directroy "beside" the libjodycode source (if you're in your program's
project directory, '../libjodycode' is the libjodycode source directory). You
will need to modify almost every file to contain the name of your program, the
correct copyright and license information, and add your own LICENSE.txt file.
You need to apply a version number and version date in version.h and then run
./tune_winres.sh to modify the Windows resource files automatically to pull in
your version information. To test that the template builds correctly, make
sure that you've built libjodycode already, then run 'make static_jc' to build
a dummy test program and try running it.



Contact information
-------------------------------------------------------------------------------
General information, help, and tech info: https://www.jdupes.com/

Development, source code, releases: https://codeberg.org/jbruchon/libjodycode

Have a bug report or questions? contact Jody Bruchon <jody@jodybruchon.com>



Legal information and software license
-------------------------------------------------------------------------------
libjodycode is Copyright (C) 2014-2026 by Jody Bruchon <jody@jodybruchon.com>

The MIT License

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
