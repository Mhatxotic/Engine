#!/bin/bash

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
FILEPREFIX=libjpeg-turbo-
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
  exit 4
fi

build()
{
  rm -rfv "build-${1}" 2>/dev/null
  mkdir "build-${1}" 2>/dev/null
  cd "build-${1}"
  if [ ! $? -eq 0 ]; then
    exit 5
  fi

  cmake -D"CMAKE_BUILD_TYPE=Release" \
        -D"CMAKE_OSX_ARCHITECTURES=${1}" \
        -D"CMAKE_C_FLAGS=-mtune=${2}" \
        -D"CMAKE_OSX_DEPLOYMENT_TARGET=10.15" \
        $2 \
        ..
  if [ ! $? -eq 0 ]; then
    exit 7
  fi

  make turbojpeg-static
  if [ ! $? -eq 0 ]; then
    exit 8
  fi

  mv -fv libturbojpeg.a "../jpeg64-${1}-${2}.a"
  if [ ! $? -eq 0 ]; then
    exit 9
  fi

  cd ..
  if [ ! $? -eq 0 ]; then
    exit 10
  fi
}

ISOSX=`uname`
if [ $ISOSX = 'Darwin' ]; then
  build x86_64 generic
  build arm64 apple-m1
  lipo jpeg64-*.a -create -output "${LIB}/jpeg64.ma"
else
  build x86_64 generic
  ar rcs "${LIB}/jpeg64.la" jpeg64-*.a
fi

if [ ! $? -eq 0 ]; then
  exit 11
fi

