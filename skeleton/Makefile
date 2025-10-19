# a_ljc_program Makefile

# PROGRAM_NAME determines the installation name and manual page name
PROGRAM_NAME = a_ljc_program

# Default flags to pass to the C compiler (can be overridden)
CFLAGS ?= -O2 -g
#CFLAGS ?= -Og -g3

# PREFIX determines where files will be installed. Common examples
# include "/usr" or "/usr/local".
PREFIX = /usr/local

# BIN_DIR indicates directory where program is to be installed.
# Suggested value is "$(PREFIX)/bin"
BIN_DIR = $(PREFIX)/bin

# MAN_DIR indicates directory where the man page is to be
# installed. Suggested value is "$(PREFIX)/man/man1"
MAN_BASE_DIR = $(PREFIX)/share/man
MAN_DIR = $(MAN_BASE_DIR)/man1
MAN_EXT = 1

# Required external tools
CC ?= gcc
INSTALL = install
RM      = rm -f
RMDIR   = rmdir -p
MKDIR   = mkdir -p
INSTALL_PROGRAM = $(INSTALL) -m 0755
INSTALL_DATA    = $(INSTALL) -m 0644

# Main object files
OBJS += $(PROGRAM_NAME).o libjodycode_check.o

# Configuration section
COMPILER_OPTIONS = -Wall -Wwrite-strings -Wcast-align -Wstrict-aliasing -Wstrict-prototypes -Wpointer-arith -Wundef
COMPILER_OPTIONS += -Wshadow -Wfloat-equal -Waggregate-return -Wcast-qual -Wswitch-default -Wswitch-enum -Wunreachable-code -Wformat=2
COMPILER_OPTIONS += -std=gnu11 -D_FILE_OFFSET_BITS=64 -fstrict-aliasing -pipe
COMPILER_OPTIONS += -DNO_ATIME

ifdef USE_LTO
 COMPILER_OPTIONS += -flto
 LINK_OPTIONS += -flto
endif

# Remove unused code if requested
ifdef GC_SECTIONS
 COMPILER_OPTIONS += -fdata-sections -ffunction-sections
 LINK_OPTIONS += -Wl,--gc-sections
endif

UNAME_S=$(shell uname -s)

# Are we running on a Windows OS?
ifeq ($(OS), Windows_NT)
 ifndef NO_WINDOWS
  ON_WINDOWS=1
 endif
endif

# Debugging code inclusion
ifdef DEBUG
 COMPILER_OPTIONS += -DDEBUG
else
 COMPILER_OPTIONS += -DNDEBUG
endif
ifdef HARDEN
 COMPILER_OPTIONS += -Wformat -Wformat-security -D_FORTIFY_SOURCE=2 -fstack-protector-strong -fPIE -fpie -Wl,-z,relro -Wl,-z,now
else
 #COMPILER_OPTIONS += -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0
endif

# MinGW needs this for printf() conversions to work
ifdef ON_WINDOWS
 ifndef NO_UNICODE
  UNICODE=1
  COMPILER_OPTIONS += -municode
 endif
 SUFFIX=.exe
 SO_EXT=.dll
 LIB_EXT=.lib
 COMPILER_OPTIONS += -D__USE_MINGW_ANSI_STDIO=1 -DON_WINDOWS=1
 ifeq ($(UNAME_S), MINGW32_NT-5.1)
  OBJS += winres_xp.o
 else
  OBJS += winres.o
 endif
else
 SO_EXT=.so
 LIB_EXT=.a
endif

# Don't use unsupported compiler options on gcc 3/4 (Mac OS X 10.5.8 Xcode)
# ENABLE_DEDUPE by default - macOS Sierra 10.12 and up required
ifeq ($(UNAME_S), Darwin)
 GCCVERSION = $(shell expr `LC_ALL=C gcc -v 2>&1 | grep '[cn][cg] version' | sed 's/[^0-9]*//;s/[ .].*//'` \>= 5)
else
 GCCVERSION = 1
 BDYNAMIC = -Wl,-Bdynamic
 BSTATIC = -Wl,-Bstatic
endif

ifeq ($(GCCVERSION), 1)
 COMPILER_OPTIONS += -Wextra -Wstrict-overflow=5 -Winit-self
endif

# Stack size limit can be too small for deep directory trees, so set to 16 MiB
# The ld syntax for Windows is the same for both Cygwin and MinGW
ifndef LOW_MEMORY
 ifeq ($(OS), Windows_NT)
  COMPILER_OPTIONS += -Wl,--stack=16777216
 else ifeq ($(UNAME_S), Darwin)
  COMPILER_OPTIONS += -Wl,-stack_size -Wl,0x1000000
 else
  COMPILER_OPTIONS += -Wl,-z,stack-size=16777216
 endif
endif

### Find and use nearby libjodycode by default
ifndef IGNORE_NEARBY_JC
 ifneq ("$(wildcard ../libjodycode/libjodycode.h)","")
  $(info Found and using nearby libjodycode at ../libjodycode)
  COMPILER_OPTIONS += -I../libjodycode -L../libjodycode
  ifeq ("$(wildcard ../libjodycode/version.o)","")
   $(error You must build libjodycode before building $(PROGRAM_NAME))
  endif
  ifneq ("$(wildcard ../libjodycode/libjodycode.a)","")
   $(info Overriding static library extension .lib for found extension .a)
   LIB_EXT=.a
  endif
  STATIC_LDFLAGS += ../libjodycode/libjodycode$(LIB_EXT)
  DYN_LDFLAGS += -l:../libjodycode/libjodycode$(SO_EXT)
 else
  STATIC_LDFLAGS += -ljodycode
  DYN_LDFLAGS += -ljodycode
 endif
