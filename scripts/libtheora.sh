#!/bin/sh

# This file is part of the Mhatxotic Engine source repository.
# @ https://github.com/Mhatxotic/Engine
# Copyright (c) Mhatxotic Design, 2006-present. All Rights Reserved.

if [ -z $3 ]; then
  echo Usage: $0 [theoraversion] [oggversion] [vorbisversion]
  exit 1;
fi

BASE=~/Assets/Engine
ARCHIVE=$BASE/archive
LIB=$BASE/lib
FILEPREFIX=libtheora-
FILEPREFIX2=libogg-
FILEPREFIX3=libvorbis-
FILE=$FILEPREFIX$1
FILE2=$FILEPREFIX2$2
FILE3=$FILEPREFIX3$3
ZIP=$ARCHIVE/$FILE.tar.bz2
ZIP2=$ARCHIVE/$FILE2.tar.xz
ZIP3=$ARCHIVE/$FILE3.tar.xz

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

if [ ! -e $ZIP3 ]; then
  echo Error 2: $ZIP3 not found!
  LS=`ls $ARCHIVE/$FILEPREFIX3* 2>/dev/null`
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

tar -xvzkf $ZIP3
if [ ! $? -eq 0 ]; then
  exit 5
fi

cd $FILE
if [ ! $? -eq 0 ]; then
  exit 4;
fi

rm -rfv *.a 2>/dev/null

build()
{
  ./configure --disable-examples --without-vorbis --disable-oggtest \
    --with-ogg-includes="../${FILE2}/include/" \
    --with-ogg-libraries="../${FILE2}" \
    --with-vorbis-includes="../${FILE3}/include/" \
    --with-vorbis-libraries="../${FILE3}" \
    CC="gcc -O3 -arch ${1} -mtune=${2} -mmacosx-version-min=10.15"

  make
  if [ ! $? -eq 0 ]; then
    exit 5;
  fi

  rm -rfv "theora64-${1}-${2}.a" 2>/dev/null
  ar rcs "theora64-${1}-${2}.a" lib/*.o lib/.libs/*.o
  if [ ! $? -eq 0 ]; then
    exit 6;
  fi

  make clean 2>/dev/null
}

build arm64 apple-m1
build x86_64 generic

lipo theora64-*.a -create -output "${LIB}/theora64.ma"
if [ ! $? -eq 0 ]; then
  exit 7;
fi
