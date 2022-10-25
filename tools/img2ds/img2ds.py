#!/usr/bin/env python3

# SPDX-License-Identifier: MIT
#
# Copyright (c) 2022 Antonio Niño Díaz <antonio_nd@outlook.com>

# This tool depends on pillow: pip3 install pillow (tested with version 9.0.1)

import os

from PIL import Image

from palette import Palette

VALID_TEXTURE_SIZES = [8, 16, 32, 64, 128, 256, 512, 1024]
VALID_FORMATS = ["A1RGB5", "PAL256", "PAL16", "PAL4", "A3PAL32", "A5PAL8",
                 "DEPTHBMP"]


def is_valid_texture_size(size):
    return size in VALID_TEXTURE_SIZES


def save_binary_file(path, byte_list):
    with open(path, "wb") as f:
        f.write(bytearray(byte_list))


def convert_a1rgb5(img):
    print("Converting to A1RGB5:")
    print("- If image alpha == 0 -> Result alpha = 0")
    print("- If image alpha != 0 -> Result alpha = 1")

    texture = []
    palette = []

    rgba = img.convert(mode="RGBA")
    for pixel in list(rgba.getdata()):
        r, g, b, a = pixel

        r = r >> 3
        g = g >> 3
        b = b >> 3
        a = 0 if a == 0 else 1

        v = r | (g << 5) | (b << 10) | (a << 15)

        texture.extend([v & 0xFF, v >> 8])

    return texture, palette


def convert_pal256(img):
    print("Converting to RGB256:")
    print("- If image alpha == 0 -> Color index = 0")
    print("- If image alpha != 0 -> Color index = actual color")
    print("- Use NE_TEXTURE_COLOR0_TRANSPARENT to make color 0 transparent")

    texture = []
    palette = []

    rgba = img.convert(mode="RGBA")

    # Generate palette
    pal = Palette()
    for pixel in list(rgba.getdata()):
        r, g, b, a = pixel
        if a < 255:
            pal.set_color_0_transparent()
        else:
            pal.add_color(r >> 3, g >> 3, b >> 3)

    num_colors = len(pal.get_palette())

    print(f"Number of colors = {num_colors}")

    if num_colors > 256:
        raise Exception(f"Too many colors: {num_colors} > 256")

    for entry in pal.get_palette():
        color = entry[0] | (entry[1] << 5) | (entry[2] << 10)
        palette.extend([color & 0xFF, color >> 8])

    # Generate texture
    for pixel in list(rgba.getdata()):
        r, g, b, a = pixel
        if a < 255:
            texture.append(0)
        else:
            texture.append(pal.get_index(r >> 3, g >> 3, b >> 3))

    return texture, palette


def convert_pal16(img):
    print("Converting to RGB16:")
    print("- If image alpha == 0 -> Color index = 0")
    print("- If image alpha != 0 -> Color index = actual color")
    print("- Use NE_TEXTURE_COLOR0_TRANSPARENT to make color 0 transparent")

    texture = []
    palette = []

    rgba = img.convert(mode="RGBA")

    # Generate palette
    pal = Palette()
    for pixel in list(rgba.getdata()):
        r, g, b, a = pixel
        if a < 255:
            pal.set_color_0_transparent()
        else:
            pal.add_color(r >> 3, g >> 3, b >> 3)

    num_colors = len(pal.get_palette())

    print(f"Number of colors = {num_colors}")

    if num_colors > 16:
        raise Exception(f"Too many colors: {num_colors} > 16")

    for entry in pal.get_palette():
        color = entry[0] | (entry[1] << 5) | (entry[2] << 10)
        palette.extend([color & 0xFF, color >> 8])

    # Generate texture
    temp_texture = []
    for pixel in list(rgba.getdata()):
        r, g, b, a = pixel
        if a < 255:
            temp_texture.append(0)
        else:
            temp_texture.append(pal.get_index(r >> 3, g >> 3, b >> 3))

    for i in range(0, len(temp_texture), 2):
        index0 = temp_texture[i + 0]
        index1 = temp_texture[i + 1]
        v = index0 | (index1 << 4)
        texture.append(v)

    return texture, palette


def convert_pal4(img):
    print("Converting to RGB4:")
    print("- If image alpha == 0 -> Color index = 0")
    print("- If image alpha != 0 -> Color index = actual color")
    print("- Use NE_TEXTURE_COLOR0_TRANSPARENT to make color 0 transparent")

    texture = []
    palette = []

    rgba = img.convert(mode="RGBA")

    # Generate palette
    pal = Palette()
    for pixel in list(rgba.getdata()):
        r, g, b, a = pixel
        if a < 255:
            pal.set_color_0_transparent()
        else:
            pal.add_color(r >> 3, g >> 3, b >> 3)

    num_colors = len(pal.get_palette())

    print(f"Number of colors = {num_colors}")

    if num_colors > 4:
        raise Exception(f"Too many colors: {num_colors} > 4")

    for entry in pal.get_palette():
        color = entry[0] | (entry[1] << 5) | (entry[2] << 10)
        palette.extend([color & 0xFF, color >> 8])

    # Generate texture
    temp_texture = []
    for pixel in list(rgba.getdata()):
        r, g, b, a = pixel
        if a < 255:
            temp_texture.append(0)
        else:
            temp_texture.append(pal.get_index(r >> 3, g >> 3, b >> 3))

    for i in range(0, len(temp_texture), 4):
        index0 = temp_texture[i + 0]
        index1 = temp_texture[i + 1]
        index2 = temp_texture[i + 2]
        index3 = temp_texture[i + 3]
        v = index0 | (index1 << 2) | (index2 << 4) | (index3 << 6)
        texture.append(v)

    return texture, palette


