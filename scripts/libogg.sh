#!/bin/sh

# This file is part of the Mhatxotic Engine source repository.
# @ https://github.com/Mhatxotic/Engine
# Copyright (c) Mhatxotic Design, 2006-present. All Rights Reserved.

if [ -z $2 ]; then
  echo Usage: $0 [oggversion] [vorbisversion]
  exit 1;
fi

BASE=~/Assets/Engine
ARCHIVE=$BASE/archive
LIB=$BASE/lib
FILEPREFIX=libogg-
FILEPREFIX2=libvorbis-
FILE=$FILEPREFIX$1
FILE2=$FILEPREFIX2$2
ZIP=$ARCHIVE/$FILE.tar.xz
ZIP2=$ARCHIVE/$FILE2.tar.xz

if [ ! -e $ZIP ]; then
  echo Error 1: $ZIP not found!
  LS=`ls $ARCHIVE/$FILEPREFIX* 2>/dev/null`
  if [ ! -z $LS ]; then
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

rm -rfv "${FILE2}/lib/barkmel.c" \
        "${FILE2}/lib/tone.c" \
        "${FILE2}/lib/psytune.c" 2>/dev/null
if [ ! $? -eq 0 ]; then
  exit 6;
fi

cd $FILE
if [ ! $? -eq 0 ]; then
  exit 7;
fi

build()
{
  gcc -O3 -arch $1 -mtune=$2 -mmacosx-version-min=10.15 \
    -Iinclude \
    -I"../${FILE2}/include" \
    -I"../${FILE2}/lib" \
    -c ../${FILE2}/lib/*.c src/*.c
  if [ ! $? -eq 0 ]; then
    exit 8;
  fi

  rm -rfv "ogg64-${1}-${2}.a" 2>/dev/null

  ar rcs "ogg64-${1}-${2}.a" *.o
  if [ ! $? -eq 0 ]; then
    exit 9;
  fi

  rm -rf *.o 2>/dev/null
  if [ ! $? -eq 0 ]; then
    exit 10;
  fi
}

build arm64 apple-m1
build x86_64 generic

lipo ogg64-*.a -create -output "${LIB}/ogg64.ma"
if [ ! $? -eq 0 ]; then
  exit 11;
fi
