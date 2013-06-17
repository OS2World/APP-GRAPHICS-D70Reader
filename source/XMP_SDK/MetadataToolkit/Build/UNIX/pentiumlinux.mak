
# File: MetadataToolkit/Build/UNIX/pentiumlinux.mak

TOOL_PATH= /user/unibat/gnutools/linux/gcc_2.95
GNU_PATH = $(TOOL_PATH)/bin:/usr/bin

CC = PATH=$(GNU_PATH) gcc -x c++
LD = PATH=$(GNU_PATH) gcc
AS = PATH=$(GNU_PATH) as
AR = PATH=$(GNU_PATH) ar

CC_DEPEND = PATH=$(GNU_PATH) gcc -MM -E

CC_PLATFORM_FLAGS = -DPENTIUM -DLINUX -DWSTRING_SUPPORTED=0 -fpcc-struct-return -fPIC

AR_FLAGS = q
LD_FLAGS = -shared

RANLIB = $(AR) ts

STATIC_LIB  = libxmptk.a
DYNAMIC_LIB = libxmptk.so

all : $(DYNAMIC_LIB) $(STATIC_LIB)

