Nitro Engine v0.7.0
===================

Introduction
------------

This is a 3D game engine, a lot of functions designed to simplify the process of
making a 3D game. It isn't standalone, it needs libnds to work. However, if you
are developing for the NDS it is likely that you already have it installed. If
not, you need to install devkitPro with devkitARM and libnds.

Setup
-----

1. Copy this folder (or create a symbolic link) to
   ``<path/to/devkitpro>/nitro-engine`` so that the Makefiles can find the
   library.

2. Go to that folder and run ``make``. This should build the library. If
   everything goes right, you're ready to go.

3. If you want to check that everything is working as expected, open one of the
   folders of the examples and type ``make``. That should build an ``.nds`` file
   that you can run on an emulator or real hardware. Note that some features of
   the 3D hardware aren't emulated by most emulators, so you may need to use an
   actual NDS to test some things.

4. By default, Nitro Engine is compiled with debug options enabled. Go to
   ``NEMain.h`` and comment the line ``#define NE_DEBUG`` to disable them and
   save CPU usage and memory.

Contact
-------

This project is currently hosted on GitHub at:

    https://github.com/AntonioND/nitro-engine

If you want to contact me (Antonio Niño Díaz) directly you can email me at:

   antonio underscore nd at outlook dot com

License
-------

Different parts of this repository are licensed under different licenses.

All the source code of the library and examples are licensed under the MIT
license.

Some tools to convert data are licensed under the MIT license and others are
licensed under the GPL. This shouldn't be a problem, as tools aren't linked with
the source code of the resulting game.

The full text of the licenses can be found under the ``licenses`` folder.

Credits
-------

- Michael Noland (joat), Jason Rogers (dovoto) and Dave Murphy (WinterMute) for
  libnds
- Chishm for libfat and DLDI
- The guys of gbadev for their help
- Martin Korth for no$gba and gbatek
- Kasikiare for NDS Model Exporter
- PadrinatoR for NDS Mesh Converter
- Ti-Ra-Nog for his help testing the engine