else
 STATIC_LDFLAGS += -ljodycode
 DYN_LDFLAGS += -ljodycode
endif

# Don't build ljc_vercheck dynamically on Windows
ifdef ON_WINDOWS
 LJC_DYNAMIC = $(BSTATIC) $(STATIC_LDFLAGS)
 $(info )
 $(info Notice: Windows dynamic builds will build but silently fail)
 $(info         without libjodycode.dll in the same directory.)
 $(info         To avoid this, link statically: 'make static_jc')
 $(info )
else
 LJC_DYNAMIC = $(BDYNAMIC) $(DYN_LDFLAGS)
endif


CFLAGS += $(COMPILER_OPTIONS) $(CFLAGS_EXTRA)
LDFLAGS += $(LINK_OPTIONS) $(LDFLAGS_EXTRA)


all: libjodycode_hint $(PROGRAM_NAME) dynamic_jc ljc_vercheck_dynamic

dynamic_jc: $(PROGRAM_NAME) ljc_vercheck_dynamic
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) $(BDYNAMIC) $(DYN_LDFLAGS) -o $(PROGRAM_NAME)$(SUFFIX)

static_jc: $(PROGRAM_NAME) ljc_vercheck_static
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) $(BSTATIC) $(STATIC_LDFLAGS) $(BDYNAMIC) -o $(PROGRAM_NAME)$(SUFFIX)

static: $(PROGRAM_NAME) ljc_vercheck_static
	$(CC) $(CFLAGS) $(OBJS) -static $(LDFLAGS) $(BSTATIC) $(STATIC_LDFLAGS) -o $(PROGRAM_NAME)$(SUFFIX)

static_stripped: $(PROGRAM_NAME) static_jc
	-strip $(PROGRAM_NAME)$(SUFFIX)

$(PROGRAM_NAME): $(OBJS)
	@:

winres.o: winres.rc winres.manifest.xml
	./tune_winres.sh
	windres winres.rc winres.o

winres_xp.o: winres_xp.rc
	./tune_winres.sh
	windres winres_xp.rc winres_xp.o

installdirs:
	test -e $(DESTDIR)$(BIN_DIR) || $(MKDIR) $(DESTDIR)$(BIN_DIR)
	test -e $(DESTDIR)$(MAN_DIR) || $(MKDIR) $(DESTDIR)$(MAN_DIR)

install: $(PROGRAM_NAME) installdirs
	$(INSTALL_PROGRAM)	$(PROGRAM_NAME)$(SUFFIX)   $(DESTDIR)$(BIN_DIR)/$(PROGRAM_NAME)$(SUFFIX)
	$(INSTALL_DATA)		$(PROGRAM_NAME).1 $(DESTDIR)$(MAN_DIR)/$(PROGRAM_NAME).$(MAN_EXT)

uninstalldirs:
	-test -e $(DESTDIR)$(BIN_DIR) && $(RMDIR) $(DESTDIR)$(BIN_DIR)
	-test -e $(DESTDIR)$(MAN_DIR) && $(RMDIR) $(DESTDIR)$(MAN_DIR)

uninstall: uninstalldirs
	$(RM)	$(DESTDIR)$(BIN_DIR)/$(PROGRAM_NAME)$(SUFFIX)
	$(RM)	$(DESTDIR)$(MAN_DIR)/$(PROGRAM_NAME).$(MAN_EXT)

test:
	./test.sh

stripped: $(PROGRAM_NAME) dynamic_jc
	strip $(PROGRAM_NAME)$(SUFFIX)

clean:
	$(RM) $(OBJS) $(OBJS_CLEAN) $(PROGRAM_NAME)$(SUFFIX) ljc_vercheck$(SUFFIX) \
		*~ .*.un~ *.gcno *.gcda *.gcov *.obj

distclean: clean
	$(RM) -rf *.pkg.tar* $(PROGRAM_NAME)-*-*/ $(PROGRAM_NAME)-*-*.zip

.PHONY: ljc_vercheck_static ljc_vercheck_dynamic

ljc_vercheck_dynamic:
	$(CC) $(CFLAGS) $(CPPFLAGS) libjodycode_check.c -DSTANDALONE $(LDFLAGS) $(LJC_DYNAMIC) -o ljc_vercheck$(SUFFIX)
	@echo
	@./ljc_vercheck 2>/dev/null || echo "Version check failed. Install libjodycode on the system and try again."
	@if [ `(./ljc_vercheck$(SUFFIX) 2>/dev/null || echo ":0:0:0:0") | cut -d: -f4` -lt `grep MY_FEATURELEVEL_REQ libjodycode_check.h | cut -d" " -f3` ]; \
		then echo "The linked libjodycode feature level is too old. Get the latest libjodycode and try again."; \
		else echo "The linked libjodycode feature level is OK."; \
	fi
	@echo

ljc_vercheck_static:
	$(CC) $(CFLAGS) $(CPPFLAGS) libjodycode_check.c -DSTANDALONE $(LDFLAGS) $(BSTATIC) $(STATIC_LDFLAGS) $(BDYNAMIC) -o ljc_vercheck$(SUFFIX)
	@echo
	@./ljc_vercheck 2>/dev/null
	@if [ `(./ljc_vercheck$(SUFFIX) 2>/dev/null || echo ":0:0:0:0") | cut -d: -f4` -lt `grep MY_FEATURELEVEL_REQ libjodycode_check.h | cut -d" " -f3` ]; \
		then echo "The linked libjodycode feature level is too old. Get the latest libjodycode and try again."; \
		else echo "The linked libjodycode feature level is OK."; \
	fi
	@echo

libjodycode_hint:
	$(info hint: if ../libjodycode is built but the program won't run, try doing 'make static_jc')
