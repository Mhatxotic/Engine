#!/bin/sh

# This file is part of the Mhatxotic Engine source repository.
# @ https://github.com/Mhatxotic/Engine
# Copyright (c) Mhatxotic Design, 2006-present. All Rights Reserved.

# Required components
# sudo apt-get install mesa-common-dev libxrandr-dev cmake nasm libjpegturbo0-dev libncurses-dev zlib1g-dev libsqlite3-dev libogg-dev libvorbis-dev libtheora-dev libfreetype-dev libpng-dev libssl-dev libopenal-dev libglfw3-dev

clear

cd ~/Assets/Engine

# do -v to see command line or -### to print it and not compile

# debug add -g and -rdynamic
g++ -v -O3 -fmax-errors=1 -I./include -I./include/ft -std=c++20 -DBETA -o ./bin/build.elf ./src/build.cpp ./lib/lzma64.la ./lib/ssl64.la -lglfw -lz -lrt -lncurses -ldl -lX11 -lpthread

