// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022 Antonio Niño Díaz <antonio_nd@outlook.com>

// DS Model Animation Library v0.2.0

#ifndef DSMA_H__
#define DSMA_H__

#include <nds.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ARM_CODE
# define ARM_CODE __attribute__((target("arm")))
#endif

// Returns the number of frames stored in the specified DSA file.
uint32_t DSMA_GetNumFrames(const void *dsa_file);

// Draws the model in the DSM file animated with the data in the specified DSA
// file, at the requested frame.
//
// The frame is a fixed point value in 20.12 format (e.g. if you pass 3 << 12 as
// value it corresponds to frame 3). Any value that isn't an exact frame number
// will draw the model by interpolating the two closest frames. It wraps around:
// when going over the max frame, it interpolates with frame 0.
//
// It returns a DSMA_* code (0 for success).
ITCM_CODE ARM_CODE
int DSMA_DrawModel(const void *dsm_file, const void *dsa_file, uint32_t frame_interp);


// Draws the model in the DSM file animated with the data in the specified DSA
// files, at the requested frame, with the requested blending factor between the
// two animations.
//
// The frames are in 20.12 fixed point. It wraps around when going over the max
// frame, it interpolates with frame 0.
//
// The blending factor is in 20.12 fixed point format as well, and it goes from
// 0.0 to display DSA file 1 to 1.0 to display DSA file 2.
//
// It returns a DSMA_* code (0 for success).
ITCM_CODE ARM_CODE
int DSMA_DrawModelBlendAnimation(const void *dsm_file,
        const void *dsa_file_1, uint32_t frame_interp_1,
        const void *dsa_file_2, uint32_t frame_interp_2,
        uint32_t blend);

#define DSMA_SUCCESS                    0
#define DSMA_INVALID_VERSION            -1
#define DSMA_INVALID_FRAME              -2
#define DSMA_INVALID_BLENDING           -3
#define DSMA_MATRIX_STACK_FULL          -4
#define DSMA_INCOMPATIBLE_ANIMATIONS    -5

#ifdef __cplusplus
}
#endif

#endif // DSMA_H__
