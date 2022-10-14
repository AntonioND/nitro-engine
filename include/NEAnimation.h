// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#ifndef NE_ANIMATION_H__
#define NE_ANIMATION_H__

/// @file   NEAnimation.h
/// @brief  Functions to load animations.

/// @defgroup animation_system Animation system
///
/// System to create and manipulate animations.
///
/// @{

#define NE_DEFAULT_ANIMATIONS 32 /// Default max number of model animations.

/// Holds information of an animation.
typedef struct {
    bool loadedfromfat; /// True if it was loaded from a filesystem.
    const void *data;   /// Pointer to the animation data (DSA file).
} NE_Animation;

/// Creates a new animation object.
///
/// @return Pointer to the newly created animation.
NE_Animation *NE_AnimationCreate(void);

/// Deletes an animation object.
///
/// @param animation Pointer to the animation.
void NE_AnimationDelete(NE_Animation *animation);

/// Loads a DSA file in RAM to an animation object.
///
/// @param animation Pointer to the animation.
/// @param pointer Pointer to the file.
/// @return It returns 1 on success.
int NE_AnimationLoad(NE_Animation *animation, const void *pointer);

/// Loads a DSA file in FAT to an animation object.
///
/// @param animation Pointer to the animation.
/// @param path Path to the file.
/// @return It returns 1 on success.
int NE_AnimationLoadFAT(NE_Animation *animation, const char *path);

/// Deletes all animations.
void NE_AnimationDeleteAll(void);

/// Resets the animation system and sets the maximun number of animations.
///
/// @param max_animations Number of animations. If it is lower than 1, it
///                       will create space for NE_DEFAULT_ANIMATIONS.
void NE_AnimationSystemReset(int max_animations);

/// Ends animation system and all memory used by it.
void NE_AnimationSystemEnd(void);

/// @}

#endif // NE_ANIMATION_H__
