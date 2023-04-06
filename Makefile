# libjodycode Makefile

CFLAGS ?= -O2 -g
PREFIX = /usr/local
PROGRAM_NAME = libjodycode
LIB_DIR = $(PREFIX)/lib
MAN_BASE_DIR = $(PREFIX)/share/man
MAN7_DIR = $(MAN_BASE_DIR)/man7
CC ?= gcc
INSTALL = install
RM      = rm -f
LN      = ln -sf
RMDIR	= rmdir -p
MKDIR   = mkdir -p
INSTALL_PROGRAM = $(INSTALL) -m 0755
INSTALL_DATA    = $(INSTALL) -m 0644

# Make Configuration
COMPILER_OPTIONS = -Wall -Wwrite-strings -Wcast-align -Wstrict-aliasing -Wstrict-prototypes -Wpointer-arith -Wundef
COMPILER_OPTIONS += -Wshadow -Wfloat-equal -Waggregate-return -Wcast-qual -Wswitch-default -Wswitch-enum -Wconversion -Wunreachable-code -Wformat=2
COMPILER_OPTIONS += -std=gnu99 -D_FILE_OFFSET_BITS=64 -fstrict-aliasing -pipe -fPIC
COMPILER_OPTIONS += -DSMA_MAX_FREE=11 -DNO_ATIME

UNAME_S=$(shell uname -s)
VERSION=$(shell grep '#define VER ' version.h | sed 's/[^"]*"//;s/".*//')
VERSION_MAJOR=$(shell grep '#define VER ' version.h | sed 's/[^"]*"//;s/\..*//')

# Don't use unsupported compiler options on gcc 3/4 (Mac OS X 10.5.8 Xcode)
ifeq ($(UNAME_S), Darwin)
	GCCVERSION = $(shell expr `LC_ALL=C gcc -v 2>&1 | grep 'gcc version ' | cut -d\  -f3 | cut -d. -f1` \>= 5)
else
	GCCVERSION = 1
endif
ifeq ($(GCCVERSION), 1)
	COMPILER_OPTIONS += -Wextra -Wstrict-overflow=5 -Winit-self
endif

# Are we running on a Windows OS?
ifeq ($(OS), Windows_NT)
	ifndef NO_WINDOWS
		ON_WINDOWS=1
	endif
endif

# Debugging code inclusion
ifdef LOUD
DEBUG=1
COMPILER_OPTIONS += -DLOUD_DEBUG
endif
ifdef DEBUG
COMPILER_OPTIONS += -DDEBUG
else
COMPILER_OPTIONS += -DNDEBUG
endif
ifdef HARDEN
COMPILER_OPTIONS += -Wformat -Wformat-security -D_FORTIFY_SOURCE=2 -fstack-protector-strong -Wl,-z,relro -Wl,-z,now
endif

# MinGW needs this for printf() conversions to work
ifdef ON_WINDOWS
	ifndef NO_UNICODE
		UNICODE=1
		COMPILER_OPTIONS += -municode
		PROGRAM_SUFFIX=.exe
	endif
	COMPILER_OPTIONS += -D__USE_MINGW_ANSI_STDIO=1 -DON_WINDOWS=1
endif

CFLAGS += $(COMPILER_OPTIONS) $(CFLAGS_EXTRA)

# ADDITIONAL_OBJECTS - some platforms will need additional object files
# to support features not supplied by their vendor. Eg: GNU getopt()
#ADDITIONAL_OBJECTS += getopt.o

OBJS += jody_cacheinfo.o jody_hash.o jody_oom.o jody_paths.o jody_sort.o
OBJS += jody_string_malloc.o jody_strtoepoch.o jody_win_stat.o jody_win_unicode.o
OBJS += $(ADDITIONAL_OBJECTS)

all: sharedlib staticlib

sharedlib: $(OBJS)
	$(CC) -shared -o $(PROGRAM_NAME).so.$(VERSION) $(OBJS)
	$(LN) $(PROGRAM_NAME).so.$(VERSION) $(PROGRAM_NAME).so.$(VERSION_MAJOR)

staticlib: $(OBJS)
	$(AR) rcs libjodycode.a $(OBJS)

#.c.o:
#	$(CC) -c $(BUILD_CFLAGS) $(CFLAGS) $<

#manual:
#	gzip -9 < jodycode.8 > jodycode.8.gz

$(PROGRAM_NAME): $(OBJS)
#	$(CC) $(CFLAGS) $(LDFLAGS) -o $(PROGRAM_NAME) $(OBJS)

installdirs:
	test -e $(DESTDIR)$(LIB_DIR) || $(MKDIR) $(DESTDIR)$(LIB_DIR)
	test -e $(DESTDIR)$(MAN7_DIR) || $(MKDIR) $(DESTDIR)$(MAN7_DIR)

install: sharedlib staticlib installdirs
	$(INSTALL_DATA)	$(PROGRAM_NAME).so.$(VERSION_MAJOR) $(DESTDIR)$(LIB_DIR)/$(PROGRAM_NAME).so.$(VERSION_MAJOR)
	$(LN)		$(PROGRAM_NAME).so.$(VERSION_MAJOR) $(DESTDIR)$(LIB_DIR)/$(PROGRAM_NAME).so.$(VERSION)
	$(INSTALL_DATA)	$(PROGRAM_NAME).7 $(DESTDIR)$(MAN7_DIR)/$(PROGRAM_NAME).7

uninstalldirs:
	-test -e $(DESTDIR)$(LIB_DIR) && $(RMDIR) $(DESTDIR)$(LIB_DIR)
	-test -e $(DESTDIR)$(MAN7_DIR) && $(RMDIR) $(DESTDIR)$(MAN7_DIR)

uninstall: uninstalldirs
	$(RM)	$(DESTDIR)$(LIB_DIR)/$(PROGRAM_NAME).so.$(VERSION_MAJOR)
	$(RM)	$(DESTDIR)$(LIB_DIR)/$(PROGRAM_NAME).so.$(VERSION)
	$(RM)	$(DESTDIR)$(MAN7_DIR)/$(PROGRAM_NAME).7

test:
	./test.sh

stripped: sharedlib staticlib
	strip --strip-unneeded libjodycode.so.$(VERSION)
	strip --strip-debug libjodycode.a

clean:
	$(RM) $(OBJS) $(OBJS_CLEAN) $(PROGRAM_NAME).so* *~ .*.un~ *.gcno *.gcda *.gcov

distclean: clean
	$(RM) *.pkg.tar.*
	$(RM) -r $(PROGRAM_NAME)-*-*/ $(PROGRAM_NAME)-*-*.zip

chrootpackage:
	+./chroot_build.sh
package:
	+./generate_packages.sh
