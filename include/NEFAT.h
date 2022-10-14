// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#ifndef NE_FAT_H__
#define NE_FAT_H__

#include <nds.h>

/// @file   NEFAT.h
/// @brief  Used to load data from FAT.

/// @defgroup fat FAT functions
///
/// Functions to load data from FAT, and to take screenshots.
///
/// @{

/// Loads a file to RAM from a filesystem.
///
/// @param filename Path to the file.
/// @return Returns a pointer to the location of the file that will have to be
///         freed with free().
char *NE_FATLoadData(const char *filename);

/// Returns size of a file.
///
/// @param filename File to check.
/// @return Returns the file of the size, or -1 on error.
size_t NE_FATFileSize(const char *filename);

/// Takes a screenshot of the 3D screen.
///
/// It takes a screenshot of the 3D screen (or both screens if in dual 3D mode)
/// and saves it as a BMP file.
///
/// @param filename File to save the screenshot.
/// @return Returns 1 on success, 0 on error.
int NE_ScreenshotBMP(const char *filename);

/// @}

#endif // NE_FAT_H__
