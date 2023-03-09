#!/bin/sh

# This file is part of the Mhatxotic Engine source repository.
# @ https://github.com/Mhatxotic/Engine
# Copyright (c) Mhatxotic Design, 2006-present. All Rights Reserved.

if [ -z $1 ]; then
  echo Usage: $0 [version]
  exit 1
fi

BASE=~/Assets/Engine
ISOSX=`uname`
if [ $ISOSX = 'Darwin' ]; then
  ISOSX=1
  EXTRA=-D"CMAKE_OSX_DEPLOYMENT_TARGET=10.9"
else
  ISOSX=0
fi
ARCHIVE=$BASE/archive
LIB=$BASE/lib
FILEPREFIX=sdl2-
FILE=$FILEPREFIX$1
ZIP=$ARCHIVE/$FILE.tar.gz

if [ ! -e $ZIP ]; then
  echo Error: $ZIP not found!
  LS=`echo $ARCHIVE/$FILEPREFIX* 2>/dev/null`
  if [ ! -z "${LS}" ]; then
    echo Available files...
    echo $LS | tr " " "\n"
  fi
  exit 2
fi

tar -xvzkf $ZIP
if [ ! $? -eq 0 ]; then
  exit 3
fi

cd $FILE
if [ ! $? -eq 0 ]; then
  exit 5
fi

build()
{
  BUILDDIR=build-${1}
  rm -rfv "$BUILDDIR" 2>/dev/null
  mkdir -p "$BUILDDIR" 2>/dev/null
  cd "$BUILDDIR"
  if [ ! $? -eq 0 ]; then
    exit 6
  fi

  cmake -D"CMAKE_BUILD_TYPE=Release" \
        -D"CMAKE_C_FLAGS=-mtune=${2}" \
        $EXTRA \
        -D"SDL_BUILD_TESTS=OFF" \
        -D"SDL_TESTS=OFF" \
        -D"SDL_INSTALL_TESTS=OFF" \
        $3 \
        ..
  if [ ! $? -eq 0 ]; then
    exit 7
  fi

  make
  if [ ! $? -eq 0 ]; then
    exit 8
  fi

  mv "libSDL2.a" "../libSDL2-${1}-${2}.a"
  if [ ! $? -eq 0 ]; then
    exit 9
  fi

  cd ..
  if [ ! $? -eq 0 ]; then
    exit 10
  fi
}

build x86_64 generic -D"CMAKE_OSX_ARCHITECTURES=x86_64"
if [ $ISOSX -eq 1 ]; then
  build arm64 apple-m1
  lipo libSDL2-*.a -create -output "${LIB}/sdl64.ma"
else
  mv libSDL2-*.a "${LIB}/sdl64.la"
fi
if [ ! $? -eq 0 ]; then
  exit 11
fi
