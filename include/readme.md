# Include files

## Directories required

We have a non-standard layout for includes to help keep compiler output messages short. The following include directories are required in order to compile the engine.

| Directory | Library | Notes |
| --- | --- | --- |
| `7z/` | [7-Zip](https://7-zip.org/sdk.html) | |
| `al/` | [OpenALSoft](https://github.com/kcat/openal-soft) | Linux can use built in headers. al/alc headers edited to not use `dllexport` on Win32. |
| `ft/` | [FreeType](https://github.com/freetype/freetype) | Includes were edited to use `#include <include>` and not `#include "include"`. |
| `gif/` | [LibNSGif](https://github.com/netsurf-browser/libnsgif) | |
| `gl/` | [OpenGL](https://github.com/KhronosGroup/OpenGL-Registry) | `glcorearb.h` was merged with `KHR/khrplatform.h`. |
| `glfw/` | [GLFW](https://github.com/glfw/glfw) | |
| `jpeg/` | [LibJPEGTurbo](https://github.com/libjpeg-turbo/libjpeg-turbo) | Overriding `jconfig.h` to load a separate jconfig for each OS. Linux can also use built in headers. |
| `lua/` | [LUA](https://github.com/lua/lua) | |
| `ncurses/` | [NCurses](https://linux.die.net/man/3/ncurses) | Linux can use built in headers. Win32 also does not use this. |
| `ogg/` | [Ogg](https://github.com/xiph/ogg) | Linux can use built in headers. |
| `ogg/` | [Vorbis](https://github.com/xiph/vorbis) | Linux can use built in headers. |
| `openssl/` | [OpenSSL](https://github.com/openssl/openssl) | |
| `png/` | [LibPNG](https://github.com/glennrp/libpng) | |
| `rapidjson/` | [RapidJson](https://github.com/Tencent/rapidjson) | |
| `sql/` | [SQLite](https://github.com/sqlite/sqlite) | Linux can use built in headers. |
| `swp/` | [SimpleWebP](https://github.com/MikuAuahDark/simplewebp) | |
| `theora/` | [Theora](https://github.com/xiph/theora) | Linux can use built in headers. |
| `zlib/` | [Z-Lib](https://github.com/madler/zlib) | Linux can use built in headers. |

## Copyright © 2006-2026 Mhatxotic Design. All Rights Reserved.
