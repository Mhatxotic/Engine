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
FILEPREFIX=lua-
FILE=$FILEPREFIX$1
ZIP=$ARCHIVE/$FILE.tar.gz

if [ ! -e $ZIP ]; then
  echo Error: $ZIP not found!
  LS=`ls $ARCHIVE/$FILEPREFIX* 2>/dev/null`
  if [ ! -z "${LS}" ]; then
    echo Available files...
    echo $LS | tr " " "\n"
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

rm -rfv src/lua.c src/luac.c src/lbitlib.* src/liolib.* src/lloadlib.* \
        src/loslib.* src/loadlib.*
if [ ! $? -eq 0 ]; then
  exit 5
fi

if [ ! -f src/linit.c.patched ]; then
  mv -fv src/linit.c src/linit.c.patch 2>/dev/null
  cat src/linit.c.patch | \
    sed 's/{LUA_IOLIBNAME, luaopen_io},//g' | \
    sed 's/{LUA_OSLIBNAME, luaopen_os},//g' | \
    sed 's/{LUA_LOADLIBNAME, luaopen_package},//g' > src/linit.c
  if [ ! $? -eq 0 ]; then
    exit 7
  fi
  mv -fv src/linit.c.patch src/linit.c.patched
  if [ ! $? -eq 0 ]; then
    exit 8
  fi
fi

if [ ! -f src/ldo.c.patched ]; then
  mv -fv src/ldo.c src/ldo.c.patch 2>/dev/null
  cat src/ldo.c.patch | \
    sed 's/throw(c)/throw(c)\n\
#include <stdexcept>\n\
#include "lauxlib.h"\n/g' | \
    sed 's/L->errorJmp = &lj;/L->errorJmp = \&lj;\
#if defined(__cplusplus)\
\ttry{\
\t(*f)(L, ud);\
\t}\
\tcatch(struct lua_longjmp *lj){\
\t\tif(!lj->status) lj->status=-1;\
\t}\
\tcatch(const std::exception \&e){\
\t\ttry{\
\t\t\tluaL_where(L,1);\
\t\t\tlua_pushstring(L,e.what());\
\t\t\tlua_concat(L,2);\
\t\t\tlua_error(L);\
\t\t}\
\t\tcatch(struct lua_longjmp *lj){\
\t\t\tif(!lj->status) lj->status=-1;\
\t\t}\
\t}\
#else\n/g' | \
    sed 's/L->errorJmp = lj.previous;/\
#endif\n\
\tL->errorJmp = lj.previous;/g' \
      > src/ldo.c
  if [ ! $? -eq 0 ]; then
    exit 9
  fi
  mv -fv src/ldo.c.patch src/ldo.c.patched
  if [ ! $? -eq 0 ]; then
    exit 10
  fi
fi

if [ ! -f src/lbaselib.c.patched ]; then
  mv -fv src/lbaselib.c src/lbaselib.c.patch 2>/dev/null
  cat src/lbaselib.c.patch | \
   sed 's/{"dofile", luaB_dofile},//g' | \
   sed 's/{"loadfile", luaB_loadfile},//g' | \
   sed 's/{"load", luaB_load},//g' | \
   sed 's/{"print\", luaB_print},//g' | \
   sed 's/{"_VERSION", nullptr},//g' | \
   sed 's/{"_G", nullptr},//g' \
     > src/lbaselib.c
  if [ ! $? -eq 0 ]; then
    exit 11
  fi
  mv -fv src/lbaselib.c.patch src/lbaselib.c.patched
  if [ ! $? -eq 0 ]; then
    exit 12
  fi
fi

if [ ! -f src/lparser.c.patched ]; then
  mv -fv src/lparser.c src/lparser.c.patch 2>/dev/null
  cat src/lparser.c.patch | sed \
    's/MAXVARS\t\t200/MAXVARS\t\t253/g' > src/lparser.c
  if [ ! $? -eq 0 ]; then
    exit 13
  fi
  mv -fv src/lparser.c.patch src/lparser.c.patched
  if [ ! $? -eq 0 ]; then
    exit 14
  fi
fi

# if [ ! -f src/lua.h.patched ]; then
#   mv -fv src/lua.h src/lua.h.patch 2>/dev/null
#   cat src/lua.h.patch | sed \
#     "s/LUA_MINSTACK\t20|LUA_MINSTACK\t20/g" > src/lua.h
#   if [ ! $? -eq 0 ]; then
#     exit 15
#   fi
#   mv -fv src/lua.h.patch src/lua.h.patched
#   if [ ! $? -eq 0 ]; then
#     exit 16
#   fi
# fi

# if [ ! -f src/luaconf.h.patched ]; then
#   mv -fv src/luaconf.h src/luaconf.h.patch 2>/dev/null
#   cat src/luaconf.h.patch | sed \
#     "s/MAXSTACK\t\t1000000|MAXSTACK\t\t1000000/g" > src/luaconf.h
#   if [ ! $? -eq 0 ]; then
#     exit 15
#   fi
#   mv -fv src/luaconf.h.patch src/luaconf.h.patched
#   if [ ! $? -eq 0 ]; then
#     exit 16
#   fi
# fi

rm -rfv *.a 2>/dev/null

build()
{
  rm -rfv *.o 2>/dev/null

  g++ -O3 $ARCH$1 -mtune=$2 \
    $EXTRA \
    -DLUA_USE_APICHECK \
    -Wno-deprecated \
    -c src/*.c
  if [ ! $? -eq 0 ]; then
    exit 17;
  fi

  ar rcs "lua64-${1}-${2}.a" *.o
  if [ ! $? -eq 0 ]; then
    exit 18;
  fi
}

PLATFORM=$(uname)
if [ "$PLATFORM" == 'Linux' ]; then
  ARCH="-D"
  EXTRA="-DLUA_USE_LINUX"
  build x86_64 generic
  ar rcs "${LIB}/lua64.la" lua64-*.a
else
  ARCH="-arch "
  EXTRA="-DLUA_USE_MACOSX -mmacosx-version-min=10.15 -stdlib=libc++"
  build arm64 apple-m1
  build x86_64 generic
  lipo lua64-*.a -create -output "${LIB}/lua64.ma"
fi

if [ ! $? -eq 0 ]; then
  exit 19;
fi
