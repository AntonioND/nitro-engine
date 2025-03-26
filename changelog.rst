Changelog
=========

Version 0.15.3 (2025-03-26)
---------------------------

- The devkitARM makefiles have been removed as they only work with old versions
  of devkitARM, which aren't supported by its maintainers. The code and examples
  of Nitro Engine will probably need changes to work with current devkitARM.
- The GRF files used in an example have been updated, they were built before the
  format was changed.
- The build scripts for assets have been modified to use the value of the
  environment variables ``BLOCKSDS`` and ``BLOCKSDSEXT`` if they are found.
- Build library with debug symbols to help debug applications that use it.

Version 0.15.2 (2025-01-15)
---------------------------

- Fix `ar` binary used to build the library.
- Update documentation.
- Fix build of examples in devkitARM.
- Improve setup instructions.

Version 0.15.1 (2024-12-23)
---------------------------

- Install licenses with the rest of the library.
- Clarify some comments in examples.

Version 0.15.0 (2024-12-01)
---------------------------

- Allow users to specify the transformation matrix of a model manually.
- Fix a devkitARM Makefile in an example.

Version 0.14.0 (2024-09-10)
---------------------------

- Use ``stdint.h`` types instead of libnds types.
- Update BlocksDS makefiles.
- Update libdsf to v0.1.3.
- Fix some memory leaks/use-after-free bugs in the RichText module.

Version 0.13.0 (2024-06-08)
---------------------------

- Define BlocksDS as the main toolchain to use with Nitro Engine.
- Simplify build and installation instructions of the library.
- Update ``md5_2_dsma`` to correctly export the base animation of models.
- Stop using ``NE_RGB5`` in the examples, this format is discouraged.
- Optimize copy of ``NE_RGB5`` textures to VRAM.

Version 0.12.0 (2024-03-30)
---------------------------

- Deprecate ``NE_ShadingEnable()``. The name was misleading. All examples that
  use it have stopped calling it.
- For sprites, add a way to specify texture coordinates inside the material to
  use a small part of the texture instead of the whole texture:
  ``NE_SpriteSetMaterialCanvas()``. This is now used in the sprites example.
- Stop using global variables in most examples. Instead, the rendering functions
  get values through the arguments of ``NE_ProcessArg()`` and
  ``NE_ProcessDualArg()``.
- Don't expect palette objects when loading GRF files if the file doesn't
  contain a palette.
- Allow loading BMFont fonts from RAM, not just nitroFS. Add an example of
  using rich text fonts from RAM.
- Added a function to reset the rich text font system. Also add a function to
  return the size that a specific text string would have if drawn.
- Add shadow volume example.
- Build some functions that do lots of multiplications as ARM to increase
  performance.
- Fix compilation with devkitARM.
- Fix linker invocation for C++ with BlocksDS.
- Update libDSF to version 0.1.2, with some speed improvements.
- Relicensed libDSF under "Zlib OR MIT" to simplify licensing with Nitro Engine.
- Some minor documentation improvements.

Version 0.11.0 (2024-03-02)
---------------------------

- Added a rich text system. This allows the user to draw non-monospaced text,
  and it's based on `BMFont <https://www.angelcode.com/products/bmfont/>`_. An
  example has been added to show how to use it to render text. It can render
  text by using one quad per character, or by rendering to a texture which is
  then drawn as a single quad.
- Fix "properties" typo. This involves renaming some functions, but compatibilty
  definitions have been added.
- Update calls to GRF libnds functions after their prototypes have changed.
- Cleanup setup of dual 3D modes that involve setting up 2D sprites to be used
  as a framebuffer.
- Now the size of a sprite is set to the size of the material it is assigned. It
  can be modified later as usual.

Version 0.10.0 (2024-01-28)
---------------------------

- Create variants of ``NE_Process()`` and ``NE_ProcessDual()`` that can pass an
  argument to the screen draw callbacks.

- Add function to load textures in GRF format (BlocksDS only).

- Add functions to load compressed textures (Texel 4x4 format).

- Add examples to load assets from the filesystem, both in binary and GRF
  function.

- Cleanup code style: Use stdint types, turn some functions ``static inline``...

- Remove unneeded call to ``DC_FlushAll()``.

- ``obj2dl``: Improve error message. Fix vertical coordinates being flipped.

- ``img2ds``: Deprecate it except for generating ``DEPTHBMP`` binaries.

- The makefile for BlocksDS now installs the tools to ``/opt/blockds/external``,
  not only the library and headers.

- Migrate all examples to using ``grit`` instead of ``img2ds``.

Version 0.9.1 (2023-11-12)
--------------------------

- Update Makefiles to build on BlocksDS.

- Fix functions used to load assets from FAT.

Version 0.9.0 (2023-10-19)
--------------------------

- Introduce two new dual 3D modes. They are resilient and they will always show
  the same output on both screens even if the framerate drops. This isn't the
  case with the previous dual 3D mode.

- Fix 2D projection used to display 3D sprites. The Y coordinate didn't work
  correctly for numbers close to 192. This means that an ugly hack to apply an
  offset to the texture coordinates of 2D polygons is no longer needed.

