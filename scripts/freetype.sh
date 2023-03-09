#!/bin/sh

# This file is part of the Mhatxotic Engine source repository.
# @ https://github.com/Mhatxotic/Engine
# Copyright (c) Mhatxotic Design, 2006-present. All Rights Reserved.

if [ -z $3 ]; then
  echo Usage: $0 [version] [zlibversion] [pngversion]
  exit 1
fi

BASE=~/Assets/Engine
ARCHIVE=$BASE/archive
LIB=$BASE/lib
INCLUDE=$BASE/include
FILEPREFIX=freetype-
FILEPREFIX2=zlib-
FILEPREFIX3=libpng-
FILE=$FILEPREFIX$1
FILE2=$FILEPREFIX2$2
FILE3=$FILEPREFIX3$3
ZIP=$ARCHIVE/$FILE.tar.xz
ZIP2=$ARCHIVE/$FILE2.tar.xz
ZIP3=$ARCHIVE/$FILE3.tar.xz

if [ ! -e $ZIP ]; then
  echo Error 1: $ZIP not found!
  LS=`ls $ARCHIVE/$FILEPREFIX* 2>/dev/null`
  if [ ! -z "${LS}" ]; then
    echo Available files...
    echo $LS
  fi
  exit 2
fi

if [ ! -e $ZIP2 ]; then
  echo Error 2: $ZIP2 not found!
  LS=`ls $ARCHIVE/$FILEPREFIX2* 2>/dev/null`
  if [ ! -z "${LS}" ]; then
    echo Available files...
    echo $LS
  fi
  exit 2
fi

if [ ! -e $ZIP3 ]; then
  echo Error 3: $ZIP3 not found!
  LS=`ls $ARCHIVE/$FILEPREFIX3* 2>/dev/null`
  if [ ! -z "${LS}" ]; then
    echo Available files...
    echo $LS
  fi
  exit 2
fi

tar -xvzkf $ZIP
if [ ! $? -eq 0 ]; then
  exit 3
fi

tar -xvzkf $ZIP2
if [ ! $? -eq 0 ]; then
  exit 4
fi

tar -xvzkf $ZIP3
if [ ! $? -eq 0 ]; then
  exit 5
fi

cd $FILE
if [ ! $? -eq 0 ]; then
  exit 6
fi

ZLIBDIR=`realpath ../${FILE2}`
PNGDIR=`realpath ../${FILE3}`

build()
{
  rm -rfv "build-${1}" 2>/dev/null
  mkdir "build-${1}" 2>/dev/null
  cd "build-${1}"
  if [ ! $? -eq 0 ]; then
    exit 7
  fi

  cmake -D"CMAKE_BUILD_TYPE=Release" \
        -D"CMAKE_OSX_ARCHITECTURES=${1}" \
        -D"CMAKE_OSX_DEPLOYMENT_TARGET=10.15" \
        -D"CMAKE_C_FLAGS=-mtune=${2}" \
        -D"FT_DISABLE_HARFBUZZ=TRUE" \
        -D"FT_DISABLE_BZIP2=TRUE" \
        -D"FT_DISABLE_BROTLI=TRUE" \
        -D"FT_REQUIRE_ZLIB=TRUE" \
        -D"ZLIB_INCLUDE_DIR=${ZLIBDIR}" \
        -D"ZLIB_LIBRARY_RELEASE=${ZLIBDIR}/zlib64-${1}-${2}.a" \
        -D"FT_REQUIRE_PNG=TRUE" \
        -D"PNG_INCLUDE_DIR=${PNGDIR}" \
        -D"PNG_LIBRARY_RELEASE=${PNGDIR}/png64-${1}-${2}.a" \
        $4 \
        ..
  if [ ! $? -eq 0 ]; then
    exit 8
  fi

  make
  if [ ! $? -eq 0 ]; then
    exit 7
  fi

  mv -fv libfreetype.a "../ft64-${1}-${2}.a"
  if [ ! $? -eq 0 ]; then
    exit 8
  fi

  cd ..
  if [ ! $? -eq 0 ]; then
    exit 9
  fi
}

build x86_64 generic
build arm64 apple-m1

lipo ft64-*.a -create -output "${LIB}/ft64.ma"
if [ $? -ne 0 ]; then
  exit 10
fi
