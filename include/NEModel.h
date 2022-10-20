// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, 2022 Antonio Niño Díaz
//
// This file is part of Nitro Engine

#ifndef NE_MODEL_H__
#define NE_MODEL_H__

/// @file   NEModel.h
/// @brief  Functions to draw and handle models.

/// @defgroup model_system Model handling system
///
/// Functions to create and manipulate animated or static models.
///
/// @{

#define NE_NO_TEXTURE       -1 ///< Value that represents a lack of textures

#define NE_DEFAULT_MODELS   512 ///< Default max number of models

/// Possible animation types.
typedef enum {
    NE_ANIM_LOOP,    ///< When the end is reached it jumps to the start.
    NE_ANIM_ONESHOT, ///< When the end is reached it stops.
} NE_AnimationType;

/// Holds information of the animation of a model.
typedef struct {
    NE_Animation *animation; ///< Pointer to animation file
    NE_AnimationType type;   ///< Animation type.
    int32_t speed;           ///< Animation speed (f32).
    int32_t currframe;       ///< Current frame. It can be between frames (f32).
    int32_t numframes;       ///< Number of frames in the animation (int).
} NE_AnimInfo;

/// Possible model types.
typedef enum {
    NE_Static,  ///< Not animated.
    NE_Animated ///< Animated.
} NE_ModelType;

/// Holds information of a model.
typedef struct {
    bool meshfromfat;         ///< True if the mesh has been loaded from storage
    bool iscloned;            ///< True if the model has been cloned
    NE_ModelType modeltype;   ///< Model type (static or animated)
    const u32 *meshdata;      ///< Display list / DSM file
    NE_AnimInfo *animinfo[2]; ///< Animation information (two can be blended)
    int32_t anim_blend;       ///< Animation blend factor
    NE_Material *texture;     ///< Material used by this model
    int x;                    ///< X position of the model (f32)
    int y;                    ///< Y position of the model (f32)
    int z;                    ///< Z position of the model (f32)
    int rx;                   ///< Rotation of the model by X axis
    int ry;                   ///< Rotation of the model by Y axis
    int rz;                   ///< Rotation of the model by Z axis
    int sx;                   ///< X scale of the model (f32)
    int sy;                   ///< Y scale of the model (f32)
    int sz;                   ///< Z scale of the model (f32)
} NE_Model;

/// Creates a new model object.
///
/// @param type Model type (static or animated).
/// @return Pointer to the newly created camera.
NE_Model *NE_ModelCreate(NE_ModelType type);

/// Deletes a model.
///
/// @param model Pointer to the model.
void NE_ModelDelete(NE_Model *model);

/// Assign a display list in RAM to a static model.
///
/// @param model Pointer to the model.
/// @param pointer Pointer to the display list.
/// @return It returns 1 on success, 0 on error.
int NE_ModelLoadStaticMesh(NE_Model *model, const void *pointer);

/// Loads a display list from a filesystem and assigns it to a static model.
///
/// @param model Pointer to the model.
/// @param path Path to the display list.
/// @return It returns 1 on success, 0 on error.
int NE_ModelLoadStaticMeshFAT(NE_Model *model, const char *path);

/// Assign a material to a model.
///
/// @param model Pointer to the model.
/// @param material Pointer to the material.
void NE_ModelSetMaterial(NE_Model *model, NE_Material *material);

/// Assign an animation to a model.
///
/// @param model Pointer to the model.
/// @param anim Pointer to the animation.
void NE_ModelSetAnimation(NE_Model *model, NE_Animation *anim);

/// Assign a secondary animation to a model.
///
/// The secondary animation is an animation that is averaged with the main
/// animation. This is useful to do transitions between animations. This takes a
/// bit more of CPU to display, though.
///
/// Whenever you want to go back to having one animation, you have to use
/// NE_ModelAnimSecondaryClear(). This function lets you stop blending
/// animations, and it gives you the option to preserve the main or the
/// secondary animation.
///
/// Function NE_ModelAnimSecondarySetFactor() lets you specify a value to
/// specify the blending factor, where 0.0 means "display the main animation
/// only" and 1.0 means "display the secondary animation only". The initial
/// value after calling NE_ModelAnimSecondaryStart() is 0.0.
///
/// @param model Pointer to the model.
/// @param anim Pointer to the animation.
void NE_ModelSetAnimationSecondary(NE_Model *model, NE_Animation *anim);