- Fix initialization of the library. Sometimes, depending on the loader, the
  game would start in a different time in the screen rendering cycle. This would
  swap the images of the screens until the framerate dropped when loading
  assets, for example.

- The code that switches between screens in dual 3D mode has been more reliable.
  Nitro Engine now swaps screens after they are actually drawn, not in the
  vertical blanking interrupt handler, when it switched every frame even if no
  new frame had been drawn by the game.

- Switch a lot of assert() in the library into permanent runtime checks. Several
  functions now return error codes instead of not returning any value.

- Use safe DMA copy functions if the libnds of the toolchain provides them (they
  are only available in BlocksDS at the moment).

- The library now supports sending display lists to the GPU in different ways to
  work around a hardware bug in the ARM9 DMA when it is set to GFX FIFO mode.

- Fix debug build of the library.

- Fix build of the NFlib template with devkitPro libraries.

- Update examples and add some more, particularly about comparisons between dual
  3D modes.

Version 0.8.2 (2023-04-20)
--------------------------

- Decouple mesh objects from model objects. This simplifies cloning models.
  Previously it was needed to preserve the original object as long as you wanted
  to use the clones. Now, it can be deleted and Nitro Engine won't free the mesh
  until all clones have been deleted.

- Support vertex color commands in ``obj2dl``. This can't be used at the same
  time as normals.

- Improve examples. A script has been added to convert all assets used by the
  examples. Also, the NFlib example has been updated to work with upstream
  NFlib.

- Support BlocksDS.

- A few minor fixes.

Version 0.8.1 (2022-11-10)
--------------------------

Models and materials:

- Improve support of specular properties of materials and add an example of how
  to use it for metalic objects.

- Fix material cloning:

  - Copy material properties apart from just the texture.

  - Assign palettes to materials instead of textures, so that a single texture
    can have multiple textures. You can load a texture to a material, clone the
    material, and assign a different palette to the cloned material.

- Support loading compressed textures and add an example of how to load them.
  Note that ``img2ds`` doesn't support this format yet. Until that support is
  added, compressed texture support should be considered experimental.

- Add example of how to use NFlib at the same time as Nitro Engine. NFlib is a
  library that has support for 2D graphics, which complements the 3D hardware
  support of Nitro Engine.

Other:

- Rename a few functions for consistency. The old names have been kept for
  compatibility, but they will be removed.

- Added some enumerations to help remember the names to be used as function
  arguments.

- The general-purpose allocator has been improved a lot to support compressed
  textures. This is needed due to the special way to load them to VRAM.
  Extensive tests for the allocator have also been added.

- Many internal changes to simplify the code and remove dependencies on libnds
  functions.

Version 0.8.0 (2022-10-21)
--------------------------

Models and materials:

- Add support for MD5 animated models (thanks to
  https://github.com/AntonioND/dsma-library): Introduce tool ``md5_to_dsma`` to
  convert them to a format that Nitro Engine can use.

- Add support for OBJ static models: Introduce tool obj2dl to convert them to a
  format that Nitro Engine can use.

- Introduce tool ``img2ds`` to convert images in many popular formats (PNG, JPG,
  BMP, etc) to DS textures (PNG is still recommended over other formats, as it
  supports alpha better than other formats).

- Drop support for MD2 models (static or animated).

- Remove NDS Model Exporter, Nitro Texture Converter, md2_to_bin and md2_to_nea.
  The animation system has been refactored (but NEA files don't work anymore, so
  you need to update your code anyway).

General:

- Huge cleanup of code style of the codebase.

- Cleanup of all examples. Add the original assets and textures used in all
  examples to the repository, along scripts to convert them to the formats used
  by Nitro Engine.

- Implement a better way to have debug and release builds of the library.

Notes:

- You can still use textures converted with Nitro Texture Converter or NDS Model
  Exporter, and you can still use any model exported with NDS Model Exporter or
  ``md2_to_bin``. However, support for NEA files has been removed (it had awful
  performance, and it was just a bad way to do things), so any file converted by
  ``md2_to_nea`` won't work anymore.

- The reason to replace most tools is that several people had issues building
  them. All the new tools are written in Python, so they don't need to be
  compiled.

Version 0.7.0 (2019-6-14)
-------------------------

- Pushed to GitHub.

- Major cleanup of code.

- Clarify license.

- Reworked tools to build under Linux and Windows.

Version 0.6.1 (2011-9-1)
------------------------

- Fixed identation in all code. Now it isn't a pain to read it (not as much as
  before, :P). Also, a few warnings fixed (related to libnds new versions).

Version 0.6.0 (2009-6-30)
-------------------------

- The functions used to modify textures and palettes now return a pointer to the
  data so that you can modify them easily.

- Each material can have different properties (amient, diffuse...). You can set
  the default ones, the properties each new material will have, and then you
  can set each material's properties individually.

- New texture and palette allocation system, it is faster and better.
  Defragmenting functions don't work now, but I'll fix them for the next
  version.

