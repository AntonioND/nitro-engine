// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2009, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include "stdafx.h"
#include <stdlib.h>
#include "palette.h"

typedef struct {
	unsigned char red, green, blue;
} _COLOR_;

_COLOR_ palette_ptr[MAX_PALETTE_COLORS];
int colors_used, max_colors;
bool warning_maxcolors;
bool can_optimize;
int MAX_COLOR_DIFFERENCE;

bool palette_error;

static int Square(int number)
{
	return number * number;
}

void Palette_New(int maxcolors, int optimize)
{
	colors_used = 0;
	warning_maxcolors = false;
	max_colors = maxcolors;
	palette_error = false;

	if (optimize > 0) {
		can_optimize = true;
		MAX_COLOR_DIFFERENCE = optimize;
	}
}

void Palette_NewColor(unsigned char red, unsigned char green,
		      unsigned char blue)
{
	if (palette_error)
		return;

	int a;
	for (a = 0; a < colors_used; a++) {
		// If same color
		if (palette_ptr[a].red == red && palette_ptr[a].green == green
		    && palette_ptr[a].blue == blue)
			return;
	}

	// Find if there is a very similar color...
	if (can_optimize) {
		// Find the most similar color
		for (a = 0; a < colors_used; a++) {
			if (MAX_COLOR_DIFFERENCE >
			    Square(palette_ptr[a].red - red) +
			    Square(palette_ptr[a].green - green) +
			    Square(palette_ptr[a].blue - blue))
				return;	// Found, don't add the new color
		}
	}

	// Add the color
	if (colors_used < max_colors) {
		palette_ptr[colors_used].red = red;
		palette_ptr[colors_used].green = green;
		palette_ptr[colors_used].blue = blue;
		colors_used++;
		return;
	}

	// Couldn't add a color...
	palette_error = true;
}

int Palette_GetIndex(unsigned char red, unsigned char green, unsigned char blue)
{
	int a;
	for (a = 0; a < colors_used; a++) {
		// If same color
		if (palette_ptr[a].red == red && palette_ptr[a].green == green
		    && palette_ptr[a].blue == blue)
			return a;
	}

	if (can_optimize) {
		int difference = 10000000;
		int chosen_color = -1;
		// Not found the same color, find a similar one
		for (a = 0; a < colors_used; a++) {
			int newdifference =
			    Square(palette_ptr[a].red - red) +
			    Square(palette_ptr[a].green - green) +
			    Square(palette_ptr[a].blue - blue);

			if (difference > newdifference) {
				chosen_color = a;
				difference = newdifference;
			}
		}

		return chosen_color;
	}

	return 0;
}

void Palette_GetColor(int index, int *red, int *green, int *blue)
{
	if (index < colors_used) {
		*red = palette_ptr[index].red;
		*green = palette_ptr[index].green;
		*blue = palette_ptr[index].blue;
	} else {
		*red = *green = *blue = 0;
	}
}

int Palette_GetColorsUsed(void)
{
	if (palette_error)
		return -1;

	return colors_used;
}
