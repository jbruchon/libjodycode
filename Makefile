# libjodycode Makefile

CFLAGS ?= -O2 -g
PREFIX ?= /usr/local
PROGRAM_NAME = libjodycode
LIB_DIR ?= $(PREFIX)/lib
INC_DIR ?= $(PREFIX)/include
MAN_BASE_DIR ?= $(PREFIX)/share/man
MAN7_DIR ?= $(MAN_BASE_DIR)/man7
CC ?= gcc
INSTALL = install
RM      = rm -f
LN      = ln -sf
RMDIR   = rmdir -p
MKDIR   = mkdir -p
INSTALL_PROGRAM = $(INSTALL) -m 0755
INSTALL_DATA    = $(INSTALL) -m 0644
SO_SUFFIX = .so
LIB_SUFFIX = .a

# Make Configuration
COMPILER_OPTIONS = -Wall -Wwrite-strings -Wcast-align -Wstrict-aliasing -Wstrict-prototypes -Wpointer-arith -Wundef
COMPILER_OPTIONS += -Wshadow -Wfloat-equal -Waggregate-return -Wcast-qual -Wswitch-default -Wswitch-enum -Wconversion -Wunreachable-code -Wformat=2
COMPILER_OPTIONS += -std=gnu11 -D_FILE_OFFSET_BITS=64 -fstrict-aliasing -pipe -fPIC

UNAME_S       = $(shell uname -s)
UNAME_M       = $(shell uname -m)
VERSION       = $(shell grep -m 1 '^.define LIBJODYCODE_VER ' libjodycode.h | sed 's/[^"]*"//;s/".*//')
VERSION_MAJOR = $(shell grep -m 1 '^.define LIBJODYCODE_VER ' libjodycode.h | sed 's/[^"]*"//;s/\..*//')
SO_VER_FULL   =$(SO_SUFFIX).$(VERSION)
SO_VER_MAJOR  =$(SO_SUFFIX).$(VERSION_MAJOR)
CROSS_DETECT  = $(shell true | $(CC) -dM -E - | grep -m 1 __x86_64 || echo "cross")

# Are we running on a Windows OS?
ifeq ($(OS), Windows_NT)
 ifndef NO_WINDOWS
  ON_WINDOWS=1
  SO_SUFFIX=.dll
  LIB_SUFFIX=.lib
  SO_VER_FULL=$(SO_SUFFIX)
  SO_VER_MAJOR=$(SO_SUFFIX)
 endif
endif

ifeq ($(UNAME_S), Darwin)
 SO_SUFFIX    = .dylib
 SO_VER_FULL  = .$(VERSION)$(SO_SUFFIX)
 SO_VER_MAJOR = .$(VERSION_MAJOR)$(SO_SUFFIX)
 LINK_OPTIONS += -Wl,-install_name,$(PROGRAM_NAME)$(SO_VER_MAJOR)
 # Don't use unsupported compiler options on gcc 3/4 (Mac OS X 10.5.8 Xcode)
 GCCVERSION = $(shell expr `LC_ALL=C gcc -v 2>&1 | grep '[cn][cg] version' | sed 's/[^0-9]*//;s/[ .].*//'` \>= 5)
 STRIP_UNNEEDED = strip -S
 STRIP_DEBUG = strip -S
else
 LINK_OPTIONS += -Wl,-soname,$(PROGRAM_NAME)$(SO_VER_MAJOR)
 GCCVERSION = 1
 STRIP_UNNEEDED = strip --strip-unneeded
 STRIP_DEBUG = strip --strip-debug
endif

ifeq ($(GCCVERSION), 1)
 COMPILER_OPTIONS += -Wextra -Wstrict-overflow=5 -Winit-self
endif

# Debugging code inclusion
ifdef LOUD
 DEBUG=1
 CFLAGS += -DLOUD_DEBUG
endif
ifdef DEBUG
 CFLAGS += -DDEBUG
else
 CFLAGS += -DNDEBUG
endif
ifdef HARDEN
 CFLAGS += -Wformat -Wformat-security -D_FORTIFY_SOURCE=2 -fstack-protector-strong -Wl,-z,relro -Wl,-z,now
endif

# MinGW needs this for printf() conversions to work
ifdef ON_WINDOWS
 ifndef NO_UNICODE
  UNICODE=1
  COMPILER_OPTIONS += -municode
  PROGRAM_SUFFIX=.exe
 endif
 ifeq ($(UNAME_S), MINGW32_NT-5.1)
  OBJS += winres_xp.o
 else
  OBJS += winres.o
 endif
# COMPILER_OPTIONS += -D__USE_MINGW_ANSI_STDIO=1
 COMPILER_OPTIONS += -DON_WINDOWS=1
