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
FILEPREFIX=openssl-
FILE=$FILEPREFIX$1
ZIP=$ARCHIVE/$FILE.tar.gz
SRC1=libcrypto.a
SRC2=libssl.a

if [ ! -e $ZIP ]; then
  echo Error: $ZIP not found!
  LS=`echo $ARCHIVE/$FILEPREFIX*`
  if [ ! -z "$LS" ]; then
    echo Available files...
    echo $LS
  fi
  exit 2
fi

tar -xvzkf $ZIP
if [ ! $? -eq 0 ]; then
  exit 3
fi

cd $FILE
if [ ! $? -eq 0 ]; then
  exit 4
fi

build()
{
  if [ -e "ssl64-${1}-${2}.a" ]; then
    return
  fi

  make clean

  perl Configure $1 -mtune=$2 $3 \
    no-aria no-blake2 no-camellia no-capieng no-cast no-cmac no-cmp no-cms \
    no-comp no-deprecated no-des no-devcryptoeng no-dh no-dsa no-dso no-dtls1 \
    no-dynamic-engine no-ecdh no-engine no-external-tests no-filenames \
    no-gost no-idea no-legacy no-makedepend no-md4 no-mdc2 no-module no-ocb \
    no-pic no-pinshared no-rc2 no-rc4 no-rmd160 no-scrypt no-seed no-shared \
    no-siphash no-siv no-sm2 no-sm3 no-sm4 no-srp no-srtp no-ssl-trace \
    no-ssl3 no-stdio no-tests no-tls1 no-tls1_1 no-trace no-ts no-ubsan \
    no-ui-console no-unit-test no-uplink no-weak-ssl-ciphers no-whirlpool \
    no-zlib no-zlib-dynamic

  if [ $? -ne 0 ]; then
    exit 5
  fi

  make
  if [ $? -ne 0 ]; then
    exit 6
  fi

  ar -x $SRC1
  if [ $? -ne 0 ]; then
    exit 7
  fi

  ar -x $SRC2
  if [ $? -ne 0 ]; then
    exit 8
  fi

  rm -fv $SRC1 $SRC2
  if [ $? -ne 0 ]; then
    exit 9
  fi

  rm -rfv "ssl64-${1}-${2}.a" 2>/dev/null
  ar rcs "ssl64-${1}-${2}.a" *.o
  if [ $? -ne 0 ]; then
    exit 10
  fi

  rm -fv *.o
  if [ $? -ne 0 ]; then
    exit 11
  fi

  cp -fv "include/openssl/configuration.h" "include/openssl/configuration-$4.h"
  if [ $? -ne 0 ]; then
    exit 12
  fi

  rm -rf "include/openssl/configuration.h"
  if [ $? -ne 0 ]; then
    exit 13
  fi
}

PLATFORM=$(uname)
if [ "$PLATFORM" == 'Linux' ]; then
  build linux-x86_64 generic "" l64
  ar rcs "${LIB}/ssl64.la" ssl64-*.a
else
  build darwin64-x86_64 generic -mmacosx-version-min=10.15 o32
  build darwin64-arm64 apple-m1 -mmacosx-version-min=11.0 o64
  lipo ssl64-*.a -create -output "${LIB}/ssl64.ma"
fi

if [ $? -ne 0 ]; then
  exit 13
fi
