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

PKG_DIR?=..
ENGINE_DIR?=engine

ifneq (,$(wildcard ../../sld-extern/lib/libIrrlicht.a))
IRRLICHT_INCLUDE = ../../sld-extern/include
IRRLICHT_LIB     = ../../sld-extern/lib
endif

ifneq (,$(wildcard ../sld-extern/lib/libIrrlicht.a))
IRRLICHT_INCLUDE = ../sld-extern/include
IRRLICHT_LIB     = ../sld-extern/lib
endif

# Path to Irrlicht lib and headers
IRRLICHT_INCLUDE ?= /usr/local/X11/include
IRRLICHT_LIB     ?= /usr/local/X11/lib

#
# Configuration section of this SystemC project
# (if a variable is omitted here, a default value is used)
#

# path (and name) of required "systemc.mk"
#SYSTEMC_MAKE := ../systemc.mk

# defaults to SystemC path on the ARBI cluster
SYSTEMC_HOME ?= /usr/local/lib/systemc


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

# This is the ARBI installation path for or1ksim
OR1KSIM_HOME   ?= /usr/local/lib/or1k
OR1KSIM_LIB     = $(OR1KSIM_HOME)/lib
OR1KSIM_INCLUDE = $(OR1KSIM_HOME)/include

X11_LIB = /usr/local/X11/lib


# Use this variable to build Exercise 5, Task 1 instead of the default Task 2
# Usage: make clean sim TASK=1
TASK ?= 2

export LD_LIBRARY_PATH = $(SYSTEMC_LIB):$(OR1KSIM_LIB):$(X11_LIB)

# Name of the test application (required!)
MODULE=line-follower

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
SRCDIRS := .
# Source files
#SRCS := example1.cpp example2.cpp

#
# Other variables, that can be set here
#
# additional include options
EXTRA_INCLUDES := -I$(ENGINE_DIR) -I../uart -isystem $(OR1KSIM_INCLUDE) -I$(IRRLICHT_INCLUDE)
#
# additional library directories and link directives
EXTRA_LIBDIRS := -L$(X11_LIB) -L$(ENGINE_DIR) -L$(OR1KSIM_LIB) -L$(IRRLICHT_LIB)
EXTRA_LIBS    := -lengine -lIrrlicht -lGL -lXxf86vm -lXext -lX11 -lsim

# additional preprocessor symbols to define
# (as list of -Dmacro[=defn])
#EXTRA_DEFINES := -DHURZ -Dever=;;

#
# Various settings
#

# Verbose compilation output? (yes,no)
VERBOSE=no

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
#EXTRA_CLEAN=extra-clean
#
#extra-clean:
#	$(DEL) my-test-file.out

build-one sim-one:     EXTRA_DEFINES+=-DSOLUTION_INCLUDED -DASSIGNMENT_ONE -DTASK=$(TASK)
build-one: all
sim-one: build-one sim

build-two sim-two:     EXTRA_DEFINES+=-DSOLUTION_INCLUDED -DASSIGNMENT_TWO -DTASK=$(TASK)
build-two: clean all
sim-two: build-two sim

build-four sim-four: EXTRA_DEFINES+=-DSOLUTION_INCLUDED -DASSIGNMENT_FOUR -DTASK=$(TASK)
build-four: all
sim-four: build-four sim

engine/libengine.a:
	$(MAKE) -C engine

.PHONY: engine-clean
engine-clean:
	-$(MAKE) -C engine clean

clean: engine-clean
main.o: engine/libengine.a


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


# TAF!