- Added a debug system. You can compile Nitro Engine in "debug mode" and it will
  send error messages to the function you want. Once you have finished debugging
  or whatever, just recompile Nitro Engine without debug mode.

- Window system renamed to Sprite system. You can set a rotation and a scale for
  each one.

- The most important thing... The animation system has been improved, and now
  animated models are drawn using linear interpolation (you can disable it,
  anyway).

- As a result, I've modified the converters, so you'll have to convert yout
  animated models again.

Version 0.5.1 (2009-1-28)
-------------------------

- Minor bugfixes.

Version 0.5.0 (2009-1-5)
------------------------

- Text system and camera system optimized. New functions for the camera system.

- ``NE_TextPrintBox()`` and ``NE_TextPrintBoxFree()`` slightly changed. They can
  limit the text drawn to a number of characters set by the coder.

- Some functions made internal. Don't use them unless you know what you are
  doing.

- Fixed (?) at least the 2D projection.

- HBL effects fixed.

- Touch test functions.

- ``NE_UPDATE_INPUT`` removed.

- It now supports any BMP size, and BMP with 4 bits of depth.

- Arrays made pointers, so there is more memory free when you are not using
  Nitro Engine. You can also configure the number of objects of each systems you
  are going to use.

- ``NE_TextPalette`` replaced by ``NE_Palette``.

- You can clone materials to use the same texture with different colors. This
  doesn't have the problems of cloning models.

- Added functions to remove all palettes and textures.

- Fixed ``NE_End()``.

- NE can free all memory used by it, and the coder can tell NE how much memory
  to use.

- Texture drawing system improved a bit.

- ``NE_PolyFormat()`` simplified.

- Some bugfixes, code reorganized, define lists converted into enums.

- Clear bitmap supported, this is used to display an bitmap as rear plane. Each
  pixel can have different depth. This needs 2 VRAM banks to work.

- Solved some problems with 2D system and culling.

- Nomad ``NDS_Texture_Converter`` is no longer included, if you want it, look for it
  in Google.

- Added Nitro Texture Converter, made by me. Open source, and it exports various
  levels of alpha in the textures that can handle it. It does only accept PNG
  files.

- NE now accepts any texture size. ``NE_SIZE_XXX`` defines removed as they are
  not needed now.

- Added a couple of examples.

Version 0.4.2 (2008-12-14)
--------------------------

- Fixed 2D system (textures were displaying wrong on 2D quads) and text system
  (paletted textures sometimes were drawn without palette).

- Modified ``MD2_2_NEA``, ``MD2_2_BIN`` and ``bin2nea`` to work in linux. Thanks
  to nintendork32.

- Added a couple of examples.

Version 0.4.1 (2008-12-12)
--------------------------

- Lots of bugfixes. Specially, UV coordinates swapping fixed.

- Added a function to draw on RGBA textures ^_^.

- Fixed ``MD2_2_NEA`` and ``MD2_2_BIN``. You'll have to convert again your
  models.

- Updated to work with latest libnds. There is a define in case you want to use
  an older version.

Version 0.4.0 (2008-10-15)
--------------------------

- Added ``MD2_2_NEA`` (converts an MD2 model into a NEA file that can used by
  Nitro Engine) and ``MD2_2_BIN`` (Converts the first frame of an MD2 model
  into a display list). Display lists created by them are really optimized.

- Updated ``DisplayList_Fixer``. Now it can remove normal commands too.

- Added a text system. It can use fonts of any size. ^^

- Added some simple API functions (buttons, check boxes, radio buttons and slide
  bars).

- Fixed 2D projection.

- Removed some internal unused functions to save space, and made 'inline' some
  of the rest.

- Functions that used float parameters modified so they use integers now. You
  can still use some wrappers if you want to use floats. This will let the
  compiler try to optimize the code.

- Animated and static models are now the same. You can move, rotate, etc them
  with the same functions.

- Now, you can 'clone' models so you can save a lot of RAM if they are repeated.

- Renamed lots of model functions. Take a look at new examples or documentation.

- ``NE_Color`` struct removed (I don't even know why I created it...).

- Examples updated to work with last version and added examples of clonning
  models, API and text system.

- libnds' console is not inited with Nitro Engine. You will have to init it
  yourself with ``NE_InitConsole()`` or libnds' functions.

Version 0.3.0 (2008-9-16)
-------------------------

- Support for animated models (NEA format) and a program to make a new NEA file
  from many models (in bin format).

- 2D over 3D system. You can draw easily quads (with or without texture) as if
  they were drawn using 2D.

- Basic physics engine (gravity, friction and collitions). It does only support
  bounding boxes for now.

- Added a function to delete all models, animated or not.

- Window system, very simple. I will make some API functions in next versions.

- Nitro Engine compiled as a library to include it easier in projects and save
  space.

- Examples folder organized a bit and added some new examples.

- Nitro Engine is now licensed under the BSD license.

Version 0.2.0 (2008-8-31)
-------------------------

- Added effects like fog and shading, functions to load BMP files and convert
  them in textures and more examples.

Version 0.1 (2008-8-24)
-----------------------

- Includes 2 examples, documentation, tools to export models from the PC, the
  license and full source.
