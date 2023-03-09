#!/bin/sh

# This file is part of the Mhatxotic Engine source repository.
# @ https://github.com/Mhatxotic/Engine
# Copyright (c) Mhatxotic Design, 2006-present. All Rights Reserved.

if [ -z $2 ]; then
  echo Usage: $0 [version] [zlibversion]
  exit 1
fi

BASE=~/Assets/Engine
ARCHIVE=$BASE/archive
LIB=$BASE/lib
FILEPREFIX=libpng-
FILEPREFIX2=zlib-
FILE=$FILEPREFIX$1
FILE2=$FILEPREFIX2$2
ZIP=$ARCHIVE/$FILE.tar.xz
ZIP2=$ARCHIVE/$FILE2.tar.xz

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
  if [ ! -z $LS ]; then
    echo Available files...
    echo $LS
  fi
  exit 3
fi

tar -xvzkf $ZIP
if [ ! $? -eq 0 ]; then
  exit 4
fi

tar -xvzkf $ZIP2
if [ ! $? -eq 0 ]; then
  exit 5
fi

cd $FILE
if [ ! $? -eq 0 ]; then
  exit 6
fi

ZLIBDIR=`realpath ../${FILE2}`

build()
{
  make clean 2>/dev/null

  ./configure --with-zlib-prefix="${ZLIBDIR}" \
              CC="gcc -O3 -arch ${1} -mtune=${2} -mmacosx-version-min=10.15"
  if [ ! $? -eq 0 ]; then
    cat config.log
    exit 7
  fi

  make
  if [ ! $? -eq 0 ]; then
    exit 8
  fi

  mv .libs/libpng16.a "png64-${1}-${2}.a"
  if [ ! $? -eq 0 ]; then
    exit 9
  fi
}

build arm64 apple-m1
build x86_64 generic

lipo png64-*.a -create -output "${LIB}/png64.ma"
if [ ! $? -eq 0 ]; then
  exit 10
fi
