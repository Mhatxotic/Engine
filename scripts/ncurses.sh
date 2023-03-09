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
FILEPREFIX=ncurses-
FILE=$FILEPREFIX$1
ZIP=$ARCHIVE/$FILE.tar.gz

if [ ! -e $ZIP ]; then
  echo Error: $ZIP not found!
  LS=`ls $ARCHIVE/$FILEPREFIX* 2>/dev/null`
  if [ ! -z $LS ]; then
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

  ./configure --prefix=/usr --without-cxx --without-cxx-binding \
    --without-ada --without-progs --without-curses-h --with-static \
    --without-debug --enable-widec --enable-const --enable-ext-colors \
    --enable-sigwinch --enable-wgetch-events --target=${1}-apple-darwin
  if [ ! $? -eq 0 ]; then
    exit 11
  fi

  find . -type f -name Makefile | while read -r FILE; do
    mv -fv $FILE $FILE.patch
    if [ ! $? -eq 0 ]; then
      exit 12
    fi
    cat $FILE.patch | \
      sed 's|-O2|-O3 -arch '${1}' -mtune='${2}' -mmacosx-version-min='${3}' -target '${1}'-apple-darwin -Wno-deprecated-declarations -Wno-c++11-extensions|g' > $FILE
    if [ ! $? -eq 0 ]; then
      exit 13
    fi
  done

  make
  if [ ! $? -eq 0 ]; then
    exit 14
  fi

  mv -fv lib/libncursesw.a "nc64-${1}-${2}.a"
  if [ ! $? -eq 0 ]; then
    exit 15
  fi
}

build x86_64 generic 10.15
build arm64 apple-m1 11.0

lipo nc64-*.a -create -output "${LIB}/nc64.ma"
if [ ! $? -eq 0 ]; then
  exit 16
fi