/// Draw a model.
///
/// @param model Pointer to the model.
void NE_ModelDraw(const NE_Model *model);

/// Clone model.
///
/// Be careful with this, if you delete the source model and try to draw the
/// destination model the game will eventually crash (because the source data is
/// no longer owned by Nitro Engine).
///
/// You must delete the destination model if you delete the source model.
///
/// The two models have to be of the same type (animated or static).
///
/// You could use this if you make a tiled floor, for example, or if you have
/// many enemies that look the same way.
///
/// @param dest Pointer to the destination model.
/// @param source Pointer to the source model.
void NE_ModelClone(NE_Model *dest, NE_Model *source);

/// Set position of a model.
///
/// @param model Pointer to the model.
/// @param x (x, y, z) Coordinates (f32).
/// @param y (x, y, z) Coordinates (f32).
/// @param z (x, y, z) Coordinates (f32).
void NE_ModelSetCoordI(NE_Model *model, int x, int y, int z);

/// Set position of a model.
///
/// @param m Pointer to the model.
/// @param x (x, y, z) Coordinates (float).
/// @param y (x, y, z) Coordinates (float).
/// @param z (x, y, z) Coordinates (float).
#define NE_ModelSetCoord(m, x, y, z) \
    NE_ModelSetCoordI(m, floattof32(x), floattof32(y), floattof32(z))

/// Set scale of a model.
///
/// @param model Pointer to the model.
/// @param x (x, y, z) Scale (f32).
/// @param y (x, y, z) Scale (f32).
/// @param z (x, y, z) Scale (f32).
void NE_ModelScaleI(NE_Model *model, int x, int y, int z);

/// Set scale of a model.
///
/// @param m Pointer to the model.
/// @param x (x, y, z) Scale (float).
/// @param y (x, y, z) Scale (float).
/// @param z (x, y, z) Scale (float).
#define NE_ModelScale(m, x, y, z) \
    NE_ModelScaleI(m, floattof32(x), floattof32(y), floattof32(z))

/// Translate a model.
///
/// @param model Pointer to the model.
/// @param x (x, y, z) Translate vector (f32).
/// @param y (x, y, z) Translate vector (f32).
/// @param z (x, y, z) Translate vector (f32).
void NE_ModelTranslateI(NE_Model *model, int x, int y, int z);

/// Translate a model.
///
/// @param m  Pointer to the model.
/// @param x (x, y, z) Translate vector (float).
/// @param y (x, y, z) Translate vector (float).
/// @param z (x, y, z) Translate vector (float).
#define NE_ModelTranslate(m, x, y, z) \
    NE_ModelTranslateI(m, floattof32(x), floattof32(y), floattof32(z))

/// Set rotation of a model.
///
/// This function sets the rotation of the model to the provided values.
///
/// @param model Pointer to the model.
/// @param rx Rotation by X axis (0 - 511).
/// @param ry Rotation by Y axis (0 - 511).
/// @param rz Rotation by Z axis (0 - 511).
void NE_ModelSetRot(NE_Model *model, int rx, int ry, int rz);

/// Rotate a model.
///
/// This function adds the values to the current rotation of the model.
///
/// @param model Pointer to the model.
/// @param rx Rotation by X axis (0 - 511).
/// @param ry Rotation by Y axis (0 - 511).
/// @param rz Rotation by Z axis (0 - 511).
void NE_ModelRotate(NE_Model *model, int rx, int ry, int rz);

/// Update internal state of the animation of all models.
void NE_ModelAnimateAll(void);

