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
FILEPREFIX=glfw-
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

cd $FILE
if [ ! $? -eq 0 ]; then
  exit 5
fi

build()
{
  rm -rfv "build-${1}" 2>/dev/null
  mkdir -p "build-${1}" 2>/dev/null
  cd "build-${1}"
  if [ ! $? -eq 0 ]; then
    exit 6
  fi

  cmake -D"CMAKE_BUILD_TYPE=Release" \
        -D"CMAKE_C_FLAGS=-mtune=${2}" \
        $EXTRA \
        -D"GLFW_BUILD_DOCS=OFF" \
        -D"GLFW_BUILD_EXAMPLES=OFF" \
        -D"GLFW_BUILD_TESTS=OFF" \
        $3 \
        ..
  if [ ! $? -eq 0 ]; then
    exit 7
  fi

  make
  if [ ! $? -eq 0 ]; then
    exit 8
  fi

  mv src/libglfw3.a "../glfw64-${1}-${2}.a"
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
  EXTRA="-DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=x86_64"
  build x86_64 generic
  EXTRA="-DCMAKE_OSX_DEPLOYMENT_TARGET=11.0"
  build arm64 apple-m1
  lipo glfw64-*.a -create -output "${LIB}/glfw64.ma"
else
  build x86_64 generic
  mv glfw64-*.a "${LIB}/glfw64.la"
fi

if [ ! $? -eq 0 ]; then
  exit 11
fi
