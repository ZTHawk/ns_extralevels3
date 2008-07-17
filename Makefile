# vi: set ts=4 sw=4 :
# vim: set tw=75 :

# MetaMOD makefile
# by Will Day, based on the Valve SDK 2.1 Makefile as well as the Makefile
# in adminmod by Alfred Reynolds

# From SDK 2.1 dlls/Makefile:
#! Half-Life StandardSDK 2.0 mp_i386.so Makefile for i386 Linux
#! April 2000 by Leon Hartwig (jehannum@planethalflife.com)


# NOTE: This is a generic Makefile for metamod and the bundled plugins, and
# is symlinked into each subdir.  Per-module config statements are in
# Config.mak in each subdir.

#SRCFILES =	events.cpp hookedfunctions.cpp meta_api.cpp player_data.cpp plugin.cpp \
#		upgrade_cybernetics.cpp upgrade_thickenedskin.cpp \
#		upgrade_reinforcedarmor.cpp upgrade_etherealshift.cpp \
#		upgrade_nanoarmor.cpp upgrade_bloodlust.cpp \
#		upgrade_advancedammopack.cpp upgrade_hunger.cpp \
#		upgrade_staticfield.cpp upgrade_acidicvengeance.cpp \
#		upgrade_uraniumammo.cpp \
#		utilfunctions.cpp

OS=linux
#OPT=opt

#############################################################################
# CONFIGURATION
#############################################################################

# set paths for your environment
ifeq "$(OS)" "linux"
	PATH_WIN=../MinGW/bin
	SDKTOP=../hlsdk
	INST_DIR=$(HOME)/half-life/cstrike/dlls
	TEST_DIR=$(HOME)/test/cstrike/dlls
	TST_DIR=$(HOME)/tmp
else	## windows
	PATH_WIN=../MinGW/bin
	SDKTOP=../hlsdk
	INST_DIR=/hlserver/tfc/dlls
	TEST_DIR=/hlserver/tfc/dlls1
endif

DLLS_DIR=../dlls
SDKSRC=$(SDKTOP)/multiplayer
METADIR=../metamod-p/metamod


#############################################################################
# OS DEPENDENCIES
#############################################################################

ifeq "$(OS)" "linux"
	INSTALL=install -m 644
	LD_WINDLL= PATH=$(PATH_WIN) $(PATH_WIN)/dllwrap
	TARGET=linux
	LIBFILE=$(LIBFILE_LINUX)
	TARGET_FILE=$(TARGET_LINUX)
else	## windows
	INSTALL=cp
	LD_WINDLL= $(PATH_WIN)/dllwrap
	TARGET=win32
	LIBFILE=$(LIBFILE_WIN)
	TARGET_FILE=$(TARGET_WIN)
endif

CC_WIN=$(PATH_WIN)/gcc
RES_WIN=$(PATH_WIN)/windres
OBJDIR_LINUX_OPT=pro.linux
OBJDIR_WIN_OPT=pro.win32
OBJDIR_LINUX_DBG=debug.linux
OBJDIR_WIN_DBG=debug.win32


#############################################################################
# COMPILE OPTIONS
#############################################################################

# original safe optimization, from valve Makefile
#CCOPT = -O2 -ffast-math -funroll-loops \
#	 -fomit-frame-pointer -fexpensive-optimizations -falign-loops=2 \
#	 -falign-jumps=2 -falign-functions=2

# safe optimization, adapted from adminmod Makefile
#CCOPT = -m486 -O6 -ffast-math -funroll-loops \
# 	 -fexpensive-optimizations -malign-loops=2  -malign-jumps=2 \
# 	 -malign-functions=2 -Wall

# full optimization, adapted from adminmod Makefile
# "WONT WORK WITH omit-frame-pointer"?
CCOPT = -march=i586 $(CCO) -ffast-math -funroll-loops \
	-fomit-frame-pointer -fexpensive-optimizations -falign-loops=2 \
	-falign-jumps=2 -falign-functions=2 -s

# optimization level; overridden for certain problematic files
CCO = -O6

# debugging; halt on warnings
CCDEBUG+= -ggdb3


#############################################################################
# COMPILE SETUP
#############################################################################

CC=gcc-linux

SRCDIR=.
INCLUDEDIRS=-I$(SRCDIR) -I ./source -I$(SRCDIR)/source -I$(METADIR) -I$(SDKSRC)/engine -I$(SDKSRC)/common \
	    -I$(SDKSRC)/pm_shared -I$(SDKSRC)/dlls -I$(SDKSRC)
FILES_ALL = *.cpp *.h [A-Z]* *.rc

