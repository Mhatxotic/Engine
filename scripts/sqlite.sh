#!/bin/sh

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
FILEPREFIX=sqlite-amalgamation
FILE=$FILEPREFIX-$1
ZIP=$ARCHIVE/$FILE.zip

if [ ! -e $ZIP ]; then
  echo Error: $ZIP not found!
  LS=`ls $ARCHIVE/$FILEPREFIX* 2>/dev/null`
  if [ ! -z "$LS" ]; then
    echo Available files...
    echo $LS
  fi
  exit 2;
fi

7zz x -aos $ZIP
if [ ! $? -eq 0 ]; then
  exit 3;
fi

cd $FILE
if [ ! $? -eq 0 ]; then
  exit 4;
fi

build()
{
  gcc -O3 -arch $1 -mtune=$2 -mmacosx-version-min=10.15 \
    -DSQLITE_DEFAULT_AUTOVACUUM=2 -DSQLITE_TEMP_STORE=2 \
    -DSQLITE_ENABLE_NULL_TRIM -DSQLITE_OS_UNIX -DSQLITE_OMIT_LOAD_EXTENSION \
    -DSQLITE_OMIT_WAL -DSQLITE_OMIT_UTF16 -DSQLITE_OMIT_DEPRECATED \
    -DSQLITE_THREADSAFE -DSQLITE_ENABLE_MATH_FUNCTIONS \
    -c sqlite3.c
  if [ ! $? -eq 0 ]; then
    exit 5;
  fi

  rm -rfv "sqlite64-${1}-${2}.a" 2>/dev/null

  ar rcs "sqlite64-${1}-${2}.a" sqlite3.o
  if [ ! $? -eq 0 ]; then
    exit 6;
  fi

  rm -rfv sqlite3.o 2>/dev/null
}


build arm64 apple-m1
build x86_64 generic

lipo sqlite64*.a -create -output "${LIB}/sqlite64.ma"
if [ ! $? -eq 0 ]; then
  exit 7;
fi