endif

# Do not build SIMD code if not on x86_64
ifneq ($(UNAME_M), x86_64)
 NO_SIMD=1
endif
ifeq ($(CROSS_DETECT), cross)
 NO_SIMD=1
endif

# SIMD SSE2/AVX2 jody_hash code
ifdef NO_SIMD
 COMPILER_OPTIONS += -DNO_SIMD -DNO_SSE2 -DNO_AVX2
else
 SIMD_OBJS += jody_hash_simd.o
 ifdef NO_SSE2
  COMPILER_OPTIONS += -DNO_SSE2
 else
  SIMD_OBJS += jody_hash_sse2.o
 endif
 ifdef NO_AVX2
  COMPILER_OPTIONS += -DNO_AVX2
 else
  SIMD_OBJS += jody_hash_avx2.o
 endif
endif


CFLAGS += $(COMPILER_OPTIONS) $(CFLAGS_EXTRA)
LDFLAGS += $(LINK_OPTIONS)

# ADDITIONAL_OBJECTS - some platforms will need additional object files
# to support features not supplied by their vendor. Eg: GNU getopt()
#ADDITIONAL_OBJECTS += getopt.o

OBJS += access.o alarm.o batch.o block_hash.o cacheinfo.o dir.o
OBJS += error.o fopen.o jc_fwprint.o getcwd.o jody_hash.o link.o
OBJS += linkfiles.o numstrcmp.o oom.o paths.o
OBJS += remove.o rename.o size_suffix.o stat.o
OBJS += string.o time.o version.o win_unicode.o
OBJS += $(ADDITIONAL_OBJECTS)

all: sharedlib staticlib
	-@test "$(CROSS_DETECT)" = "cross" && echo "NOTICE: SIMD disabled: !x86_64 or a cross-compiler detected (CC = $(CC))" || true

sharedlib: $(OBJS) $(SIMD_OBJS)
	$(CC) -shared -o $(PROGRAM_NAME)$(SO_VER_FULL) $(OBJS) $(SIMD_OBJS) $(LDFLAGS) $(CFLAGS) $(CFLAGS_EXTRA)
	-test "$(ON_WINDOWS)" != "1" && $(LN) $(PROGRAM_NAME)$(SO_VER_FULL) $(PROGRAM_NAME)$(SO_VER_MAJOR)
	-test "$(ON_WINDOWS)" != "1" && $(LN) $(PROGRAM_NAME)$(SO_VER_MAJOR) $(PROGRAM_NAME)$(SO_SUFFIX)

staticlib: $(OBJS) $(SIMD_OBJS)
	$(AR) rcs libjodycode$(LIB_SUFFIX) $(OBJS) $(SIMD_OBJS)

jody_hash_simd.o:
	$(CC) $(CFLAGS) $(COMPILER_OPTIONS) $(WIN_CFLAGS) $(CFLAGS_EXTRA) $(CPPFLAGS) -mavx2 -c -o jody_hash_simd.o jody_hash_simd.c

jody_hash_avx2.o: jody_hash_simd.o
	$(CC) $(CFLAGS) $(COMPILER_OPTIONS) $(WIN_CFLAGS) $(CFLAGS_EXTRA) $(CPPFLAGS) -mavx2 -c -o jody_hash_avx2.o jody_hash_avx2.c

jody_hash_sse2.o: jody_hash_simd.o
	$(CC) $(CFLAGS) $(COMPILER_OPTIONS) $(WIN_CFLAGS) $(CFLAGS_EXTRA) $(CPPFLAGS) -msse2 -c -o jody_hash_sse2.o jody_hash_sse2.c

apiver: helper_code/libjodycode_apiver.c
	$(CC) $(CFLAGS) $(COMPILER_OPTIONS) $(WIN_CFLAGS) $(CFLAGS_EXTRA) -I. -o apiver helper_code/libjodycode_apiver.c

vercheck: helper_code/libjodycode_check.c
	$(CC) $(CFLAGS) $(COMPILER_OPTIONS) $(WIN_CFLAGS) $(CFLAGS_EXTRA) -DJC_TEST -I. -c -o vercheck.o helper_code/libjodycode_check.c
	$(CC) $(CFLAGS) $(COMPILER_OPTIONS) $(WIN_CFLAGS) $(CFLAGS_EXTRA) -I. -L. -Wl,-Bstatic vercheck.o -ljodycode -Wl,-Bdynamic -o vercheck

cacheinfo:
	$(CC) cacheinfo.c -DJC_TEST $(CFLAGS) $(LDFLAGS) -o cacheinfo

