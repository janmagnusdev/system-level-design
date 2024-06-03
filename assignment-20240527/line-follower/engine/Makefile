Target := libengine.a

# List of source files, separated by spaces
Sources := auto.cpp simulation.cpp

# X11 include directory
X11_INCLUDE:=/usr/include/X11

ifneq (,$(wildcard ../../../sld-extern/lib/libIrrlicht.a))
IRRLICHT_INCLUDE = ../../../sld-extern/include
IRRLICHT_LIB     = ../../../sld-extern/lib
endif

ifneq (,$(wildcard ../../sld-extern/lib/libIrrlicht.a))
IRRLICHT_INCLUDE = ../../sld-extern/include
IRRLICHT_LIB     = ../../sld-extern/lib
endif

# Path to Irrlicht lib and headers
IRRLICHT_INCLUDE ?= /usr/local/X11/include
IRRLICHT_LIB     ?= /usr/local/X11/lib

# general compiler settings (might need to be set when compiling the lib, too)
# preprocessor flags, e.g. defines and include paths
USERCPPFLAGS =
# compiler flags such as optimization flags
USERCXXFLAGS = -ffast-math -g -Wall
#USERCXXFLAGS = -g -Wall
# linker flags such as additional libraries and link paths
USERLDFLAGS = -g

CXX ?= clang++

####
#no changes necessary below this line
####

CPPFLAGS = -I$(IRRLICHT_INCLUDE) -I$(X11_INCLUDE) $(USERCPPFLAGS)
CXXFLAGS = $(USERCXXFLAGS)
LDFLAGS = $(USERLDFLAGS)

#default target is Linux
all: all_linux

all_linux clean_linux: SYSTEM=Linux

all_win32 clean_win32 static_win32: SYSTEM=Win32-gcc
static_win32: CPPFLAGS += -D_IRR_STATIC_LIB_
all_win32: LDFLAGS += -lopengl32 -lm
static_win32: LDFLAGS += -lgdi32 -lwinspool -lcomdlg32 -lole32 -loleaut32 -luuid -lodbc32 -lodbccp32 -lopengl32

export SYSTEM

$(Target): $(Sources:.cpp=.o)
	$(warning Creating library...)
	$(AR) rcs $@ $^

all_linux all_win32 static_win32: $(Target)

clean:
	-$(RM) $(Target) $(Sources:.cpp=.o)

.PHONY: all all_win32 static_win32 clean clean_linux clean_win32

