#!/bin/bash

# This file is part of the Mhatxotic Engine source repository.
# @ https://github.com/Mhatxotic/Engine
# Copyright (c) Mhatxotic Design, 2006-present. All Rights Reserved.

if [ -z $1 ]; then
  echo Usage: $0 [version]
  exit 1;
fi

BASE=~/Assets/Engine
ARCHIVE=$BASE/archive
LIB=$BASE/lib
FILEPREFIX=libnsgif-
FILE=$FILEPREFIX$1
ZIP=$ARCHIVE/$FILE.tar.gz

if [ ! -e $ZIP ]; then
  echo Error: $ZIP not found!
  LS=`ls $ARCHIVE/$FILEPREFIX* 2>/dev/null`
  if [ ! -z "${LS}" ]; then
    echo Available files...
    echo $LS
  fi
  exit 2;
fi

tar -xvzkf $ZIP
if [ ! $? -eq 0 ]; then
  exit 3;
fi

cd $FILE
if [ ! $? -eq 0 ]; then
  exit 4;
fi

build()
{
  gcc -O3 $ARCH$1 $EXTRA -Iinclude -c src/*.c

  if [ ! $? -eq 0 ]; then
    exit 5;
  fi

  rm -rfv "nsgif64-${1}-${2}.a" 2>/dev/null
  ar rcs "nsgif64-${1}-${2}.a" *.o
  if [ ! $? -eq 0 ]; then
    exit 6;
  fi

  rm -rfv *.o
}

PLATFORM=$(uname)
if [ "$PLATFORM" == 'Linux' ]; then
  ARCH="-D"
  EXTRA=""
  build x86_64 generic
  mv nsgif64-*.a "${LIB}/nsgif64.la"
else
  ARCH="-arch "
  EXTRA="-mmacosx-version-min=10.15"
  build arm64 apple-m1
  build x86_64 generic
  lipo nsgif64-*.a -create -output "${LIB}/nsgif64.ma"
fi

if [ ! $? -eq 0 ]; then
  exit 7;
fi

