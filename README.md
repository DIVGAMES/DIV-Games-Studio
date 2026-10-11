# README #

DIV Games Studio 2, ported, translated and upgraded.

Released under the GNU General Public License version 3. The full text is `LICENCE.txt`. Bundled libraries (SDL_gfx, miniz, the libretro API header) keep their own permissive licenses. 

[![Build Status](https://travis-ci.org/DIVGAMES/DIV-Games-Studio.svg?branch=master)](https://travis-ci.org/DIVGAMES/DIV-Games-Studio)

homepage: http://div-arena.co.uk (binary downloads, demos, examples, community)

Cross platform IDE, compiler and runtime for

* DOS
* Windows (32 and 64bit cpu)
* OSX
* Android 
* Amazon Fire TV Stick (WIP)
* Amazon Kindle (WIP)
* Ouya Console (WIP)
* Linux (X86 / X84)
* HTML / Javascript (emscripten - runtime export only)
* Raspberry Pi (B+, 2, zero)
* GCW-Zero
* Pandora Console
* GP2X Console
* PSP

Future Possible Platforms:

* Nintendo DS
* Nintendo Wii
* XBOX (XNA)
* IOS (iPhone / iPad)
* Windows Mobile
* PS2
* Dreamcast
* Pyra Handheld


100% compatible with existing DIV1 and DIV2 code including plugin dlls - EVEN MODE8 3D!

## Libretro core

The RetroArch core is `div_libretro`. It runs a DIV2 program from an `exe`, or from a zip of a Windows or Linux build.

```bash
make -f Makefile.libretro
```

That needs zlib. The result is `div_libretro.so`, `.dylib` or `.dll` depending on the host. Load it with `-L` and pass the game zip or exe. The core does not link SDL: `src/libretro/sdl` is a small 1.2-shaped shim over the libretro framebuffer, timer and audio callback. WAV and raw PCM sound effects are mixed at 44100 Hz. Mode 8 (the VPE renderer) is compiled in. Module music is not decoded.

Quick Menu → Controls remaps the pad onto the DIV keys. Port 1 is arrows, Space, Ctrl and the number keys. Port 2 is W A S D.

Desktop builds (Linux, Windows, macOS) are what `.gitlab-ci.yml` asks the libretro buildbot to compile. Android, iOS and consoles are not in that list yet.

`div_libretro.info` is the core-info entry. Its license field is GPLv3, matching `LICENCE.txt`. 