.c.o:
	$(CC) -c $(COMPILER_OPTIONS) $(CFLAGS) $(CPPFLAGS) $< -o $@

#manual:
#	gzip -9 < jodycode.8 > jodycode.8.gz

$(PROGRAM_NAME): jodyhash $(OBJS)
#	$(CC) $(CFLAGS) $(LDFLAGS) -o $(PROGRAM_NAME) $(OBJS)

winres.o: winres.rc winres.manifest.xml
	./tune_winres.sh
	windres winres.rc winres.o

winres_xp.o: winres_xp.rc
	./tune_winres.sh
	windres winres_xp.rc winres_xp.o

installdirs:
	test -e $(DESTDIR)$(LIB_DIR) || $(MKDIR) $(DESTDIR)$(LIB_DIR)
	test -e $(DESTDIR)$(INC_DIR) || $(MKDIR) $(DESTDIR)$(INC_DIR)
	test -e $(DESTDIR)$(MAN7_DIR) || $(MKDIR) $(DESTDIR)$(MAN7_DIR)

installfiles: installdirs
	$(INSTALL_PROGRAM) $(PROGRAM_NAME)$(SO_VER_FULL) $(DESTDIR)$(LIB_DIR)/$(PROGRAM_NAME)$(SO_VER_FULL)
	-test "$(ON_WINDOWS)" != "1" && $(LN) $(PROGRAM_NAME)$(SO_VER_FULL) $(DESTDIR)$(LIB_DIR)/$(PROGRAM_NAME)$(SO_VER_MAJOR)
	-test "$(ON_WINDOWS)" != "1" && $(LN) $(PROGRAM_NAME)$(SO_VER_MAJOR) $(DESTDIR)$(LIB_DIR)/$(PROGRAM_NAME)$(SO_SUFFIX)
	$(INSTALL_DATA) $(PROGRAM_NAME)$(LIB_SUFFIX)  $(DESTDIR)$(LIB_DIR)/$(PROGRAM_NAME)$(LIB_SUFFIX)
	$(INSTALL_DATA) $(PROGRAM_NAME).h  $(DESTDIR)$(INC_DIR)/$(PROGRAM_NAME).h
	$(INSTALL_DATA) $(PROGRAM_NAME).7  $(DESTDIR)$(MAN7_DIR)/$(PROGRAM_NAME).7

install: installdirs installfiles

uninstalldirs:
	-test -e $(DESTDIR)$(LIB_DIR)  && $(RMDIR) $(DESTDIR)$(LIB_DIR)
	-test -e $(DESTDIR)$(INC_DIR)  && $(RMDIR) $(DESTDIR)$(INC_DIR)
	-test -e $(DESTDIR)$(MAN7_DIR) && $(RMDIR) $(DESTDIR)$(MAN7_DIR)

uninstallfiles:
	$(RM)  $(DESTDIR)$(LIB_DIR)/$(PROGRAM_NAME)$(SO_VER_FULL)
	$(RM)  $(DESTDIR)$(LIB_DIR)/$(PROGRAM_NAME)$(SO_VER_MAJOR)
	$(RM)  $(DESTDIR)$(LIB_DIR)/$(PROGRAM_NAME)$(SO_SUFFIX)
	$(RM)  $(DESTDIR)$(LIB_DIR)/$(PROGRAM_NAME)$(LIB_SUFFIX)
	$(RM)  $(DESTDIR)$(INC_DIR)/$(PROGRAM_NAME).h
	$(RM)  $(DESTDIR)$(MAN7_DIR)/$(PROGRAM_NAME).7

uninstall: uninstallfiles uninstalldirs

test:
	./test.sh

stripped: sharedlib staticlib
	$(STRIP_UNNEEDED) libjodycode$(SO_SUFFIX)
	$(STRIP_DEBUG) libjodycode$(LIB_SUFFIX)

objsclean:
	$(RM) $(OBJS) $(SIMD_OBJS) vercheck.o *.obj

clean: objsclean
	$(RM) $(PROGRAM_NAME)$(SO_SUFFIX) $(PROGRAM_NAME)$(SO_VER_MAJOR) $(PROGRAM_NAME)$(SO_VER_FULL)
	$(RM) $(PROGRAM_NAME)$(LIB_SUFFIX) apiver cacheinfo vercheck
	$(RM) *~ helper_code/*~ libjodycode.so.* libjodycode.dll.* .*.un~ *.gcno *.gcda *.gcov

distclean: objsclean clean
	$(RM) *.pkg.tar.*
	$(RM) -r $(PROGRAM_NAME)-*-*/ $(PROGRAM_NAME)-*-*.zip

chrootpackage:
	+./chroot_build.sh
package:
	+./generate_packages.sh
