#!/bin/sh

# This file is part of the Mhatxotic Engine source repository.
# @ https://github.com/Mhatxotic/Engine
# Copyright (c) Mhatxotic Design, 2006-present. All Rights Reserved.

if [ -z $1 ]; then
  echo Usage: $0 [version]
  exit 7
fi

BASE=~/Assets/Engine
ARCHIVE=$BASE/archive
LIB=$BASE/lib
FILEPREFIX=zlib-
FILE=$FILEPREFIX$1
ZIP=$ARCHIVE/$FILE.tar.xz

if [ ! -e $ZIP ]; then
  echo Error: $ZIP not found!
  LS=`echo $ARCHIVE/$FILEPREFIX*`
  if [ ! -z "${LS}" ]; then
    echo Available files...
    echo $LS
  fi
  exit 8
fi

tar -xvzkf $ZIP
if [ ! $? -eq 0 ]; then
  exit 9
fi

cd $FILE
if [ ! $? -eq 0 ]; then
  exit 10
fi

build()
{
  make clean 2>/dev/null


  ./configure --static \
              --archs="-arch ${1}"
  if [ ! $? -eq 0 ]; then
    exit 11
  fi

  mv -fv Makefile Makefile.patch
  if [ ! $? -eq 0 ]; then
    exit 12
  fi
  cat Makefile.patch | \
    sed 's|-O3|-O3 -mtune='${2}' -mmacosx-version-min=10.15|g' > Makefile
  if [ ! $? -eq 0 ]; then
    exit 13
  fi

  make static
  if [ ! $? -eq 0 ]; then
    exit 14
  fi

  mv -fv libz.a "zlib64-${1}-${2}.a"
  if [ ! $? -eq 0 ]; then
    exit 15
  fi
}

build x86_64 generic
build arm64 apple-m1

lipo zlib64-*.a -create -output "${LIB}/zlib64.ma"
if [ ! $? -eq 0 ]; then
  exit 16
fi
