
# File: MetadataToolkit/Build/UNIX/release.mak

CC_DEBUG_FLAGS = \
   -DNDEBUG \
   -O2 \
   -fno-omit-frame-pointer \
   -ffloat-store \
   -fkeep-inline-functions

XMP_DEBUG_FLAGS = \
   -DXAP_DEBUG_BUILD=0