/// Starts the animation of an animated model.
///
/// The speed can be positive or negative. A speed of 0 stops the animation, a
/// speed of 1 << 12 means that the model will advance one model frame per NDS
/// frame. Anything in between will advance less than one model frame per NDS
/// frame.
///
/// @param model Pointer to the model.
/// @param type Animation type (NE_ANIM_LOOP / NE_ANIM_ONESHOT).
/// @param speed Animation speed. (f32)
void NE_ModelAnimStart(NE_Model *model, NE_AnimationType type, int32_t speed);

/// Starts the secondary animation of an animated model.
///
/// @param model Pointer to the model.
/// @param type Animation type (NE_ANIM_LOOP / NE_ANIM_ONESHOT).
/// @param speed Animation speed. (f32)
void NE_ModelAnimSecondaryStart(NE_Model *model, NE_AnimationType type,
                                int32_t speed);

/// Sets animation speed.
///
/// The speed can be positive or negative. A speed of 0 stops the animation, a
/// speed of 1 << 12 means that the model will advance one model frame per NDS
/// frame. Anything in between will advance less than one model frame per NDS
/// frame.
///
/// @param model Pointer to the model.
/// @param speed New speed. (f32)
void NE_ModelAnimSetSpeed(NE_Model *model, int32_t speed);

/// Sets animation speed of the secondary animation.
///
/// @param model Pointer to the model.
/// @param speed New speed. (f32)
void NE_ModelAnimSecondarySetSpeed(NE_Model *model, int32_t speed);

/// Returns the current frame of an animated model.
///
/// @param model Pointer to the model.
/// @return Returns the frame in f32 format.
int32_t NE_ModelAnimGetFrame(const NE_Model *model);

/// Returns the current frame of the secondary animation of an animated model.
///
/// @param model Pointer to the model.
/// @return Returns the frame in f32 format.
int32_t NE_ModelAnimSecondaryGetFrame(const NE_Model *model);

/// Sets the current frame of an animated model.
///
/// @param model Pointer to the model.
/// @param frame Frame to set. (f32)
void NE_ModelAnimSetFrame(NE_Model *model, int32_t frame);

/// Sets the current frame of the secondary animation of an animated model.
///
/// @param model Pointer to the model.
/// @param frame Frame to set. (f32)
void NE_ModelAnimSecondarySetFrame(NE_Model *model, int32_t frame);

/// Sets the current blending factor between animations.
///
/// This is a value between 0.0 and 1.0 where 0.0 means "display the main
/// animation only" and 1.0 means "display the secondary animation only".
///
/// @param model Pointer to the model.
/// @param factor Blending factor to set. (f32)
void NE_ModelAnimSecondarySetFactor(NE_Model *model, int32_t factor);

/// Clears the secondary animation of a model.
///
/// It is possible to replace the main animation by the secondary animation, or
/// to simply remove the secondary animation. In both cases, the secondary
/// animation will be cleared.
///
/// @param model Pointer to the model.
/// @param replace_base_anim Set to true to replace the base animation by the
///                          secondary animation.
void NE_ModelAnimSecondaryClear(NE_Model *model, bool replace_base_anim);

/// Loads a DSM file stored in RAM to a model.
///
/// @param model Pointer to the model.
/// @param pointer Pointer to the file.
/// @return It returns 1 on success, 0 on error.
int NE_ModelLoadDSM(NE_Model *model, const void *pointer);

/// Loads a DSM file stored in a filesystem to a model.
///
/// @param model Pointer to the model.
/// @param path Path to the file.
/// @return It returns 1 on success, 0 on error.
int NE_ModelLoadDSMFAT(NE_Model *model, const char *path);

/// Deletes all models and frees all memory used by them.
void NE_ModelDeleteAll(void);

/// Resets the model system and sets the maximun number of models.
///
/// @param max_models Number of models. If it is lower than 1, it will create
///                   space for NE_DEFAULT_MODELS.
void NE_ModelSystemReset(int max_models);

/// Ends model system and frees all memory used by it.
void NE_ModelSystemEnd(void);

/// @}

#endif // NE_MODEL_H__
