# SPDX-License-Identifier: MIT
#
# Copyright (c) 2022 Antonio Niño Díaz <antonio_nd@outlook.com>

class Palette():
    def __init__(self):
        self.palette = []
        # Start thinking that color 0 isn't transparent
        self.color_0_transparent = False

    def set_color_0_transparent(self):
        self.color_0_transparent = True

    def add_color(self, r, g, b):
        """r, g and b must be between 0 and 255."""
        c = (r, g, b)
        if c in self.palette:
            return
        self.palette.append(c)

    def get_index(self, r, g, b):
        c = (r, g, b)
        index = self.palette.index(c)
        if self.color_0_transparent:
            index = index + 1
        return index

    def get_palette(self):
        if self.color_0_transparent:
            # Make color 0 magenta for VRAM viewers in emulators
            zero = (31, 0, 31)
            return [zero] + self.palette
        return self.palette
