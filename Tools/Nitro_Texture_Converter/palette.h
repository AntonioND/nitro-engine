// SPDX-License-Identifier: MIT
//
// Copyright (c) 2009, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#define MAX_PALETTE_COLORS 256 //256 is the max. number of colors for a palette for NDS

void Palette_New(int maxcolors, int optimize);

void Palette_NewColor(unsigned char red, unsigned char green, unsigned char blue);

int Palette_GetIndex(unsigned char red, unsigned char green, unsigned char blue);

void Palette_GetColor(int index, int * red, int * green, int * blue);

int Palette_GetColorsUsed(void);
