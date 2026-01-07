#!/bin/bash

# This file is part of the Mhatxotic Engine source repository.
# @ https://github.com/Mhatxotic/Engine
# Copyright (c) Mhatxotic Design, 2006-present. All Rights Reserved.

# Usage check
if [ -z $1 ]; then echo Usage: $0 [version]; exit 1; fi

# Base directory
BASE=~/Assets/Engine
# Archives directory
ARCHIVE=$BASE/archive
# Libraries directory
LIB=$BASE/lib
# Archive prefix
FILEPREFIX=lua-
# Specified archive to find
FILE=$FILEPREFIX$1
# Location of archive
ZIP=$ARCHIVE/$FILE.tar.gz

# Archive doesn't exist
if [ ! -e $ZIP ]; then
  # Archive not found
  echo Error: $ZIP not found!
  # Print listing of possible archives
  LS=`ls $ARCHIVE/$FILEPREFIX* 2>/dev/null`
  if [ ! -z "${LS}" ]; then
    echo Available files...
    echo $LS | tr " " "\n"
  fi
  # Abnormal exit
  exit 2
fi

# Extract tarball and exit if failed
echo "*** Extracting $ZIP ***"
tar -xvzkf $ZIP
if [ ! $? -eq 0 ]; then exit 3; fi

# Switch to archive directory and exit if failed
echo "*** Switching directory to $FILE ***"
cd $FILE
if [ ! $? -eq 0 ]; then exit 4; fi

# Erase Lua source files we don't need and exit if failed
echo "*** Removing files ***"
rm -rfv src/lua.c src/luac.c src/lbitlib.* src/liolib.* src/lloadlib.* \
        src/loslib.* src/loadlib.*
if [ ! $? -eq 0 ]; then exit 5; fi

# Patching files
echo "*** Applying patches ***"

# Patch source file to remove namespaces we don't need
if [ ! -f src/linit.c.patched ]; then
  mv -fv src/linit.c src/linit.c.patch 2>/dev/null
  cat src/linit.c.patch | \
    sed 's/{LUA_IOLIBNAME, luaopen_io},//g' | \
    sed 's/{LUA_OSLIBNAME, luaopen_os},//g' | \
    sed 's/{LUA_LOADLIBNAME, luaopen_package},//g' > src/linit.c
  if [ ! $? -eq 0 ]; then exit 6; fi
  mv -fv src/linit.c.patch src/linit.c.patched
  if [ ! $? -eq 0 ]; then exit 7; fi
fi

# Patch source file to remove functions we don't need
if [ ! -f src/lbaselib.c.patched ]; then
  mv -fv src/lbaselib.c src/lbaselib.c.patch 2>/dev/null
  cat src/lbaselib.c.patch | \
    sed 's/{"dofile", luaB_dofile},//g' | \
    sed 's/{"loadfile", luaB_loadfile},//g' | \
    sed 's/{"load", luaB_load},//g' | \
    sed 's/{"print\", luaB_print},//g' | \
    sed 's/{"_VERSION", nullptr},//g' | \
    sed 's/{"_G", nullptr},//g' \
      > src/lbaselib.c
  if [ ! $? -eq 0 ]; then exit 8; fi
  mv -fv src/lbaselib.c.patch src/lbaselib.c.patched
  if [ ! $? -eq 0 ]; then exit 9; fi
fi

# Patch source file to allow more local variables
if [ ! -f src/lparser.c.patched ]; then
  mv -fv src/lparser.c src/lparser.c.patch 2>/dev/null
  cat src/lparser.c.patch | sed \
    's/MAXVARS\t\t200/MAXVARS\t\t253/g' > src/lparser.c
  if [ ! $? -eq 0 ]; then exit 10; fi
  mv -fv src/lparser.c.patch src/lparser.c.patched
  if [ ! $? -eq 0 ]; then exit 11; fi
fi

# Function to run a command on files if they exist
exists()
{
  cmd=$1
  shift
  for f in $*; do if [ -e "$f" ]; then $cmd $* 2>/dev/null; break; fi; done
}

# Remove archive files if they exist
echo "*** Removing existing archives ***"
exists "rm -rfv" *.a

build()
{
  # Remove object files if they exist
  echo "*** Building $1 ($2) version ***"
  exists "rm -rfv" *.o
  # Do requested compilation and exit if failed
  g++ -O3 $ARCH$1 -mtune=$2 \
    $EXTRA \
    -std=c++20 \
    -DLUA_USE_APICHECK \
    -Wno-deprecated \
    -c src/*.c
  if [ ! $? -eq 0 ]; then exit 12; fi
  # Make an archive of objects and exit if failed
  ar rcs "lua64-${1}-${2}.a" *.o
  if [ ! $? -eq 0 ]; then exit 13; fi
}

# Get platform and if running Linux?
PLATFORM=$(uname)
if [ "$PLATFORM" == 'Linux' ]; then
  # Linux options
  ARCH="-D"
  EXTRA="-DLUA_USE_LINUX"
  # Build Linux source
  build x86_64 generic
  # Move built archive into position
  SRC=`echo lua64-*.a`
  DST="${LIB}/lua64.la"
  echo "*** Moving $SRC to $DST"
  mv -fv $SRC $DST
  if [ ! $? -eq 0 ]; then exit 15; fi
# If running MacOS?
elif [ "$PLATFORM" == 'Darwin' ]; then
  # Mac options
  ARCH="-arch "
  EXTRA="-DLUA_USE_MACOSX -mmacosx-version-min=10.15 -stdlib=libc++"
  # Build MacOS ARM and X86 versions
  build arm64 apple-m1
  build x86_64 generic
  # Merge both into a universal library and exit if failed
  SRC=`echo lua64-*.a`
  DST="${LIB}/lua64.ma"
  echo "*** Joining $SRC to $DST"
  lipo $SRC -create -output $DST
  if [ ! $? -eq 0 ]; then exit 16; fi
# Unknown platform?
else
  # Show unknown platform and exit
  echo Unknown platform '$PLATFORM'!
  exit 17;
fi

# Success
echo "*** Completed ***"