def convert_a3pal32(img):
    print("Converting to A3PAL32")

    texture = []
    palette = []

    rgba = img.convert(mode="RGBA")

    # Generate palette
    pal = Palette()
    for pixel in list(rgba.getdata()):
        r, g, b, a = pixel
        alpha = a >> 5
        if alpha > 0:
            pal.add_color(r >> 3, g >> 3, b >> 3)

    num_colors = len(pal.get_palette())

    print(f"Number of colors = {num_colors}")

    if num_colors > 32:
        raise Exception(f"Too many colors: {num_colors} > 32")

    for entry in pal.get_palette():
        color = entry[0] | (entry[1] << 5) | (entry[2] << 10)
        palette.extend([color & 0xFF, color >> 8])

    # Generate texture
    for pixel in list(rgba.getdata()):
        r, g, b, a = pixel

        alpha = a >> 5
        if alpha == 0:
            # The index doesn't matter if the alpha value is 0
            texture.append(0)
        else:
            index = pal.get_index(r >> 3, g >> 3, b >> 3)
            texture.append(index | (alpha << 5))

    return texture, palette


def convert_a5pal8(img):
    print("Converting to A5PAL8")

    texture = []
    palette = []

    rgba = img.convert(mode="RGBA")

    # Generate palette
    pal = Palette()
    for pixel in list(rgba.getdata()):
        r, g, b, a = pixel
        alpha = a >> 3
        if alpha > 0:
            pal.add_color(r >> 3, g >> 3, b >> 3)

    num_colors = len(pal.get_palette())

    print(f"Number of colors = {num_colors}")

    if num_colors > 8:
        raise Exception(f"Too many colors: {num_colors} > 8")

    for entry in pal.get_palette():
        color = entry[0] | (entry[1] << 5) | (entry[2] << 10)
        palette.extend([color & 0xFF, color >> 8])

    # Generate texture
    for pixel in list(rgba.getdata()):
        r, g, b, a = pixel

        alpha = a >> 3
        if alpha == 0:
            # The index doesn't matter if the alpha value is 0
            texture.append(0)
        else:
            index = pal.get_index(r >> 3, g >> 3, b >> 3)
            texture.append(index | (alpha << 3))

    return texture, palette


def convert_depthbmp(img):
    print("Converting to DEPTHBMP:")
    print("- Depth is calculated like this:")
    print("    if (R > 0) -> Value = 0x00000 (Hides 2D + 3D)")
    print("    else -> Value = 0x67FF + (0x1800 * B / 255) (Gradually hides 3D)")
    print("- If image alpha == 0 -> Fog = 1")
    print("- If image alpha != 0 -> Fog = 0")
    print("- NOTE: This may change in the future")

    texture = []
    palette = []

    rgba = img.convert(mode="RGBA")
    for pixel in list(rgba.getdata()):
        r, g, b, a = pixel

        if r > 0:
            v = 0
        else:
            v = int(0x67FF + (0x1800 * b / 255))

        if a == 0:
            v = v | (1 << 15) # Fog enabled

        texture.extend([v & 0xFF, v >> 8])

    return texture, palette


def convert_img(in_path, out_name, out_folder, out_format):

    if out_format not in VALID_FORMATS:
        raise Exception(f"Unknown format {out_format}. Valid: {VALID_FORMATS}")

    texture_path = os.path.join(out_folder, f"{out_name}_tex.bin")
    palette_path = os.path.join(out_folder, f"{out_name}_pal.bin")

    with Image.open(in_path, "r") as img:
        print(f"Original format: {img.mode}")

        width, height = img.size
        if width not in VALID_TEXTURE_SIZES:
            print(f"WARN: Width {width} is not a valid texture size")
        if height not in VALID_TEXTURE_SIZES:
            print(f"WARN: Height {height} is not a valid texture size")

        if out_format == "A1RGB5":
            texture, palette = convert_a1rgb5(img)
        elif out_format == "DEPTHBMP":
            texture, palette = convert_depthbmp(img)
        elif out_format == "PAL256":
            texture, palette = convert_pal256(img)
        elif out_format == "PAL16":
            texture, palette = convert_pal16(img)
        elif out_format == "PAL4":
            texture, palette = convert_pal4(img)
        elif out_format == "A3PAL32":
            texture, palette = convert_a3pal32(img)
        elif out_format == "A5PAL8":
            texture, palette = convert_a5pal8(img)

    print(f"Saving texture to: {texture_path}")
    save_binary_file(texture_path, texture)

    if len(palette) > 0:
        print(f"Saving palette to: {palette_path}")
        save_binary_file(palette_path, palette)


if __name__ == "__main__":

    import argparse
    import sys
    import traceback

    print("img2ds v0.1.0")
    print("Copyright (c) 2022 Antonio Niño Díaz <antonio_nd@outlook.com>")
    print("All rights reserved")
    print("")

    parser = argparse.ArgumentParser(
            description='Convert PNG files into NDS textures.')

    # Required arguments
    parser.add_argument("--input", required=True,
                        help="input file")
    parser.add_argument("--name", required=True,
                        help="output name: [name]_tex.bin, [name]_pal.bin")
    parser.add_argument("--output", required=True,
                        help="output directory")
    parser.add_argument("--format", required=True, choices=VALID_FORMATS,
                        help="format of the texture")

    args = parser.parse_args()

    try:
        convert_img(args.input, args.name, args.output, args.format)
    except BaseException as e:
        print("ERROR: " + str(e))
        traceback.print_exc()
        sys.exit(1)

    print("Done!")

    sys.exit(0)
