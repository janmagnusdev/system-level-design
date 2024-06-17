# -----------------------------------------------------------------------
#
# Generic Makefile for small to medium SystemC applications
#
# Copyright (c) 2006-2012 OFFIS e.V. Institute for Information Technology
# All rights reserved.
#
# Author: Philipp A. Hartmann <philipp.hartmann@offis.de>
#
# -----------------------------------------------------------------------
#
# Redistribution and use in source form, with or without modification,
# are permitted provided that the following conditions are met:
#
# * Redistributions must retain the above copyright notice, this list
#   of conditions and the following disclaimer.
# * Neither the name of OFFIS nor the names of its contributors may
#   be used to endorse or promote products derived from this software
#   without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# -----------------------------------------------------------------------
#
# This file defines generic targets for (small) SystemC applications.
# The main targets, that are available by default, are:
#
# all (default)
#    - build the application
# [MODULE]
#    - build and run the application
#      (name determined by MODULE variable)
# clean
#    - cleanup generated files
#
# -----------------------------------------------------------------------
#
# Each of the following variables can be overwritten inside
# the specialized Makefile. In most cases, the default values
# should be fine.
#

#
# The variable MODULE has to be set, since this determines
# the name of the executable
#
ifeq (,$(strip $(MODULE)))
$(error MODULE not set! Bailing out)
else
# Name of the executable
EXE := $(MODULE).x
endif

DEL := rm -f

# Target architecture
# This is only needed to link against SystemC properly, though.
#TARGET_ARCH=


ifneq (,$(wildcard ../../sld-extern/include/systemc.h))
SYSTEMC_HOME = ../../sld-extern
SYSTEMC_LIB = ../../sld-extern/lib
endif

ifneq (,$(wildcard ../sld-extern/include/systemc.h))
SYSTEMC_HOME = ../sld-extern
SYSTEMC_LIB = ../sld-extern/lib
endif

# SystemC installation
# If the variable that points to SystemC installation path is not
# set, try a sane default
SYSTEMC_HOME?=/usr/local/lib/systemc
SYSTEMC_LIBNAME?=systemc


# set additional defines
#
# This variable can hold additional command-line defines, that
# should be passed to the compiler e.g "-DSC_INCLUDE_FX=1".
EXTRA_DEFINES ?=

# set include path
#
# This setting should not be modified/set directly
# from within a specific Makefile. Add directories
# to the variable $(EXTRA_INCLUDES), if you need
# additional directories
#
INCLUDES = -isystem $(SYSTEMC_HOME)/include $(EXTRA_INCLUDES)

# set library path
#
# This setting should not be modified/set directly
# from within a specific Makefile. Add directories
# to the variable $(EXTRA_LIBDIRS), if you need
# additional directories
#
LIBDIRS  = -L$(SYSTEMC_LIB) $(EXTRA_LIBDIRS)

# set static libraries to link against
#
# This setting should not be modified/set directly
# from within a specific Makefile. Add directories
# to the variable $(EXTRA_LIBS), if you need
# additional libraries
#
LIBS     = $(EXTRA_LIBS) -l$(SYSTEMC_LIBNAME) -lm

CXX ?= clang++

# compiler settings
#
# The following variables are respected during the compilation
# process. Defaults are below.
# Note:
#    CXXFLAGS_DEBUG is only added, iff $(DEBUG)  = yes
#    CXXFLAGS_OPT   is only added, iff $(DEBUG) != yes
CXXFLAGS       ?= -Wall -Wextra -pedantic #-Werror -Wno-variadic-macros -Wno-long-long -Wno-tautological-compare -Wno-type-limits
CXXFLAGS_OPT   ?= -O2
CXXFLAGS_DEBUG ?= -g -DDEBUG=1 -DTRACE=1
CXXFLAGS_EXTRA ?=