CFLAGS=-Wall -Wno-unknown-pragmas -Wno-unused

ifeq "$(OPT)" "opt"
	ODEF = -DOPT_TYPE=\"optimized\"
	CFLAGS := $(CCOPT) $(CFLAGS) $(ODEF)
	OBJDIR_LINUX = $(OBJDIR_LINUX_OPT)
	OBJDIR_WIN = $(OBJDIR_WIN_OPT)
else	# debug
	ODEF = -DOPT_TYPE=\"debugging\"
	CFLAGS := $(CCDEBUG) $(CFLAGS) $(ODEF)
	OBJDIR_LINUX = $(OBJDIR_LINUX_DBG)
	OBJDIR_WIN = $(OBJDIR_WIN_DBG)
	DLLS_DIR := $(DLLS_DIR)/debug
endif

include Config.mak
# any local CFLAGS from Config.mak
CFLAGS += $(EXTRA_CFLAGS) 

$(OBJDIR_LINUX)/sdk_util.o: CFLAGS += -Wno-unused
$(OBJDIR_WIN)/sdk_util.o: CFLAGS += -Wno-unused

# these files seem to create "Internal compiler error" errors under mingw
# when using -O6
#$(OBJDIR_WIN)/engine_api.o: CCO = -O5
#$(OBJDIR_WIN)/dllapi_api.o: CCO = -O5

OBJDIR_LINUX_SOURCE = $(OBJDIR_LINUX)/source

#############################################################################
# BUILDING LINUX SO
#############################################################################

# linux .so compile commands
DO_CC_LINUX=$(CC) $(CFLAGS) -fPIC $(INCLUDEDIRS) -o $@ -c $<
# XXX: added the following linker option to make HL load the shared library 
# under linux to fix the error:
# [...] symbol not found: __gxx_personality_v0 [...]
#
# This is just a workaround.
#LINK_LINUX=$(CC) $(CFLAGS) -shared -ldl -lm $(EXTRA_LINK) \
#	   -Wl,--defsym -Wl,__gxx_personality_v0=0 $(OBJ_LINUX) /usr/lib/libstdc++-3-libc6.2-2-2.10.0.a -o $@

#LINK_LINUX=$(CC) $(CFLAGS) -shared -ldl -lm $(EXTRA_LINK) \
#	   $(OBJ_LINUX) $(PATH_WIN)/../lib/libstdc++.a -o $@
LINK_LINUX=$(CC) $(CFLAGS) -shared -ldl -lm $(EXTRA_LINK) \
	   $(OBJ_LINUX) -lstdc++ -o $@


# sort by date
ifeq "$(OS)" "linux"
	SRCFILES := $(shell ls -t $(SRCFILES))
endif
# linux object files
OBJ_LINUX := $(SRCFILES:%.cpp=$(OBJDIR_LINUX)/%.o)

# compiling linux object files
$(OBJDIR_LINUX)/%.o: $(SRCDIR)/%.cpp
	$(DO_CC_LINUX)

# linux .so target file
LIBFILE_LINUX = $(MODNAME)_i386.so
TARGET_LINUX = $(OBJDIR_LINUX)/$(LIBFILE_LINUX)


#############################################################################
# BUILDING WINDOWS DLL
#############################################################################

# windows .dll compile commands
DO_CC_WIN=$(CC_WIN) $(CFLAGS) $(INCLUDEDIRS) -o $@ -c $<
DO_RES_WIN=$(RES_WIN) '$(ODEF)' -i $< -O coff -o $@
#LINK_WIN=$(LD_WINDLL) -k -mwindows --add-stdcall-alias --def metamod.def -o $@ $(OBJ_WIN)
#LINK_WIN=$(LD_WINDLL) -A -k -mwindows --export-all-symbols -o $@ $(OBJ_WIN)
LINK_WIN=$(LD_WINDLL) -mwindows --add-stdcall-alias $(OBJ_WIN) $(RES_OBJ_WIN) -o $@

# windows object files
OBJ_WIN := $(SRCFILES:%.cpp=$(OBJDIR_WIN)/%.o)
RES_OBJ_WIN := $(RESFILE:%.rc=$(OBJDIR_WIN)/%.o)

# compiling windows object files
$(OBJDIR_WIN)/%.o: $(SRCDIR)/%.cpp
	$(DO_CC_WIN)

# compiling windows resource file
$(OBJDIR_WIN)/%.o: $(SRCDIR)/%.rc $(VERSFILE)
	$(DO_RES_WIN)

# windows .dll target file
LIBFILE_WIN = $(MODNAME).dll
TARGET_WIN = $(OBJDIR_WIN)/$(LIBFILE_WIN)


