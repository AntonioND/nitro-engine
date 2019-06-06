Just some things you have to know...


1. This program converts any PNG into RGBA 8 bits of depth before converting it
into any other format, so any PNG should work the same. That conversion is done
by libpng.


2. If the image size is not power of 2 the program will output a warning message
but it will convert it anyway. Be careful. If the width is not power of two the
image will need much more time to load (and use as much memory as if it was the
closest power of two). If the height is not a power of two, it will only use the
strictly needed memory, so this is actually advisable.


3. DEPTHBMP is just a depth map used for clear BMP. It is quite complicated to
find the correct depth, so I've done what I can to make it easier to
understand... It may change in the future. Depth is calculated like this:

  IF RED THEN 0x00000 (Hides 2D projection + 3D)
  ELSE 0x67FF + (0x1800 * BLUE / 255) (Gradually hides 3D)

Take a look at the clear_bmp example to see how it works exactly.


4. If the image has alpha channel, it will be used for transparency in this way:

- A1RGB5:

  IF alpha == 0 THEN texture_alpha = 0 ELSE texture_alpha = 1

- RGB256/RGB16/RGB4:

  First, if for any pixel alpha == 0, a dummy color will be put at index 0.
  In that case the next expression is used:

    IF alpha == 0 THEN color_index = 0 ELSE color_index = actual_color_index

  In this way, you can set the color 0  to transparent in texture propierties.

- A3RGB32:

  texture_alpha = alpha >> 5

- A5RGB8:

  texture_alpha = alpha >> 3

- DEPTHBMP:

  IF alpha == 0 THEN fog_enabled = 1 ELSE fog_enabled = 0


5. For paletted textures (RGB256/RGB16/RGB4/A3RGB32/A5RGB8): If the original
image has more colors than allowed, this program will find a palette with less
colors than the original, but the image will lose quality. You should do this in
your image edition program to get the best possible result, but you can use this
tool if you want to.
