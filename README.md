Introduction
-------------------------------------------------------------------------------
libjodycode is a software code library containing code shared among several of
the programs written by Jody Bruchon such as imagepile, jdupes, winregfs, and
zeromerge. These shared pieces of code were copied between each program as
they were updated. As the number of programs increased and keeping these
pieces of code synced became more annoying, the decision was made to combine
all of them into a single reusable shared library.

Please consider financially supporting continued development of libjodycode
using the links on my home page (Ko-fi, PayPal, SubscribeStar, etc.):

https://www.jodybruchon.com/


Version compatibility
-------------------------------------------------------------------------------
libjodycode 3.0 introduced a new "feature level" number which changes on every
revision to the public API. Programs can check this number against the number
that corresponds to the newest library interface that they use. Whenever any
function or variable is added to the public API this number will increase.
To find the number your program should store and check against this number,
find every interface you use documented in FEATURELEVELS.txt and choose the
highest feature level number out of those.

In libjodycode 2.0 a new version table was introduced that maintains a separate
version number for each logical section of the library; this table was removed
in libjodycode 4.0 in favor of the feature level number.

Programs can use the `libjodycode_check.c/.h` helper code provided to check
for linking against an incompatible version of libjodycode. Copy the files to
the program's code base, add `#include "libjodycode_check.h"` to the main C
file, and add

`if (libjodycode_version_check(verbose, bail) != 0) failure_action();`


somewhere early in `main()`. Set verbose to 1 to output detailed error info
via `fprintf(stderr, ...)` if a bad version is found. Set bail to 1 to have
the check code immediately exit if a bad version is found instead of returning
to the caller with a non-zero return value.



Contact information
-------------------------------------------------------------------------------
General information, help, and tech info: https://www.jdupes.com/

Development, source code, releases: https://codeberg.org/jbruchon/libjodycode

Have a bug report or questions? contact Jody Bruchon <jody@jodybruchon.com>



Legal information and software license
-------------------------------------------------------------------------------
libjodycode is Copyright (C) 2014-2025 by Jody Bruchon <jody@jodybruchon.com>

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
