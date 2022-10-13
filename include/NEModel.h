// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#ifndef NE_MODEL_H__
#define NE_MODEL_H__

/*! \file   NEModel.h
 *  \brief  Functions to draw and handle models.
 */

/*! @defgroup model_system Model system
 *
 * System to create and manipulate animated or static models.
 * @{
 */

/*! \def    #define NE_NO_TEXTURE   -1 */
#define NE_NO_TEXTURE   -1

#define NE_DEFAULT_MODELS 512	/*! \def #define NE_DEFAULT_MODELS 512 */

/*! \enum  NE_AnimationTypes
 *  \brief Possible animation types.
 */
typedef enum {
	NE_ANIM_LOOP,		/*!< When the end is reached it jumps to the start. */
	NE_ANIM_ONESHOT,	/*!< When the end is reached it stops. */
} NE_AnimationType;

/*! \struct NE_AnimData
 *  \brief  Holds information of the animation of a model.
 */
typedef struct {
	NE_AnimationType type;
	int32_t speed;
	int32_t currframe; // f32
	int32_t numframes; // int
} NE_AnimData;

/*! \struct NE_Model
 *  \brief  Holds information of a model.
 */
typedef struct {
	bool meshfromfat;
	bool iscloned;
	int modeltype;		// Animated or not
	const u32 *meshdata; // Display list / DSM file
	NE_Animation *animation;
	NE_AnimData animdata;
	NE_Material *texture;
	int x, y, z;		// f32
	int rx, ry, rz;
	int sx, sy, sz;		// f32
} NE_Model;

/*! \enum  NE_ModelType
 *  \brief Possible model types.
 */
typedef enum {
	NE_Static,		/*!< Not animated. */
	NE_Animated		/*!< Animated. */
} NE_ModelType;

/*! \fn    NE_Model *NE_ModelCreate(NE_ModelType type);
 *  \brief Returns a pointer to a NE_Model struct.
 */
NE_Model *NE_ModelCreate(NE_ModelType type);

/*! \fn    void NE_ModelDelete(NE_Model *model);
 *  \brief Deletes a model struct previously loaded with NE_ModelCreate.
 *  \param model Pointer to the model.
 */
void NE_ModelDelete(NE_Model *model);

/*! \fn    int NE_ModelLoadStaticMesh(NE_Model *model, const void *pointer);
 *  \brief Assign a display list in RAM to a static model.
 *  \param model Pointer to the model.
 *  \param pointer Pointer to the display list.
 */
int NE_ModelLoadStaticMesh(NE_Model *model, const void *pointer);

/*! \fn    int NE_ModelLoadStaticMeshFAT(NE_Model *model, const char *path);
 *  \brief Loads a display list from FAT and assign it to a static model.
 *         It returns 1 on success.
 *  \param model Pointer to the model.
 *  \param path Path to the display list.
 */
int NE_ModelLoadStaticMeshFAT(NE_Model *model, const char *path);

/*! \fn    void NE_ModelSetMaterial(NE_Model *model, NE_Material *material);
 *  \brief Assign a material to a model.
 *  \param model Pointer to the model.
 *  \param material Pointer to the material.
 */
void NE_ModelSetMaterial(NE_Model *model, NE_Material *material);

/*! \fn    void NE_ModelSetAnimation(NE_Model *model, NE_Animation *anim);
 *  \brief Assign an animation to a model.
 *  \param model Pointer to the model.
 *  \param anim Pointer to the animation.
 */
void NE_ModelSetAnimation(NE_Model *model, NE_Animation *anim);

/*! \fn    void NE_ModelDraw(NE_Model *model);
 *  \brief Draws a model.
 *  \param model Pointer to the model.
 */
void NE_ModelDraw(NE_Model *model);

/*! \fn    void NE_ModelClone(NE_Model *dest, NE_Model *source);
 *  \brief Clone model.
 *  \param dest Pointer to the destination model.
 *  \param source Pointer to the source model.
 *
 * NOTE: Be careful with this, if you delete source model and try to draw
 * destination model game will eventually crash.
 *
 * You MUST delete destination model if you delete source model.
 *
 * The two models MUST BE THE SAME TYPE!!! (Animated or static)
 *
 * You should use this if you make a tiled floor, for example, or for making
 * lots of enemies.
 */
void NE_ModelClone(NE_Model *dest, NE_Model *source);

/*! \fn    void NE_ModelSetCoordI(NE_Model *model, int x, int y, int z);
 *  \brief Set coordinates of a model.
 *  \param model Pointer to the model.
 *  \param x (x, y, z) Coordinates.
 *  \param y (x, y, z) Coordinates.
 *  \param z (x, y, z) Coordinates.
 */
void NE_ModelSetCoordI(NE_Model *model, int x, int y, int z);

/*! \def   NE_ModelSetCoord(NE_Model *model, float x, float y, float z);
 *  \brief Set coordinates of a model.
 *  \param m Pointer to the model.
 *  \param x (x, y, z) Coordinates.
 *  \param y (x, y, z) Coordinates.
 *  \param z (x, y, z) Coordinates.
 */
#define NE_ModelSetCoord(m, x, y, z) \
	NE_ModelSetCoordI(m, floattof32(x), floattof32(y), floattof32(z))

/*! \fn    void NE_ModelScaleI(NE_Model *model, int x, int y, int z);
 *  \brief Set scale vector of a model.
 *  \param model Pointer to the model.
 *  \param x (x, y, z) Scale vector.
 *  \param y (x, y, z) Scale vector.
 *  \param z (x, y, z) Scale vector.
 */
