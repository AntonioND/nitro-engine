/****************************************************************************
*****************************************************************************
****       _   _ _ _               ______             _                  ****
****      | \ | (_) |             |  ____|           (_)                 ****
****      |  \| |_| |_ _ __ ___   | |__   _ __   __ _ _ _ __   ___       ****
****      | . ` | | __| '__/ _ \  |  __| | '_ \ / _` | | '_ \ / _ \      ****
****      | |\  | | |_| | | (_) | | |____| | | | (_| | | | | |  __/      ****
****      |_| \_|_|\__|_|  \___/  |______|_| |_|\__, |_|_| |_|\___|      ****
****                                             __/ |                   ****
****                                            |___/      V 0.6.1       ****
****                                                                     ****
****                     Copyright (C) 2008 - 2011 Antonio Niño Díaz     ****
****                                   All rights reserved.              ****
****                                                                     ****
*****************************************************************************
****************************************************************************/

/****************************************************************************
*                                                                           *
* Nitro Engine V 0.6.1 is licensed under the terms of <readme_license.txt>. *
* If you have any question, email me at <antonio_nd@hotmail.com>.           *
*                                                                           *
****************************************************************************/

#ifndef __NE_FAT_H__
#define __NE_FAT_H__

#include <nds.h>

/*! \file   NEFAT.h
 *  \brief  Used to load data from FAT. */

/*! @defgroup fat FAT functions.

	Functions to load data from FAT, and to take screenshots.
	@{
*/

/*! \fn    char* NE_FATLoadData(char* filename); 
 *  \brief Load a file to RAM. Returns a pointer to the location of the file that will have 
			to be freed with free().
 *  \param filename Path to the file. */
char* NE_FATLoadData(char* filename); 

/*! \fn    u32 NE_FATFileSize(char* filename);
 *  \brief Returns size of a file. -1 if error.
 *  \param filename File to check. */
u32 NE_FATFileSize(char* filename);

/*! \fn    int NE_ScreenshotBMP(char * filename);
 *  \brief Takes a screenshot of main screen (or both screens in dual 3D mode) and saves 
           it as BMP. Returns 1 if OK, 0 if error.
 *  \param filename File to save the screenshot. */
int NE_ScreenshotBMP(char * filename);

/*! @} */ //fat

#endif //__NE_FAT_H__