#############################################################################
# BUILD RULES
#############################################################################

default: $(TARGET_FILE)

all: $(TARGET_LINUX) $(TARGET_WIN)

opt:
	$(MAKE) default OPT=opt

# make sure to recompile vdate.c for each link
$(OBJDIR)/vdate.o: $(SRCFILES)

linux: $(TARGET_LINUX)
win32: $(TARGET_WIN)

$(TARGET_LINUX): msgs/debug msgs/log msgs/error $(OBJDIR_LINUX) $(OBJDIR_LINUX_SOURCE) $(OBJ_LINUX)
	$(LINK_LINUX)

$(TARGET_WIN): $(OBJDIR_WIN) $(OBJ_WIN) $(RES_OBJ_WIN)
	$(LINK_WIN)

$(OBJDIR_LINUX_SOURCE) $(OBJDIR_LINUX) $(OBJDIR_WIN) msgs:
	mkdir $@

domsgs: msgs/debug msgs/log msgs/error

msgs/debug: $(SRCFILES) msgs
	egrep "DEBUG\([0-9]" $(SRCFILES) *.h | sed "s/:[ 	]*/	/" | sort -k2,2 > $@

msgs/log: $(SRCFILES) msgs
	egrep "META_LOG\(" $(SRCFILES) *.h | sed "s/:[ 	]*/	/" | sort > $@

msgs/error: $(SRCFILES) msgs
	egrep "META_ERROR\(" $(SRCFILES) *.h | sed "s/:[ 	]*/	/" | sort > $@

tags:	.tags .htags
ctags:	.tags
htags:	.htags

.tags: $(SRCFILES) *.h
	-ctags -a $?

.htags: *.h
	-htags $?

retags:
	-rm -f .tags .htags
	ctags -f .tags `find $(SDKSRC) -name '*.h'`
	ctags -f .tags -a `find $(METADIR) -name old -prune -o -name '*.h' -print`
	ctags -f .tags -a $(SRCFILES)
	htags -R $(SDKSRC)
	htags `find $(METADIR) -name old -prune -o -name '*.h' -print`
	htags *.h

clean: clean_$(TARGET)

clean_linux:
	test -n "$(OBJDIR_LINUX)"
	-rm -rf $(OBJDIR_LINUX)/*
	

clean_win32:
	test -n "$(OBJDIR_WIN)"
	-rm -f $(OBJDIR_WIN)/*

cleanall_linux:
	$(MAKE) clean_linux
	$(MAKE) clean_linux OPT=opt

cleanall_win32:
	$(MAKE) clean_win32
	$(MAKE) clean_win32 OPT=opt

cleanall: cleanall_linux cleanall_win32

dll_linux dll_win32: 
	$(MAKE) do_$@
	$(MAKE) do_$@ OPT=opt

do_dll_linux: $(DLLS_DIR)/$(LIBFILE_LINUX)
do_dll_win32: $(DLLS_DIR)/$(LIBFILE_WIN)

$(DLLS_DIR)/$(LIBFILE_LINUX): $(TARGET_LINUX)
	$(INSTALL) $+ $@

$(DLLS_DIR)/$(LIBFILE_WIN): $(TARGET_WIN)
	$(INSTALL) $+ $@

dlls: dll_linux dll_win32
rmdlls:
	-rm -f ../dlls/*.* ../dlls/debug/*.*

spotless: cleanall
	-rmdir $(OBJDIR_LINUX) $(OBJDIR_WIN)
	-rm -f .snap

distclean: spotless

install: $(INST_DIR)/$(LIBFILE)
test: $(TEST_DIR)/$(LIBFILE)

$(INST_DIR)/$(LIBFILE) $(TEST_DIR)/$(LIBFILE): $(TARGET_FILE)
	$(INSTALL) $< $@

tst: $(TST_DIR)/$(LIBFILE_WIN)

$(TST_DIR)/$(LIBFILE_WIN): $(TARGET_WIN)
	cp $< $@

snap:	.snap

.snap: $(FILES_ALL)
	mkdir -p snapshots
	tar zcvf snapshots/`date '+%m%d-%H%M'`.tgz $(FILES_ALL)
	touch .snap

depend: Rules.depend

ifeq "$(OS)" "linux"
Rules.depend: $(SRCFILES)
	$(CC) -MM $(INCLUDEDIRS) $(SRCFILES) | \
	sed "s;\(^[^ 	]*\):\(.*\);$(OBJDIR_LINUX)/\1 $(OBJDIR_WIN)/\1: \2;" > $@
endif

include Rules.depend
