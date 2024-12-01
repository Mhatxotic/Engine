#!/bin/sh

# This file is part of the Mhatxotic Engine source repository.
# @ https://github.com/Mhatxotic/Engine
# Copyright (c) Mhatxotic Design, 2006-present. All Rights Reserved.

if [ -z $1 ]; then
  echo Usage: $0 [version]
  exit 1
fi

BASE=~/Assets/Engine
ARCHIVE=$BASE/archive
LIB=$BASE/lib
FILEPREFIX=openal-soft-
FILE=$FILEPREFIX$1
ZIP=$ARCHIVE/$FILE.zip

if [ ! -e $ZIP ]; then
  echo Error: $ZIP not found!
  LS=`echo $ARCHIVE/$FILEPREFIX* 2>/dev/null`
  if [ ! -z "${LS}" ]; then
    echo Available files...
    echo $LS | tr " " "\n"
  fi
  exit 2
fi

unzip -n $ZIP
if [ ! $? -eq 0 ]; then
  exit 3
fi

FILE=${FILE:r}
cd $FILE
if [ ! $? -eq 0 ]; then
  exit 4
fi

build()
{
  rm -rfv CMakeFiles CMakeCache* 2>/dev/null

  cmake -D"LIBTYPE=STATIC" \
        -D"CMAKE_BUILD_TYPE=Release" \
        -D"CMAKE_OSX_ARCHITECTURES=${1}" \
        -D"CMAKE_CXX_FLAGS=-mtune=${2} -stdlib=libc++" \
        -D"CMAKE_OSX_DEPLOYMENT_TARGET=${3}" \
        -D"ALSOFT_BACKEND_PORTAUDIO=FALSE" \
        -D"ALSOFT_BACKEND_WAVE=FALSE" \
        -D"ALSOFT_DLOPEN=FALSE" \
        -D"ALSOFT_EMBED_HRTF_DATA=FALSE" \
        -D"ALSOFT_EXAMPLES=FALSE" \
        -D"ALSOFT_INSTALL_AMBDEC_PRESETS=FALSE" \
        -D"ALSOFT_INSTALL_CONFIG=FALSE" \
        -D"ALSOFT_INSTALL_EXAMPLES=FALSE" \
        -D"ALSOFT_INSTALL_HRTF_DATA=FALSE" \
        -D"ALSOFT_INSTALL_UTILS=FALSE" \
        -D"ALSOFT_INSTALL=FALSE" \
        -D"ALSOFT_NO_CONFIG_UTIL=TRUE" \
        -D"ALSOFT_REQUIRE_SDL2=FALSE" \
        -D"ALSOFT_UPDATE_BUILD_VERSION=FALSE" \
        -D"ALSOFT_UTILS=FALSE" \
        -D"SDL2_CORE_LIBRARY=FALSE" \
        -D"SDL2_INCLUDE_DIR=FALSE" \
        .
  if [ ! $? -eq 0 ]; then
    exit 6
  fi

  make
  if [ ! $? -eq 0 ]; then
    exit 7
  fi

  find CMakeFiles/OpenAL.dir -name *.o -print > openalfiles.txt
  if [ ! $? -eq 0 ]; then
    exit 8
  fi

  while read p; do
    nn=`echo $p | sed 's|CMakeFiles/OpenAL\.dir/||g'`
    if [ ! $? -eq 0 ]; then
      exit 9
    fi

    nn=`echo $nn | sed 's|/|-|g'`
    if [ ! $? -eq 0 ]; then
      exit 10
    fi

    mv -fv "$p" "${nn}-${1}-${2}.o"
    if [ ! $? -eq 0 ]; then
      exit 11
    fi
  done <openalfiles.txt

  rm -fv openalfiles.txt
  if [ ! $? -eq 0 ]; then
    exit 11
  fi

  rm -rfv "al64-${1}-${2}.a" 2>/dev/null
  ar rcs "al64-${1}-${2}.a" *.o
  if [ ! $? -eq 0 ]; then
    exit 12
  fi

  rm -rfv *.o
  if [ ! $? -eq 0 ]; then
    exit 13
  fi
}

build x86_64 generic 10.15
build arm64 apple-m1 11.0

lipo al64-*.a -create -output "${LIB}/al64.ma"
if [ ! $? -eq 0 ]; then
  exit 15
fi
