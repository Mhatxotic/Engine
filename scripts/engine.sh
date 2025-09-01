#!/bin/bash

# *** Mhatxotic Engine Shell Helper Script.
# *** Copyright © 2006-present, Mhatxotic Design.
# *** All Rights Reserved Worldwide.

# Binary locations
ROOTBIN=/bin
BINCHMOD=$ROOTBIN/chmod
BINECHO=$ROOTBIN/echo
ROOTUSRBIN=/usr/bin
BINREALPATH=$ROOTBIN/realpath
USRBINBASENAME=$ROOTUSRBIN/basename
USRBINDIRNAME=$ROOTUSRBIN/dirname
USRBINREADLINK=$ROOTUSRBIN/readlink
USRBINGDB=$ROOTUSRBIN/gdb
USRBINLEAKS=$ROOTUSRBIN/leaks
USRBINLLDB=lldb
#USRBINLLDB=$ROOTUSRBIN/lldb
USRBINTR=$ROOTUSRBIN/tr
USRBINUNANE=$ROOTUSRBIN/uname
USRBINVALGRIND=$ROOTUSRBIN/valgrind
ENGBIN=../bin

# If this script was symbolically linked?
if [ -L "$0" ]; then
  # Get base path of executable
  BASEDIR="$($USRBINDIRNAME "$0")/"
  if [ ! $? -eq 0 ]; then exit 1; fi
  # Get target of link
  LINKTARGET="$($USRBINREADLINK "$0")"
  if [ ! $? -eq 0 ]; then exit 2; fi
  # Get real directory of scripts
  SCRIPTDIR="$($USRBINDIRNAME "$BASEDIR$LINKTARGET")"
  if [ ! $? -eq 0 ]; then exit 3; fi
  # Get directory where engine executables are
  ENGBINREL="$SCRIPTDIR/$ENGBIN"
  ENGBIN="$($BINREALPATH "$ENGBINREL")"
  if [ ! $? -eq 0 ]; then exit 4; fi
fi

# Check for debug flag
if [ ! -z $1 ]; then
  if [ $1 = "debug" ]; then
    DEBUG=1
    shift
  elif [ $1 = "valgrind" ]; then
    VALGRIND=1
    shift
  elif [ $1 = "leaks" ]; then
    LEAKS=1
    shift
  fi
fi


# Compare result and set appropriate executable extension
# Get unix environment
UNAME=$($USRBINUNANE -s)
case "$UNAME" in
  Linux*)
    if [ ! -z $DEBUG ]; then
      export WAYLAND_DEBUG=1
      PREFIX="$USRBINGDB --args "
    fi
    if [ ! -z $VALGRIND ]; then
      PREFIX="$USRBINVALGRIND -s --leak-check=full --show-leak-kinds=all "
    fi
    EXT=elf
    ;;
  Darwin*)
    if [ ! -z $DEBUG ]; then
      E="settings set target.env-vars DYLD_INSERT_LIBRARIES=/usr/lib/libgmalloc.dylib NSZombieEnabled=YES MallocHelp=YES NSDeallocateZombies=NO MallocCheckHeapEach=1000000 MallocCheckHeapStart=1000000 MallocScribble=YES MallocGuardEdges=YES MallocCheckHeapAbort=1"
#     P="process handle SIGPIPE -s true -p true -n true"
      P=""
      PREFIX="$USRBINLLDB -o \"$E\" -o \"$P\" -- "
      EXEPARAMS=" log_file=-"
    fi
    if [ ! -z $LEAKS ]; then
      PREFIX="$USRBINLEAKS --atExit -- "
      EXEPARAMS=" log_file=-"
    fi
    EXT=mac
    ;;
  *)
    $BINECHO "Environment '$UNAME' is un-supported!"
    exit 4
esac

# Move to correct directory
WORK=`$USRBINDIRNAME "$0"`
cd "$WORK"
if [ ! $? -eq 0 ]; then
  exit 5
fi

# Check calling name
BASE=`$USRBINBASENAME "$0" | $USRBINTR '[:upper:]' '[:lower:]'`
if [ $BASE = "engine.sh" ]; then
  $BINECHO "This script is not meant to be run directly."
  exit 6
elif [ $BASE = "build" ]; then
  EXE=bin/build.$EXT
  EXEPARAMS=""
else
  SUFFIX=`echo $BASE | tail -c 3`
  if [ $SUFFIX = "32" ]; then
    EXE=$ENGBIN/engine32.$EXT
  else
    EXE=$ENGBIN/engine64.$EXT
  fi
  EXEPARAMS="$EXEPARAMS ast_basedir=$PWD sql_db=$BASE"
fi

# Check that the file exists and is executable and try to make it executable
if [ ! -x "$EXE" ]; then
  if [ ! -f "$EXE" ]; then
    $BINECHO "The file '$EXE' does not exist!"
    exit 7
  fi
  $BINECHO "The file '$EXE' is not executable!";
  $BINCHMOD -v 700 "$EXE"
  if [ ! $? -eq 0 ]; then
    $BINECHO "The file '$EXE' could not be made executable!"
    exit 8
  fi
fi

# Go execute the application
/bin/bash -c "$PREFIX$EXE$EXEPARAMS $*"
