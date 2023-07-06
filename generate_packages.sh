#!/bin/bash

# Generate package folders with variant builds

# Number of parallel make processes
test -z "$PM" && PM=12

NAME="libjodycode"

VER="$(cat libjodycode.h | grep '#define\s\s*LIBJODYCODE_VER .*"' | cut -d\" -f2)"
echo "Program version: $VER"

TA=__NONE__
PKGTYPE=gz
EXT1=.so; EXT2=.a

UNAME_S="$(uname -s | tr '[:upper:]' '[:lower:]')"
UNAME_P="$(uname -p)"
UNAME_M="$(uname -m)"

# Detect macOS
if [ "$UNAME_S" = "darwin" ]
	then
	PKGTYPE=zip
	TA=mac32
	test "$UNAME_M" = "x86_64" && TA=mac64
fi

# Detect Power Macs under macOS
if [[ "$UNAME_P" = "Power Macintosh" || "$UNAME_P" = "powerpc" ]]
	then
	PKGTYPE=zip
	TA=macppc32
	test "$(sysctl hw.cpu64bit_capable)" = "hw.cpu64bit_capable: 1" && TA=macppc64
fi

# Detect Linux
if [ "$UNAME_S" = "linux" ]
	then
	TA="linux-$UNAME_M"
	[ ! -z "$ARCH" ] && TA="linux-$ARCH"
	PKGTYPE=xz
fi

# Fall through - assume Windows
if [ "$TA" = "__NONE__" ]
	then
	PKGTYPE=zip
	TGT=$(gcc -v 2>&1 | grep Target | cut -d\  -f2- | cut -d- -f1)
	test "$TGT" = "i686" && TA=win32
	test "$TGT" = "x86_64" && TA=win64
	test "$UNAME_S" = "MINGW32_NT-5.1" && TA=winxp
	EXT1=".dll"
fi

echo "Target architecture: $TA"
test "$TA" = "__NONE__" && echo "Failed to detect system type" && exit 1
PKGNAME="${NAME}-${VER}-$TA"

echo "Generating package for: $PKGNAME"
mkdir -p "$PKGNAME"
test ! -d "$PKGNAME" && echo "Can't create directory for package" && exit 1
cp CHANGES.txt README.md LICENSE.txt libjodycode.h $PKGNAME/
E1=1
make clean && make -j$PM stripped && cp -R $NAME${EXT1}* $NAME$EXT2 $PKGNAME/ && E1=0
#make clean
[ $E1 -gt 0 ] && echo "Error building packages; aborting." && exit 1
# Make a fat binary on macOS x86_64 if possible
#if [ "$TA" = "mac64" ] && ld -v 2>&1 | grep -q 'archs:.*i386'
#	then
#	ERR=0
#	TYPE=-i386; CE=-m32
#	# On macOS Big Sur (Darwin 20) or higher, try to build a x86_64 + arm64 binary
#	[ $(uname -r | cut -d. -f1) -ge 20 ] && TYPE=-arm64 && CE="-target arm64-apple-macos11"
#	make clean && make -j$PM CFLAGS_EXTRA="$CE" stripped && cp $NAME$EXT1 $PKGNAME/$NAME$EXT$TYPE || ERR=1
#	[ $ERR -eq 0 ] && lipo -create -output $PKGNAME/libjodycode_temp $PKGNAME/$NAME$EXT$TYPE $PKGNAME/$NAME$EXT && mv $PKGNAME/libjodycode_temp $PKGNAME/$NAME$EXT
#	make clean
#	test $ERR -gt 0 && echo "Error building packages; aborting." && exit 1
#	rm -f $PKGNAME/$NAME$EXT$TYPE
#fi
test "$PKGTYPE" = "zip" && zip -9r $PKGNAME.zip $PKGNAME/
test "$PKGTYPE" = "gz"  && tar -c $PKGNAME/ | gzip -9 > $PKGNAME.pkg.tar.gz
test "$PKGTYPE" = "xz"  && tar -c $PKGNAME/ | xz -e > $PKGNAME.pkg.tar.xz
echo "Package generation complete."
