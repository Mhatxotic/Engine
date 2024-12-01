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
FILEPREFIX=lzma
FILE=$FILEPREFIX$1
ZIP=$ARCHIVE/$FILE.7z

if [ ! -e $ZIP ]; then
  echo Error: $ZIP not found!
  LS=`ls $ARCHIVE/$FILEPREFIX* 2>/dev/null`
  if [ ! -z "$LS" ]; then
    echo Available files...
    echo $LS
  fi
  exit 2;
fi

7zz x -aos $ZIP -o$FILE
if [ ! $? -eq 0 ]; then
  exit 3;
fi

chmod -R 700 $FILE
if [ ! $? -eq 0 ]; then
  exit 4;
fi

cd $FILE/C
if [ ! $? -eq 0 ]; then
  exit 5;
fi

# We handle this (universal compatibility)
if [ ! -f CpuArch.cpatched ]; then
  cp CpuArch.c CpuArch.cpatch
  if [ ! $? -eq 0 ]; then
    exit 6;
  fi
  sed '/\#define USE_ASM/d' CpuArch.cpatch > CpuArch.c
  if [ ! $? -eq 0 ]; then
    exit 7;
  fi
  mv CpuArch.cpatch CpuArch.cpatched
  if [ ! $? -eq 0 ]; then
    exit 8;
  fi
fi

build()
{
  TYPE=$1
  shift

  SUBTYPE=$1
  shift

  ASMOPT=$1
  shift
  if [ $ASMOPT -eq 1 ]; then
    ASMOPT="-D_LZMA_DEC_OPT"
  else
    ASMOPT=""
  fi

  ARCH="lzma64-${TYPE}-${SUBTYPE}.a"

  gcc $EXTRA$TYPE -mtune=$SUBTYPE -O3 -Wall -Wextra \
      -DUSE_ASM $ASMOPT -D_REENTRANT\
      -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DNDEBUG \
      -Wno-implicit-function-declaration \
      -Wno-#pragma-messages \
      -c $* *.c
  if [ ! $? -eq 0 ]; then
    exit 9;
  fi

  rm -rf $ARCH 2>/dev/null

  ar rcs $ARCH *.o
  if [ ! $? -eq 0 ]; then
    exit 10;
  fi

  rm *.o
  if [ ! $? -eq 0 ]; then
    exit 11;
  fi
}

ISOSX=`uname`
if [ $ISOSX = 'Darwin' ]; then
  EXTRA="-mmacosx-version-min=10.15 -arch "
  build x86_64 generic 0
  build arm64 apple-m1 1 ../Asm/arm64/*.S
  lipo lzma64-*.a -create -output "${LIB}/lzma64.ma"
else
  EXTRA="-D"
  build x86_64 generic 0
  ar rcs "${LIB}/lzma64.la" lzma64-*.a
fi

if [ ! $? -eq 0 ]; then
  exit 12;
fi
