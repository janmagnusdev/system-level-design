#
# Makefile for small to medium SystemC applications
# Copyright (c) 2006 OFFIS e.V. Institute for Information Technology
#
# Author(s): Philipp A. Hartmann (philipp.hartmann@offis.de)

#
# This Makefile uses, and thus requires the generic SystemC
# Makefile.systemc, which contains the actual build rules.
#
# Within this file, only configuration variables should be changed.
#

# The make targets, that are available by default include:
#
# all
#    - build the application
# [MODULE]
#    - build and run the application
#      (name determined by MODULE variable below)
# clean
#    - cleanup generated files
#

#
# Configuration section of this SystemC project
# (if a variable is omitted here, a default value is used)
#

# path (and name) of required "systemc.mk"
#SYSTEMC_MAKE := ../systemc.mk


# SystemC installation (required)
#
# The variable SYSTEMC_HOME is required to determine the
# location of the SystemC installation. If set in the
# environment, this setting is used. 
# SYSTEMC_HOME ?= /opt/site/legacy/hs-ehs/sw/eda/systemc/systemc-2.3

# The variable SYSTEMC_LIB is required to locate the static
# library file libsystemc.a, matching the current commpiler
# and SystemC headers.
SYSTEMC_LIB  ?= $(SYSTEMC_HOME)/lib-$(TARGET_ARCH)


# Name of the test application (required!)
MODULE=uart

# Default parameters to pass to test application
# (used, if "make run" is called)
#ARGS:=42

# source file extension
#
# Set the extension of C++ source files.
# Defaults to "<name>.cpp"
#SRCEXT := cc

# The actual source code files can be specified via a list of
# source directories (in the variable SRCDIRS) OR as a list of
# .cpp-files (in the variable SRCS). If set, the variable SRCS takes
# precedence, otherwise, all .cpp-files in SRCDIRS are taken as
# input files (see also: $(SRCEXT) above).
#
# Source-Directories
#SRCDIRS := .
# Source files
#SRCS := example1.cpp example2.cpp

#
# Other variables, that can be set here
#
# additional include options
#EXTRA_INCLUDES := -I/path/to/my/inc/dir
#
# additional library directories and link directives
#EXTRA_LIBDIRS := -L/path/to/my/lib/dir
#EXTRA_LIBS    := -lmylib

# additional preprocessor symbols to define
# (as list of -Dmacro[=defn])
#EXTRA_DEFINES := -DHURZ -Dever=;;

#
# Various settings
#

# Verbose compilation output? (yes,no)
#VERBOSE=no

# Include debug symbols? (yes,no)
DEBUG=yes

# Include profiling information (yes,no)
#PROFILE=no

# Compiler settings
#
#
# The following variables are respected during the compilation
# process. Defaults are below.
# Note:
#    CXXFLAGS_DEBUG is only added, iff $(DEBUG)  = yes
#    CXXFLAGS_OPT   is only added, iff $(DEBUG) != yes
#
#OSSS_CXXFLAGS       := -Wall
#
#OSSS_CXXFLAGS_OPT   := -O2
#OSSS_CXXFLAGS_DEBUG := -g -DDEBUG=1 -DTRACE=1

# additional target to clean up current test application
EXTRA_CLEAN=extra-clean


# -----------------------------------------------------------------------
# look for common build rules in generic places
SYSTEMC_MAKE += \
  ./systemc.mk   \
  ../systemc.mk  \
   $(SYSTEMC_HOME)/examples/systemc.mk
SYSTEMC_MAKE := $(word 1,$(wildcard $(SYSTEMC_MAKE)))
# include generic OSSS Makefile
ifneq (,$(strip $(SYSTEMC_MAKE)))
include $(SYSTEMC_MAKE)
else
$(error Common SystemC Makefile 'systemc.mk' not found! Bailing out)
endif

#
extra-clean:
	$(Q)$(call cmd-delete, $(wildcard *.vcd))

# TAF!