#
# Look for source files
#
# C++ filename extension, defaults to "<name>.cpp"
SRCEXT  ?= cpp
ifndef SRCS
# source files are not specified directly
# check source directories - if not set, use current directory
SRCDIRS ?= .
# look for source files in all source directories
SRCS    := $(wildcard $(SRCDIRS:%=%/*.$(SRCEXT)))
endif

ifeq (,$(strip $(SRCS)))
$(error No source files found! Bailing out)
endif

# Object files
OBJS := $(SRCS:.$(SRCEXT)=.o)
# Dependency files
DEPS := $(SRCS:.$(SRCEXT)=.d)
# Preprocessor output
PRES := $(SRCS:.$(SRCEXT)=.pre)

#
# change compiler settings according to selection
#
CXXFLAGS+=$(EXTRA_DEFINES)
CXXFLAGS+=-std=c++17
ifeq (yes,$(strip $(DEBUG)))
CXXFLAGS+=$(CXXFLAGS_DEBUG)
else
CXXFLAGS+=$(CXXFLAGS_OPT)
endif
ifeq (yes,$(strip $(PROFILE)))
CXXFLAGS+=-pg
endif
ifneq (,$(strip $(CXXFLAGS_EXTRA)))
CXXFLAGS+=$(CXXFLAGS_EXTRA)
endif

# check, if only cleanup is requested
CLEAN_TARGETS := clean objclean preclean depclean
CLEAN_ONLY    := \
  $(if $(MAKECMDGOALS),$(if \
     $(filter-out $(CLEAN_TARGETS),$(MAKECMDGOALS)),no,yes),no)

# we're building something
ifeq (no,$(CLEAN_ONLY))

# guess target architecture, if not set
ifeq (,$(strip $(TARGET_ARCH)))
SYSTEM_ARCH:=$(shell $(CXX) -dumpmachine)
TARGET_ARCH:=\
  case '$(SYSTEM_ARCH)' in \
    sparc-sun-solaris*)               echo 'gccsparcOS5'  ;; \
    x86_64*linux*)                    echo 'linux64'      ;; \
    *linux*)                          echo 'linux'        ;; \
    i?86-apple-macosx*)               echo 'macosx386'    ;; \
    powerpc-apple-macosx*)            echo 'macosx'       ;; \
    amd64*freebsd* | x86_64*freebsd*) echo 'freebsd64'    ;; \
    *freebsd*)                        echo 'freebsd'      ;; \
    *cygwin*)                         echo 'cygwin'       ;; \
    *mingw*)                          echo 'mingw'        ;; \
    *hpux11*)                         echo 'gcchpux11'    ;; \
  esac
TARGET_ARCH:=$(shell $(TARGET_ARCH))
endif
SYSTEMC_LIB?=$(SYSTEMC_HOME)/lib-$(TARGET_ARCH)

# Add $(SYSTEMC_LIB) to the linker runtime search path to avoid
# LD_LIBRARY_PATH hassle.
LDFLAGS ?= -Wl,-rpath $(SYSTEMC_LIB)

# Environment checks
ifeq (,$(strip $(SYSTEMC_HOME)))
$(error Variable SYSTEMC_HOME not set! Bailing out)
endif

ifeq (,$(wildcard $(SYSTEMC_HOME)/include/systemc.h))
$(error No SystemC headers in '$(SYSTEMC_HOME)/include' found! Bailing out)
endif

ifeq (,$(wildcard $(SYSTEMC_LIB)/))
$(error No SystemC library directory '$(SYSTEMC_LIB)' found! Bailing out)
endif

# look for SystemC library (use -dbg variant, if DEBUG and existing
ifeq (yes,$(strip $(DEBUG)))
ifneq (,$(wildcard $(SYSTEMC_LIB)/lib$(SYSTEMC_LIBNAME)-dbg.a))
SYSTEMC_LIBNAME:=$(SYSTEMC_LIBNAME)-dbg
endif
endif
ifeq (,$(wildcard $(SYSTEMC_LIB)/lib$(SYSTEMC_LIBNAME).a))
$(error No SystemC library 'lib$(SYSTEMC_LIBNAME).a' in '$(SYSTEMC_LIB)' found! Bailing out)
endif

# look for TLM library (optional)
ifneq (,$(strip $(TLM_HOME)))
TLM_INCLUDE_DIR:=$(TLM_HOME)/include/tlm
ifeq (,$(wildcard $(TLM_INCLUDE_DIR)/tlm.h))
$(warning No TLM headers found in '$(TLM_INCLUDE_DIR)'!)
else
INCLUDES+=-isystem $(TLM_INCLUDE_DIR)
endif
endif

# look for SystemC AMS extensions (optional)
ifneq (,$(strip $(SYSTEMC_AMS_HOME)))
AMS_INCLUDE_DIR:=$(SYSTEMC_AMS_HOME)/include
ifeq (,$(wildcard $(AMS_INCLUDE_DIR)/systemc-ams))
$(warning No SystemC AMS headers found in '$(AMS_INCLUDE_DIR)'!)
else
INCLUDES+=-isystem $(AMS_INCLUDE_DIR)
LIBDIRS+=-L$(SYSTEMC_AMS_HOME)/lib-$(TARGET_ARCH)
LIBS:=-lsystemc-ams $(LIBS)
endif
endif

# default target
all: $(EXE)

%.d: %.$(SRCEXT)
	$(call cmd-cxx-depend,$<,$@)

%.pre: %.$(SRCEXT) %.d
	$(call cmd-cpp-only,$<,$@)

# rule to compile a single source file
%.o: %.$(SRCEXT) %.d
	$(call cmd-cxx-compile,$<,$@)

# default target: build the executable
# depends on object files 
$(EXE): $(OBJS)
	$(call cmd-cxx-exe,$@,$^)

# Shortcut: dependencies only
dep: $(DEPS)

# Shortcut: objects only
obj: $(OBJS)

# Shortcut: preprocess only
pre: $(PRES)

# Shortcut: run simulation
# This target runs the test application after  a succesful build process.
# Parameters to this run can be given in the variable ARGS.
sim: $(EXE)
	$(call cmd-run-simulation,$<,$(ARGS))
PHONY+=sim

endif # build something

# Cleanup
CLEAN_TARGETS := clean objclean preclean depclean

objclean:
	$(call cmd-delete,$(wildcard $(OBJS)))
PHONY += objclean

preclean:
	$(call cmd-delete,$(wildcard $(PRES)))
PHONY += preclean

depclean:
	$(call cmd-delete,$(wildcard $(DEPS)))
PHONY += depclean

clean: objclean preclean depclean $(EXTRA_CLEAN)
	$(call cmd-delete,$(wildcard $(EXE)))
PHONY += clean

# actually process dependencies,
# if we are not cleaning the directory :)
ifeq (no,$(CLEAN_ONLY))
-include $(DEPS)
endif

#
# helper functions
#

# delete files
define cmd-delete
$(foreach __f,$(1),$(call cmd-delete-file,$(__f)))
endef
# Note: empty line is required!
define cmd-delete-file
	@echo '[DEL] $(notdir $1)'
	$(Q)$(DEL) $(2) $(1)
	
endef

# compile object file
define cmd-cxx-compile
	@echo "[CXX] $(notdir $(1))"
	$(Q)$(CXX) -c $(CXXFLAGS) $(INCLUDES) \
	  -o $(2) $(1)
endef

# process dependencies
define cmd-cxx-depend
	@echo "[DEP] $(notdir $(1))"
	$(Q)$(CXX) -c $(CXXFLAGS) -MM -MP $(INCLUDES) \
	  -MT '$(2) $(2:.d=.o)' -o $(2) $(1)
endef

# preprocess source file
define cmd-cpp-only
	@echo "[CPP] $(notdir $(1))"
	$(Q)$(CXX) -E $(CXXFLAGS) $(INCLUDES) $(1) | cat -s > $2
endef

# link executable
define cmd-cxx-exe
	@echo "[EXE] $(1)"
	$(Q)$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LIBDIRS) -o $(1) $(2) $(LIBS) 2>&1 | c++filt
	$(Q)test -x $(1)
endef

# run simulation
define cmd-run-simulation
	@echo "[SIM] $(1) $(2)"
	$(Q)$(CURDIR)/$(1) $(2)
endef

# prefix, to suppress certain commands, if not building verbosely
Q := $(if $(filter-out yesPlease,$(strip $(VERBOSE))Please),@)

#disable implicit rules
.SUFFIXES:

.PHONY: $(PHONY)
# Taf!
