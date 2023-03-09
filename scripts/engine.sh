#!/bin/sh

# *** Mhatxotic Engine Shell Helper Script.
# *** Copyright © 2006-present, Mhatxotic Design.
# *** All Rights Reserved Worldwide.

# Binary locations
BIN=/bin
BINCHMOD=$BIN/chmod
BINECHO=$BIN/echo
USRBIN=/usr/bin
USRBINBASENAME=$USRBIN/basename
USRBINDIRNAME=$USRBIN/dirname
USRBINGDB=$USRBIN/gdb
USRBINLLDB=lldb
#USRBINLLDB=$USRBIN/lldb
USRBINTR=$USRBIN/tr
USRBINUNANE=$USRBIN/uname
USRBINVALGRIND=$USRBIN/valgrind

# Check for debug flag
if [ ! -z $1 ]; then
  if [ $1 = "debug" ]; then
    DEBUG=1
    shift
  elif [ $1 = "valgrind" ]; then
    VALGRIND=1
    shift
  fi
fi

# Compare result and set appropriate executable extension
# Get unix environment
UNAME=$($USRBINUNANE -s)
case "$UNAME" in
  Linux*)
    if [ ! -z $DEBUG ]; then
      PREFIX="$USRBINGDB --args "
    fi
    if [ ! -z $VALGRIND ]; then
      PREFIX="$USRBINVALGRIND -s --leak-check=full --show-leak-kinds=all "
    fi
    EXT=elf
    ;;
  Darwin*)
    if [ ! -z $DEBUG ]; then
#        export DYLD_INSERT_LIBRARIES=/usr/lib/libgmalloc.dylib
#        export NSZombieEnabled=YES
#        export MallocHelp=YES
#        export NSDeallocateZombies=NO
#        export MallocCheckHeapEach=1000000
#        export MallocCheckHeapStart=1000000
#        export MallocScribble=YES
#        export MallocGuardEdges=YES
#        export MallocCheckHeapAbort=1
      PREFIX="$USRBINLLDB -- "
    fi
    EXT=mac
    ;;
  *)
    $BINECHO "Environment '$UNAME' is un-supported!"
    exit 1
esac

# Move to correct directory
WORK=`$USRBINDIRNAME "$0"`
cd "$WORK"
if [ ! $? -eq 0 ]; then
  exit 3
fi

# Check calling name
BASE=`$USRBINBASENAME "$0" | $USRBINTR '[:upper:]' '[:lower:]'`
if [ $BASE = "engine.sh" ]; then
  $BINECHO "This script is not meant to be run directly."
  exit 2
elif [ $BASE = "build" ]; then
  EXE=bin/build.$EXT
  EXEPARAMS=""
else
  SUFFIX=`echo $BASE | tail -c 3`
  if [ $SUFFIX = "32" ]; then
    EXE=../bin/engine32.$EXT
  else
    EXE=../bin/engine64.$EXT
  fi
  EXEPARAMS=" ast_basedir=$PWD sql_db=$BASE"
fi

# Check that the file exists and is executable and try to make it executable
if [ ! -x "$EXE" ]; then
  if [ ! -f "$EXE" ]; then
    $BINECHO "The file '$EXE' does not exist!"
    exit 4
  fi
  $BINECHO "The file '$EXE' is not executable!";
  $BINCHMOD -v 700 "$EXE"
  if [ ! $? -eq 0 ]; then
    $BINECHO "The file '$EXE' could not be made executable!"
    exit 5
  fi
fi

# Go execute the application
$PREFIX$EXE$EXEPARAMS $*
