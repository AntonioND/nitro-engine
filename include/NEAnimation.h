// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#ifndef NE_ANIMATION_H__
#define NE_ANIMATION_H__

/*! \file   NEAnimation.h
 *  \brief  Functions to load animations.
 */

/*! @defgroup animation_system Animation system
 *
 * System to create and manipulate animations.
 * @{
 */

#define NE_DEFAULT_ANIMATIONS 32	/*! \def #define NE_DEFAULT_ANIMATIONS 32 */

/*! \struct NE_Animation
 *  \brief  Holds information of an animation.
 */
typedef struct {
	bool loadedfromfat;
	bool iscloned;
	const void *data;
} NE_Animation;

/*! \fn    NE_Animation *NE_AnimationCreate(void);
 *  \brief Returns a pointer to a NE_Animation struct.
 */
NE_Animation *NE_AnimationCreate(void);

/*! \fn    void NE_AnimationDelete(NE_Animation *animation);
 *  \brief Deletes an animation struct previously loaded with NE_AnimationCreate.
 *  \param animation Pointer to the animation.
 */
void NE_AnimationDelete(NE_Animation *animation);

/*! \fn    void NE_AnimationClone(NE_Animation *dest, NE_Animation *source);
 *  \brief Clone animation.
 *  \param dest Pointer to the destination animation.
 *  \param source Pointer to the source animation.
 *
 * NOTE: Be careful with this, if you delete source animation and try to use
 * destination animation game will eventually crash.
 *
 * You MUST delete destination animation if you delete source animation.
 *
 * The two animations MUST BE THE SAME TYPE!!! (Animated or static)
 *
 * You should use this if you make a tiled floor, for example, or for making
 * lots of enemies.
 */
void NE_AnimationClone(NE_Animation *dest, NE_Animation *source);

/*! \fn    int NE_AnimationLoad(NE_Animation *animation, const void *pointer);
 *  \brief Loads a DSA file in RAM to an animation. Returns 1 if no error
 *         happened.
 *  \param animation Pointer to the animation.
 *  \param pointer Pointer to the file.
 */
int NE_AnimationLoad(NE_Animation *animation, const void *pointer);

/*! \fn    int NE_AnimationLoadFAT(NE_Animation *animation, const char *path);
 *  \brief Loads a DSA file in FAT to an animation. Returns 1 if no error
 *         happened.
 *  \param animation Pointer to the animation.
 *  \param path Path to the file.
 */
int NE_AnimationLoadFAT(NE_Animation *animation, const char *path);

/*! \fn    void NE_AnimationDeleteAll(void);
 *  \brief Deletes all animations.
 */
void NE_AnimationDeleteAll(void);

/*! \fn    void NE_AnimationSystemReset(int number_of_animations);
 *  \brief Resets the animation system and sets the maximun number of animations.
 *  \param number_of_animations Number of animations. If it is less than 1, it
 *         will create space for NE_DEFAULT_ANIMATIONS.
 */
void NE_AnimationSystemReset(int number_of_animations);

/*! \fn    void NE_AnimationSystemEnd(void);
 *  \brief Ends animation system and all memory used by it.
 */
void NE_AnimationSystemEnd(void);

/*! @} */

#endif // NE_ANIMATION_H__
