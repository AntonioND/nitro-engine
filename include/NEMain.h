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

#ifndef __NITRO_ENGINE_H__
#define __NITRO_ENGINE_H__

/*! \file   NEMain.h
 *  \brief  Main file of Nitro Engine.  */

#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------------------------------------------
// If Nitro Engine is compiled with this option, it will check a lot of things and will output  
// error messages to an user-defined function. See the example at Examples/Other/ErrorHandling.
// If you have finished testing your code, just comment the define and recompile Nitro Engine to
// save RAM and CPU usage.
//-----------------------------------------------------------------------------------------------
#define NE_DEBUG

//-----------------------------------------------------------------------------------------------

#include "NEGeneral.h"
#include "NEPolygon.h"
#include "NEFormats.h"
#include "NEFAT.h"
#include "NECamera.h"
#include "NEPalette.h"
#include "NETexture.h"
#include "NEModel.h"
#include "NE2D.h"
#include "NEPhysics.h"
#include "NEAPI.h"
#include "NEText.h"

//-----------------------------------------------------------------------------------------------

#define NITRO_ENGINE_MAJOR (0)
#define NITRO_ENGINE_MINOR (6)
#define NITRO_ENGINE_PATCH (1)

#define NITRO_ENGINE_VERSION ((NITRO_ENGINE_MAJOR<<16) | (NITRO_ENGINE_MINOR<<8) | \
							 (NITRO_ENGINE_PATCH))

#define NITRO_ENGINE_VERSION_STRING "0.6.1"

//-----------------------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif //__NITRO_ENGINE_H__
