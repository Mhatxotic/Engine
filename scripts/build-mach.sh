#!/bin/sh

# This file is part of the Mhatxotic Engine source repository.
# @ https://github.com/Mhatxotic/Engine
# Copyright (c) Mhatxotic Design, 2006-present. All Rights Reserved.

# Set ALPHA, BETA or RELEASE type
TYPE=BETA

# All includes should be in 'include' dir.
INCLUDE="-Iinclude \
         -Iinclude/curses \
         -Iinclude/ft \
         -Isrc"

# Core operating system frameworks
FRAMEWORKS="-framework AudioToolbox \
            -framework AudioUnit \
            -framework Cocoa \
            -framework CoreAudio \
            -framework CoreVideo \
            -framework IOKit \
            -framework OpenGL"

# Make sure libraries are combined with x86_64 and arm64
LIBS="lib/glfw64.ma \
      lib/lzma64.ma \
      lib/nc64.ma \
      lib/ssl64.ma \
      lib/zlib64.ma"

# Colours
C0="\033[1;37m"
C1="\033[1;32m"
C2="\033[1;34m"
C3="\033[1;36m"
C4="\033[1;39m"

# Compilation helper function
compile()
{ # Set switches based on requested type
  if   [ $1 = "ALPHA" ];   then SWITCHES="-g -O0"
  elif [ $1 = "BETA" ];    then SWITCHES="-O2"
  elif [ $1 = "RELEASE" ]; then SWITCHES="-O3"
  else exit 1; fi

  # Build command
  COMMAND="g++ \
           $SWITCHES \
           -D$1 \
           -target $2 \
           -std=gnu++20 \
           -stdlib=libc++ \
           $INCLUDE \
           $FRAMEWORKS \
           $LIBS \
           src/build.cpp \
           -o bin/build$3.mac"

  # Print command
  echo ${C1}Building ${C2}$2${C1} \(${C3}$1${C1}\): ${C4}$COMMAND${C1}...${C0}
  # Execute the compilation
  $COMMAND
  # Add new line
  echo
}

# Clear the screen
printf '\33c\e[3J'
# Set the project directory incase we're not in it
cd ~/Assets/Engine
if [ $? -ne 0 ]; then exit 2; fi
# Compile X86-64 version
compile $TYPE x86_64-apple-macos10.12 32
if [ $? -ne 0 ]; then exit 3; fi
# Compile ARM64 version
compile $TYPE arm64-apple-macos11 64
if [ $? -ne 0 ]; then exit 4; fi

# If both binaries available?
if [ -f bin/build32.mac ] && [ -f bin/build64.mac ]; then
  # Print progress
  echo ${C1}Finalising...${C0}
  # Create the universal binary
  lipo -create -output bin/build.mac bin/build32.mac bin/build64.mac
  # Show binary result
  file bin/build32.mac bin/build64.mac bin/build.mac
  stat bin/build32.mac bin/build64.mac bin/build.mac
  # Remove the old files
  rm bin/build32.mac bin/build64.mac
  # Add new line
  echo
fi

# Print end of script
echo ${C1}Completed\!
