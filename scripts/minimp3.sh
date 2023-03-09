#!/bin/sh

# This file is part of the Mhatxotic Engine source repository.
# @ https://github.com/Mhatxotic/Engine
# Copyright (c) Mhatxotic Design, 2006-present. All Rights Reserved.

#if [ -z $1 ]; then
#  echo Usage: $0 [version]
#  exit 1;
#fi

BASE=~/Assets/Engine
ARCHIVE=$BASE/archive
LIB=$BASE/lib
FILEPREFIX=minimp3
FILE=$FILEPREFIX$1
ZIP=$ARCHIVE/$FILE.tar.gz

if [ ! -e $ZIP ]; then
  echo Error: $ZIP not found!
  LS=`ls $ARCHIVE/$FILEPREFIX* 2>/dev/null`
  if [ ! -z $LS ]; then
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
  gcc -O3 -arch $1 -mtune=$2 -Wno-all -mmacosx-version-min=10.15 -c minimp3.c
  if [ ! $? -eq 0 ]; then
    exit 5;
  fi

  rm -rfv "mpt64-${1}-${2}.a" 2>/dev/null

  ar rcs "mpt64-${1}-${2}.a" minimp3.o
  if [ ! $? -eq 0 ]; then
    exit 6;
  fi

  rm -rfv minimp3.o 2>/dev/null
}

build arm64 apple-m1
build x86_64 generic

lipo mpt64*.a -create -output "${LIB}/mpt64.ma"
if [ ! $? -eq 0 ]; then
  exit 7;
fi
