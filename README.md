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
In libjodycode 2.0 a new version table was introduced that maintains a separate
version number for each logical section of the library. If something in the
library changes in a way that's no longer compatible with previous versions,
this version table paired with the provided "libjodycode_check.c/.h" files will
allow the linked program to verify that the sections of the library it actually
uses have not changed, ignoring the rest. This has a significant advantage over
the simpler whole-API version system because the program itself can detect if
the library it's linked to is still compatible enough to safely continue.

The provided version check code reports detailed information about the problem
in a way that is both understandable by users and informative to maintainers.

libjodycode 3.0 introduced a new "feature level" number which changes on every
revision to the public API. Programs can check this number against the number
that corresponds to the newest library interface that they use. Whenever any
function or variable is added to the public API this number will increase.
To find the number your program should store and check against this number,
find every interface you use documented in FEATURELEVELS.txt and choose the
highest feature level number out of those.



Contact information
-------------------------------------------------------------------------------
General information, help, and tech info: https://www.jdupes.com/

Development, source code, releases: https://codeberg.org/jbruchon/libjodycode

Have a bug report or questions? contact Jody Bruchon <jody@jodybruchon.com>



Legal information and software license
-------------------------------------------------------------------------------
libjodycode is Copyright (C) 2014-2023 by Jody Bruchon <jody@jodybruchon.com>

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
