# NekoTextractor

![How it looks](screenshot.png)

[English](README.md) ● [Español](README_ES.md) ● [简体中文](README_SC.md) ● [Русский](README_RU.md) ● [한국어](README_KR.md) ● [ภาษาไทย](README_TH.md) ● [Français](README_FR.md) ● [Italiano](README_IT.md) ● [日本語](README_JP.md) ● [Bahasa Indonesia](README_ID.md) ● [Português](README_PT.md) ● [Deutsch](README_DE.md)

**NekoTextractor** is an open-source x86/x64 video game text hooker for Windows 7+ (and Wine), forked from [Textractor](https://github.com/Artikash/Textractor) by Artikash.

Originally based on [ITHVNR](https://web.archive.org/web/20160202084144/http://www.hongfire.com/forum/showthread.php/438331-ITHVNR-ITH-with-the-VNR-engine).<br>
Watch the [tutorial video](docs/TUTORIAL.md) for a quick rundown on using it.

## NekoTextractor vs Textractor

This fork is maintained as part of the [N.E.K.O.](https://github.com/MomiJiSan/N.E.K.O.) project — a visual novel AI companion. Key improvements over the original:

- **Modern UI**: Redesigned left-right split layout with card-style panels
- **Runtime language switching**: Switch UI language without restarting
- **Enhanced process management**: PID display, path search, duplicate process handling
- **Improved UX**: Quick start guide in Console, better hook search guidance
- **Active maintenance**: Bugs are fixed, issues are responded to

## Download

Official releases of NekoTextractor can be found [here](https://github.com/MomiJiSan/NekoTextractor/releases).

## Features

- Highly extensible and customizable
- Auto hook many game engines (including some not supported by VNR!)
- Hook text using /H "hook" codes (most AGTH codes supported)
- Automatically search for possible hook codes

## Extensions

See the original [Example Extension project](https://github.com/Artikash/ExampleExtension) to see how to build an extension.<br>
See the extensions folder for examples of what extensions can do.

## Contributing

All contributions are appreciated! Please open an issue or pull request on GitHub.

## Compiling

Before compiling NekoTextractor, you need Qt version 5.13 and Visual Studio with CMake support.
Clone with `git clone https://github.com/MomiJiSan/NekoTextractor.git` and initialize submodules with `git submodule update --init`.
You should then be able to just open the source folder in Visual Studio and build.

## Project Architecture

The host injects texthook into the target process and connects to it via 2 pipe files.
texthook waits for the pipe to be connected, then injects a few instructions into any text outputting functions (e.g. TextOut, GetGlyphOutline) that cause their input to be sent through the pipe.<br>
Additional information about hooks is exchanged via shared memory.<br>
The text that the host receives through the pipe is then processed a little before being dispatched back to the GUI.<br>
Finally, the GUI dispatches the text to extensions before displaying it.

## Credits

Original Textractor by [Artikash](https://github.com/Artikash). See [docs/CREDITS.md](docs/CREDITS.md) for full developer credits.
