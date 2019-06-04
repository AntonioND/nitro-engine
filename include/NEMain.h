// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#ifndef NITRO_ENGINE_H__
#define NITRO_ENGINE_H__

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

//------------------------------------------------------------------------------
// If Nitro Engine is compiled with this option, it will check a lot of things
// and will output  error messages to an user-defined function. See the example
// at Examples/Other/ErrorHandling.  If you have finished testing your code,
// just comment the define and recompile Nitro Engine to save RAM and CPU usage.
//------------------------------------------------------------------------------
#define NE_DEBUG

//------------------------------------------------------------------------------

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
#include "NEGUI.h"
#include "NEText.h"

//------------------------------------------------------------------------------

#define NITRO_ENGINE_MAJOR (0)
#define NITRO_ENGINE_MINOR (6)
#define NITRO_ENGINE_PATCH (2)

#define NITRO_ENGINE_VERSION ((NITRO_ENGINE_MAJOR << 16) |	\
			      (NITRO_ENGINE_MINOR << 8) |	\
			      (NITRO_ENGINE_PATCH))

#define NITRO_ENGINE_VERSION_STRING "0.6.2"

//------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif // NITRO_ENGINE_H__
