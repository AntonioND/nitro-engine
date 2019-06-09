// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#ifndef NE_FAT_H__
#define NE_FAT_H__

#include <nds.h>

/*! \file   NEFAT.h
 *  \brief  Used to load data from FAT.
 */

/*! @defgroup fat FAT functions.
 *
 * Functions to load data from FAT, and to take screenshots.
 *
 * @{
*/

/*! \fn    char* NE_FATLoadData(char* filename);
 *  \brief Load a file to RAM. Returns a pointer to the location of the file
 *         that will have to be freed with free().
 *  \param filename Path to the file.
 */
char *NE_FATLoadData(char *filename);

/*! \fn    u32 NE_FATFileSize(char* filename);
 *  \brief Returns size of a file. -1 if error.
 *  \param filename File to check.
 */
size_t NE_FATFileSize(char *filename);

/*! \fn    int NE_ScreenshotBMP(char * filename);
 *  \brief Takes a screenshot of main screen (or both screens in dual 3D mode)
 *         and saves it as BMP. Returns 1 if OK, 0 if error.
 *  \param filename File to save the screenshot.
 */
int NE_ScreenshotBMP(char *filename);

/*! @} */

#endif // NE_FAT_H__
