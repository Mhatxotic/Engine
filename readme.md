# Mhatxotic Engine

## [Contents](#contents)…
1. [About](#about)…
2. [Features](#features)…
3. [Using](#using)…
4. [Scripts](#scripts)…
5. [Examples](#examples)…
6. [Credits](#credits)…
7. [Contributing](#contributing)…

## [About](#about)…
Mhatxotic Engine attempts to be a safe, simple and fast [cross-platform](https://en.wikipedia.org/wiki/Cross-platform_software) [2-D](https://en.wikipedia.org/wiki/2D_computer_graphics) [multimedia](https://en.wikipedia.org/wiki/Multimedia) [engine](https://en.wikipedia.org/wiki/Game_engine) written in [C++20](https://en.wikipedia.org/wiki/C%2B%2B20) for [Visual C++](https://en.wikipedia.org/wiki/Microsoft_Visual_C%2B%2B), [Clang](https://en.wikipedia.org/wiki/Clang) and [GNU C++](https://en.wikipedia.org/wiki/GNU_Compiler_Collection) Compilers. This engine brings together many [open-source](https://en.wikipedia.org/wiki/Open_source) [libraries](https://en.wikipedia.org/wiki/Library_(computing)) into one easy-to-use environment controlled by the [LUA interpreter](https://en.wikipedia.org/wiki/Lua_(programming_language)). Right now the engine aims to operate on [Windows XP](https://en.wikipedia.org/wiki/Windows_XP) and [better](https://en.wikipedia.org/wiki/List_of_Microsoft_Windows_versions) (x86 plus [x86-64](https://en.wikipedia.org/wiki/Windows_XP_Professional_x64_Edition)), [MacOS](https://en.wikipedia.org/wiki/MacOS) ([x86-64](https://en.wikipedia.org/wiki/OS_X_Mountain_Lion) plus [Arm64](https://en.wikipedia.org/wiki/MacOS_Big_Sur)) and [Linux](https://en.wikipedia.org/wiki/Linux) ([Ubuntu](https://en.wikipedia.org/wiki/Ubuntu) and [Mint](https://en.wikipedia.org/wiki/Linux_Mint) [x86-64](https://en.wikipedia.org/wiki/X86-64)).

Development is mainly focused by myself on the [MacOS](https://en.wikipedia.org/wiki/MacOS) version using an [M1 Ultra](https://en.wikipedia.org/wiki/Apple_M1) machine with the latest version [operating system](https://en.wikipedia.org/wiki/Operating_system) between 2020 to present. Infrequent development is on the [Linux](https://en.wikipedia.org/wiki/Linux) port with an old [i7 Mid-2011 iMac](https://en.wikipedia.org/wiki/IMac_(Intel-based)) and [AMD Radeon 6970](https://en.wikipedia.org/wiki/Radeon_HD_6000_series) [graphics](https://en.wikipedia.org/wiki/Graphics_card) running the latest [Ubuntu](https://en.wikipedia.org/wiki/Ubuntu) and uses [GcEnx's CrossOver port](https://github.com/Kegworks-App/Kegworks) to maintain both [Windows](https://en.wikipedia.org/wiki/Microsoft_Windows) versions since 2020 and was the main development environment between 2006 and 2020 on various high-end [gaming](https://en.wikipedia.org/wiki/Gaming_computer) and [workstations](https://en.wikipedia.org/wiki/Workstation). The engine is also tested successfully on a very old Dell [XPS M1330](https://en.wikipedia.org/wiki/Dell_XPS) Laptop from 2007 with [NVidia GeForce 8400GS](https://en.wikipedia.org/wiki/GeForce_8_series) graphics running the latest [Mint](https://en.wikipedia.org/wiki/Linux_Mint).

<sup><sub><sup><sub>[↑ Top](#contents)</sub></sup></sub></sup>
## [Features](#features)…
* Scaleable to the limits of your hardware and operating system.
* Use of LUA interpreter with configurability and infinite-loop timeout.
* Full and safe error reporting with extensive use of C++ exceptions.
* Front-end choice of Unix NCurses, Win32 console or OpenGL 2-D interface.
* Asynchronous image format loading (such as PNG, JPG, GIF and DDS).
* Support for transparent frame buffer objects and window transparency.
* Optional global 8-bit colour palette system for retro applications.
* Asynchronous audio format loading (such as WAV, CAF, MP3 and OGG).
* Full support for Theora (OGV) video with basic keying using GLSL.
* OpenAL audio with samples, streams and sources interfaces.
* Asynchronous basic HTTP/HTTPS client and SSL socket functionality.
* Optional encryption of your non-volatile data with cvars system.
* Safe directory, memory and file manipulation functions.
* SqLite database support optimised for speed.
* (De)Compression supporting ZIP, LZMA, RAW and AES encryption.
* Very fast JSon support via RapidJson.
* Better optimised binaries from *semi-amalgamated* source code.
* Less than half-a-second startup time to usable on a modern device.

<sup><sub>[↑ Top](#contents)</sub></sup>
## [Using](#using)…
All the engine needs to run is an `app.cfg` file in the directory, a non-solid 7-zip archive (ending in `.adb` not `.7z`) in the directory or a `.7z` archive appended to the engine executable. This text file contains a list of CVars that configure the engine the way you want. It can be formatted as Windows, MacOS or Unix so it does not matter about the line ending format, but it does have to be UTF-8 (No BoM) formatted which is what all files, filenames and all strings are handled as in the engine. On Windows, Unicode filenames are automatically converted to and from UTF-8.

You also need to set the `app_cflags` cvar properly in the `app.cfg` to specify which subsystems you want to enable. The default value is `0` which means show an error to say you did not set a front-end value. The possible combination of flags you can specify are as follows…

| Flag | Front-end | Purpose | Namespaces |
| --- | --- | --- | --- |
| `0x1` | Yes | Enable Win32 or NCurses Terminal. |
| `0x2` | No | Enable OpenAL subsystem. | `Audio`, `Source`, `Sample` and `Stream`. |
| `0x4` | Yes | Enable OpenGL subsystem. | `Clip`, `Cursor`, `Display`, `Fbo`, `Font`, `Input`, `Palette` and `Texture`. |

Combining certain flags unlocks additional namespaces…

| Flag | Purpose | Namespaces |
| --- | --- | --- |
| `0x6` (`0x2+0x4`) | Enable OpenAL and OpenGL subsystems. | `Video`. |

<sup><sub>[↑ Top](#contents)</sub></sup>
## [Scripts](#scripts)…
The engine looks for the specified file named via the `lua_script` variable which is `main.lua` by default, compiles it, caches it in the database for faster future access, executes it and waits for a termination request either via a console command, operating system or the script itself and will execute continuously based on the `app_tickrate` variable which is by default, every 1/60th of a second (16.6msec) and in OpenGL mode, will execute regardless of hardware limitations as an accumulator is used to skip rendering in an attempt to catchup.

### Writable directory locations…
| System | Location |
| --- | --- |
| Windows | `C:\Users\<UserName>\AppData\Roaming\<AuthorName>\<AppShortName>` |
| MacOS | `/Users/<UserName>/Library/Application Support/<AuthorName>/<AppShortName>` |
| Linux | `/home/<UserName>/.local/<AuthorName>/<AppShortName>` |

The engine does not allow the use of `..` (*parent*), the use of a `/` (*root*) directory nor `X:/` (*Windows drive*) prefix for the accessing of any out of scope assets to maintain a sandbox-like safe environment. The only exception is the start-up database name `sql_db` and the working directory `ast_basedir` which can only be set on the command-line. In Windows, all backslashes (`\`) in pathnames are automatically replaced with forward-slashes (`/`) for cross-platform compatibility as MSVC's standard C library file and Windows API functions support unix forward-slashes natively.

See this [automatically generated document](https://Mhatxotic.github.io/Engine) for a complete rundown of all configuration variables and scripting functions. LUA core reference manual is [here](https://www.lua.org/manual/5.4/).

<sup><sub>[↑ Top](#contents)</sub></sup>
## [Examples](#examples)…
A remake of the classic [Amiga](https://en.wikipedia.org/wiki/Amiga) and [DOS](https://en.wikipedia.org/wiki/DOS) game [Diggers](https://github.com/Mhatxotic/Diggers) was made with this engine and available to play. The contents of the self-contained and portable executable are available in the [diggers](diggers) sub-directory.

[This YouTube video](https://www.youtube.com/watch?v=ccePNvBzsWg) is a preview of a private (quick and messy) script I made that dynamically builds a carousel which takes advantage of framebuffer-objects, True-type fonts, triangle rotations, dynamic texture coordinate manipulations, Vorbis audio and Theora video streams, and Lua's incredibly versatile ability to build event and animation systems.

<sup><sub>[↑ Top](#contents)</sub></sup>
## [Credits](#credits)…
This engine makes use of the following [open-source](https://en.wikipedia.org/wiki/Open_source) and commercially distributable components that are always updated to the latest versions...

| Library | Author | Purpose |
| --- | --- | --- |
| [7-Zip](https://7-zip.org/sdk.html) | [Igor Pavlov](https://en.wikipedia.org/wiki/Igor_Pavlov_(programmer)) | Powerful general data [codec](https://en.wikipedia.org/wiki/Codec). |
| [FreeType](https://github.com/freetype/freetype) | [© The FreeType Project](https://freetype.org/) | Load and render [TTF](https://en.wikipedia.org/wiki/TrueType) [fonts](https://en.wikipedia.org/wiki/Font). |
| [GLFW](https://github.com/glfw/glfw) | [© Marcus Geelnard & Camilla Löwy](https://www.glfw.org/) | Interface to Window, [OpenGL](https://en.wikipedia.org/wiki/OpenGL) and [input](https://en.wikipedia.org/wiki/Input_device). |
| [LibJPEGTurbo](https://github.com/libjpeg-turbo/libjpeg-turbo) | [© IJG](https://www.ijg.org/)/[Contributing authors](https://libjpeg-turbo.org/) | [Codec](https://en.wikipedia.org/wiki/Codec)/[container](https://en.wikipedia.org/wiki/JPEG) for [YCbCr](https://en.wikipedia.org/wiki/YCbCr) still [DV](https://en.wikipedia.org/wiki/Digital_video) data. |
| [LibNSGif](https://github.com/netsurf-browser/libnsgif) | [© Richard Wilson & Sean Fox](https://www.netsurf-browser.org/projects/libnsgif/) | [Codec](https://en.wikipedia.org/wiki/Codec)/[container](https://en.wikipedia.org/wiki/GIF) for motion paletted [RGB](https://en.wikipedia.org/wiki/RGB_color_model) pixel data. |
| [LibPNG](https://github.com/glennrp/libpng) | [© Contributing authors](https://www.libpng.org/pub/png/libpng.html) | [Codec](https://en.wikipedia.org/wiki/Codec)/[container](https://en.wikipedia.org/wiki/PNG) for [RGB](https://en.wikipedia.org/wiki/RGB_color_model) pixel data. |
| [LUA](https://github.com/lua/lua) ([modded](https://github.com/Mhatxotic/Lua)) | © [Lua.org](https://lua.org), [PUC-Rio](http://www.puc-rio.br/english/) | User command [interpreter](https://en.wikipedia.org/wiki/Interpreter_(computing)). |
| [MiniMP3](https://github.com/lieff/minimp3) ([modded](https://github.com/Mhatxotic/MiniMP3)) | Lieff/Lion | [Codec](https://en.wikipedia.org/wiki/Codec)/[Container](https://en.wikipedia.org/wiki/MP3) for legacy [PCM](https://en.wikipedia.org/wiki/Pulse-code_modulation) audio data. |
| [NCurses](https://linux.die.net/man/3/ncurses) | [© Free Software Foundation](https://en.wikipedia.org/wiki/Free_Software_Foundation) | [Linux](https://en.wikipedia.org/wiki/Linux) and [MacOS](https://en.wikipedia.org/wiki/MacOS) [text mode](https://en.wikipedia.org/wiki/Computer_terminal) support. |
| [Ogg](https://github.com/xiph/ogg) | [Xiph.Org](https://xiph.org/ogg/) | [Container](https://en.wikipedia.org/wiki/Ogg) for [Vorbis](https://en.wikipedia.org/wiki/Vorbis) and [Theora](https://en.wikipedia.org/wiki/Theora) data. |
| [OpenALSoft](https://github.com/kcat/openal-soft) | [Chris Robinson](https://www.openal-soft.org/) | [3-D audio API](https://en.wikipedia.org/wiki/OpenAL). |
| [OpenSSL](https://github.com/openssl/openssl) | [OpenSSL Software Foundation](https://www.openssl.org/) | Basic [SSL](https://en.wikipedia.org/wiki/Transport_Layer_Security) networking and (de/en)cryption. |
| [RapidJson](https://github.com/Tencent/rapidjson) | [© THL A29 Ltd., Tencent co. & Milo Yip](https://rapidjson.org/) | Store and access [JSON](https://en.wikipedia.org/wiki/JSON) objects. |
| [SQLite](https://github.com/sqlite/sqlite) | [Contributing authors](https://sqlite.org/) | [Interpreter](https://en.wikipedia.org/wiki/SQLite) to store and access user non-volatile data. |
| [Theora](https://github.com/xiph/theora) | [Xiph.Org](https://xiph.org/theora/) | [Codec](https://en.wikipedia.org/wiki/Codec) for [YCbCr](https://en.wikipedia.org/wiki/YCbCr) motion [DV](https://en.wikipedia.org/wiki/Digital_video) data. |
| [Vorbis](https://github.com/xiph/vorbis) | [Xiph.Org](https://xiph.org/vorbis/) | [Codec](https://en.wikipedia.org/wiki/Codec) for [PCM](https://en.wikipedia.org/wiki/Pulse-code_modulation) audio data. |
| [Z-Lib](https://github.com/madler/zlib) | [© Jean-loup Gailly & Mark Adler](https://www.zlib.net/) | Common general data [codec](https://en.wikipedia.org/wiki/Codec). |

## [Contributing](#contributing)…
Thank you for your interest in contributing to the [Mhatxotic Engine](https://github.com/Mhatxotic/Engine) project. If you decide to contribute code in any way — whether that may be via [issues](https://github.com/Mhatxotic/Engine/issues), [discussions](https://github.com/Mhatxotic/Engine/discussions), [pull requests](https://github.com/Mhatxotic/Engine/pulls) or any other medium then by doing so you accept that you give full and permanent permission for us to use and modify your code as we see fit. As gratitude, your (nick)name or organisation will be credited in the [credits](#credits) section and the [credits 0](https://mhatxotic.github.io/Engine/#cc.credits) console command output inside [all future executables](https://github.com/Mhatxotic/Engine/releases).

<sup><sub>[↑ Top](#contents)</sub></sup>
## [Copyright © 2006-2025 Mhatxotic Design. All Rights Reserved](https://github.com/mhatxotic).