void NE_ModelScaleI(NE_Model *model, int x, int y, int z);

/*! \def   NE_ModelScale(NE_Model *model, float x, float y, float z);
 *  \brief Set scale vector of a model.
 *  \param m Pointer to the model.
 *  \param x (x, y, z) Scale vector.
 *  \param y (x, y, z) Scale vector.
 *  \param z (x, y, z) Scale vector.
 */
#define NE_ModelScale(m, x, y, z) \
	NE_ModelScaleI(m, floattof32(x), floattof32(y), floattof32(z))

/*! \fn    void NE_ModelTranslateI(NE_Model *model, int x, int y, int z);
 *  \brief Move a model.
 *  \param model Pointer to the model.
 *  \param x (x, y, z) Translate vector.
 *  \param y (x, y, z) Translate vector.
 *  \param z (x, y, z) Translate vector.
 */
void NE_ModelTranslateI(NE_Model *model, int x, int y, int z);

/*! \def   NE_ModelTranslate(NE_Model *model, float x, float y, float z);
 *  \brief Move a model.
 *  \param m Pointer to the model.
 *  \param x (x, y, z) Translate vector.
 *  \param y (x, y, z) Translate vector.
 *  \param z (x, y, z) Translate vector.
 */
#define NE_ModelTranslate(m, x, y, z) \
	NE_ModelTranslateI(m, floattof32(x), floattof32(y), floattof32(z))

/*! \fn    void NE_ModelSetRot(NE_Model *model, int rx, int ry, int rz);
 *  \brief Set rotation of a model.
 *  \param model Pointer to the model.
 *  \param rx Rotation by X axis (0 - 511).
 *  \param ry Rotation by Y axis (0 - 511).
 *  \param rz Rotation by Z axis (0 - 511).
 */
void NE_ModelSetRot(NE_Model *model, int rx, int ry, int rz);

/*! \fn    void NE_ModelRotate(NE_Model *model, int rx, int ry, int rz);
 *  \brief Increases rotation of a model.
 *  \param model Pointer to the model.
 *  \param rx Rotation by X axis (0 - 511).
 *  \param ry Rotation by Y axis (0 - 511).
 *  \param rz Rotation by Z axis (0 - 511).
 */
void NE_ModelRotate(NE_Model *model, int rx, int ry, int rz);

/*! \fn    void NE_ModelAnimateAll(void);
 *  \brief Needed to update current frame of every model.
 */
void NE_ModelAnimateAll(void);

/*! \fn    void NE_ModelAnimStart(NE_Model *model, NE_AnimationTypes type,
 *                                int32_t speed);
 *  \brief Starts the animation of an animated model.
 *  \param model Pointer to the model.
 *  \param type Animation tipe. ( NE_ANIM_LOOP / NE_ANIM_ONESHOT )
 *  \brief speed Animation speed in f32 fixed point. It can be positive or
 *         negative. A speed of 0 stops the animation, a speed of 1 << 12 is the
 *         normal speed.
 */
void NE_ModelAnimStart(NE_Model *model, NE_AnimationType type, int32_t speed);

/*! \fn    void NE_ModelAnimSetSpeed(NE_Model *model, int32_t speed);
 *  \brief speed Animation speed in f32 fixed point. It can be positive or
 *         negative. A speed of 0 stops the animation, a speed of 1 << 12 is the
 *         normal speed.
 *  \param model Pointer to the model.
 *  \param speed New speed.
 */
void NE_ModelAnimSetSpeed(NE_Model *model, int32_t speed);

/*! \fn    int32_t NE_ModelAnimGetFrame(NE_Model *model);
 *  \brief Returns current frame of an animated model in f32 format.
 *  \param model Pointer to the model.
 */
int32_t NE_ModelAnimGetFrame(NE_Model *model);

/*! \fn    void NE_ModelAnimSetFrame(NE_Model *model, int32_t frame);
 *  \brief Sets current frame of an animated model in f32 format.
 *  \param model Pointer to the model.
 *  \param frame Frame to set.
 */
void NE_ModelAnimSetFrame(NE_Model *model, int32_t frame);

/*! \fn    int NE_ModelLoadDSM(NE_Model *model, const void *pointer);
 *  \brief Loads every frame of a DSM file in RAM to an animated model. Returns
 *         1 if no error happened.
 *  \param model Pointer to the model.
 *  \param pointer Pointer to the file.
 */
int NE_ModelLoadDSM(NE_Model *model, const void *pointer);

/*! \fn    int NE_ModelLoadDSMFAT(NE_Model *model, const char *path);
 *  \brief Loads every frame of a DSM file in FAT to an animated model. Returns
 *         1 if no error happened.
 *  \param model Pointer to the model.
 *  \param path Path to the file.
 */
int NE_ModelLoadDSMFAT(NE_Model *model, const char *path);

/*! \fn    void NE_ModelDeleteAll(void);
 *  \brief Deletes all models.
 */
void NE_ModelDeleteAll(void);

/*! \fn    void NE_ModelSystemReset(int number_of_models);
 *  \brief Resets the model system and sets the maximun number of models.
 *  \param number_of_models Number of models. If it is less than 1, it will
 *         create space for NE_DEFAULT_MODELS.
 */
void NE_ModelSystemReset(int number_of_models);

/*! \fn    void NE_ModelSystemEnd(void);
 *  \brief Ends model system and all memory used by it.
 */
void NE_ModelSystemEnd(void);

/*! @} */

#endif // NE_MODEL_H__
