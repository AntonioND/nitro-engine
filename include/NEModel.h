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

#ifndef __NE_MODEL_H__
#define __NE_MODEL_H__

/*! \file   NEModel.h
 *  \brief  Functions draw models, etc... */

/*! @defgroup model_system Model system.

	System to create and manipulate animated or static models.
	@{
*/

/*! \def    #define NE_NO_TEXTURE   9999 */
#define NE_NO_TEXTURE   9999

#define NE_DEFAULT_MODELS 1024 /*! \def #define NE_DEFAULT_MODELS 1024 */

/*! \struct NE_Model
 *  \brief  Holds information of a model. */
typedef struct {
	bool meshfromfat;
	bool iscloned;
	bool anim_interpolate; //Use linear interpolation between frames or not (animated models).
	int modeltype; //animated or not
	u32 * meshdata; //display list or NE_AnimData struct
	NE_Material * texture;
	int x, y, z; //f32
	int rx, ry, rz;
	int sx, sy, sz; //f32
} NE_Model;

/*! \enum  NE_ModelType
 *  \brief Possible model types. */
typedef enum {
	NE_Static, /*!< Not animated. */
	NE_Animated /*!< Animated. */
} NE_ModelType;

/*! \fn    NE_Model * NE_ModelCreate(NE_ModelType type);
 *  \brief Returns a pointer to a NE_Model struct. */
NE_Model * NE_ModelCreate(NE_ModelType type);

/*! \fn    void NE_ModelDelete(NE_Model * model);
 *  \brief Deletes a model struct previously loaded with NE_ModelCreate.
 *  \param model Pointer to the model. */
void NE_ModelDelete(NE_Model * model);

/*! \fn    int NE_ModelLoadStaticMesh(NE_Model * model, u32 * pointer);
 *  \brief Assign a display list in RAM to a static model.
 *  \param model Pointer to the model. 
 *  \param pointer Pointer to the display list. */
int NE_ModelLoadStaticMesh(NE_Model * model, u32 * pointer);

/*! \fn    int NE_ModelLoadStaticMeshFAT(NE_Model * model, char * path);
 *  \brief Loads a display list from FAT and assign it to a static model. Returns 1 if OK.
 *  \param model Pointer to the model. 
 *  \param path Path to the display list. */
int NE_ModelLoadStaticMeshFAT(NE_Model * model, char * path);

/*! \fn    void NE_ModelSetMaterial(NE_Model * model, NE_Material * material);
 *  \brief Assign a material to a model.
 *  \param model Pointer to the model. 
 *  \param material Pointer to the material. */
void NE_ModelSetMaterial(NE_Model * model, NE_Material * material);

/*! \fn    void NE_ModelDraw(NE_Model * model);
 *  \brief Draws a model.
 *  \param model Pointer to the model. */
void NE_ModelDraw(NE_Model * model);

/*! \fn    void NE_ModelClone(NE_Model * dest, NE_Model * source);
 *  \brief Clone model.
 *  \param dest Pointer to the destiny model. 
 *  \param source Pointer to the source model.
 
NOTE: Be careful with this, if you delete source model and try to draw destiny model game will eventually crash. 
You MUST delete destiny model if you delete source model.

The two models MUST BE THE SAME TYPE!!! (Animated or static)

You should use this if you make a tiled floor, for example, or for making lots of enemies. */
void NE_ModelClone(NE_Model * dest, NE_Model * source);

/*! \fn    void NE_ModelSetCoordI(NE_Model * model, int x, int y, int z);
 *  \brief Set coordinates of a model.
 *  \param model Pointer to the model.
 *  \param x (X,y,z) Coordinates. 
 *  \param y (X,y,z) Coordinates. 
 *  \param z (X,y,z) Coordinates. */
void NE_ModelSetCoordI(NE_Model * model, int x, int y, int z);

/*! \def   NE_ModelSetCoord(NE_Model * model, float x, float y, float z);
 *  \brief Set coordinates of a model.
 *  \param m Pointer to the model.
 *  \param x (X,y,z) Coordinates. 
 *  \param y (X,y,z) Coordinates. 
 *  \param z (X,y,z) Coordinates. */
#define NE_ModelSetCoord(m, x, y, z)                                        \
		NE_ModelSetCoordI(m, floattof32(x), floattof32(y), floattof32(z))

/*! \fn    void NE_ModelScaleI(NE_Model * model, int x, int y, int z);
 *  \brief Set scale vector of a model.
 *  \param model Pointer to the model.
 *  \param x (X,y,z) Scale vector. 
 *  \param y (X,y,z) Scale vector. 
 *  \param z (X,y,z) Scale vector. */
void NE_ModelScaleI(NE_Model * model, int x, int y, int z);

/*! \def   NE_ModelScale(NE_Model * model, float x, float y, float z);
 *  \brief Set scale vector of a model.
 *  \param m Pointer to the model.
 *  \param x (X,y,z) Scale vector. 
 *  \param y (X,y,z) Scale vector. 
 *  \param z (X,y,z) Scale vector. */
#define NE_ModelScale(m, x, y, z)                                        \
		NE_ModelScaleI(m, floattof32(x), floattof32(y), floattof32(z))

/*! \fn    void NE_ModelTranslateI(NE_Model * model, int x, int y, int z);
 *  \brief Move a model.
 *  \param model Pointer to the model. 
 *  \param x (X,y,z) Translate vector. 
 *  \param y (X,y,z) Translate vector. 
 *  \param z (X,y,z) Translate vector. */
void NE_ModelTranslateI(NE_Model * model, int x, int y, int z);

/*! \def   NE_ModelTranslate(NE_Model * model, float x, float y, float z);
 *  \brief Move a model.
 *  \param m Pointer to the model. 
 *  \param x (X,y,z) Translate vector. 
 *  \param y (X,y,z) Translate vector. 
 *  \param z (X,y,z) Translate vector. */
#define NE_ModelTranslate(m, x, y, z)                                        \
		NE_ModelTranslateI(m, floattof32(x), floattof32(y), floattof32(z))

/*! \fn    void NE_ModelSetRot(NE_Model * model, int rx, int ry, int rz);
 *  \brief Set rotation of a model.
 *  \param model Pointer to the model. 
 *  \param rx Rotation by X axis (0 - 511). 
 *  \param ry Rotation by Y axis (0 - 511). 
 *  \param rz Rotation by Z axis (0 - 511). */
void NE_ModelSetRot(NE_Model * model, int rx, int ry, int rz);

/*! \fn    void NE_ModelRotate(NE_Model * model, int rx, int ry, int rz);
 *  \brief Increases rotation of a model.
 *  \param model Pointer to the model. 
 *  \param rx Rotation by X axis (0 - 511). 
 *  \param ry Rotation by Y axis (0 - 511). 
 *  \param rz Rotation by Z axis (0 - 511). */
void NE_ModelRotate(NE_Model * model, int rx, int ry, int rz);

/*! \fn    void NE_ModelAnimateAll(void);
 *  \brief Needed to update current frame of every model. */
void NE_ModelAnimateAll(void);

/*! \fn    void NE_ModelAnimStart(NE_Model * model, int min, int start, int max, NE_AnimationTypes type, int speed);
 *  \brief Starts the animation of an animated model.
 *  \param model Pointer to the model. 
 *  \param min Lowest frame possible.
 *  \param start Start frame.
 *  \param max Highest frame.
 *  \param type Animation tipe. [ NE_ANIM_LOOP/NE_ANIM_ONESHOT/NE_ANIM_UPDOWN ]
 *  \param speed Animation speed. 0 = stop, 1 = slow, 64 = max speed; */
void NE_ModelAnimStart(NE_Model * model, int min, int start, int max, NE_AnimationTypes type, int speed);

/*! \fn    void NE_ModelAnimSetSpeed(NE_Model * model, int speed);
 *  \brief Sets speed of an animated model. 0 = stop, 1 = slow, 60 = max speed;
 *  \param model Pointer to the model. 
 *  \param speed New speed. */
void NE_ModelAnimSetSpeed(NE_Model * model, int speed);

/*! \fn    void NE_ModelAnimSetFrameSpeed(NE_Model * model, int frame, int speed);
 *  \brief Sets speed of a single frame of an animated model. 0 = stop, 1 = slow, 60 = max speed;
 *  \param model Pointer to the model. 
 *  \param frame Frame.
 *  \param speed New speed. */
void NE_ModelAnimSetFrameSpeed(NE_Model * model, int frame, int speed);

/*! \fn    int NE_ModelAnimGetFrame(NE_Model * model);
 *  \brief Returns current frame of an animated model.
 *  \param model Pointer to the model. */
int NE_ModelAnimGetFrame(NE_Model * model);

/*! \fn    void NE_ModelAnimSetFrame(NE_Model * model, int frame);
 *  \brief Sets current frame of an animated model.
 *  \param model Pointer to the model. 
 *  \param frame Frame to set. */
void NE_ModelAnimSetFrame(NE_Model * model, int frame);

/*! \fn    void NE_ModelAnimInterpolate(NE_Model * model, bool interpolate);
 *  \brief Enables or disables linear interpolation for the drawing of this animated model.
 *  \param model Pointer to the model. 
 *  \param interpolate [true/false] to enable or disable. 
 
If you enable linear interpolation, the animation will be smoother, but it will need more CPU
to be drawn. Disable it if your model has a lot of frames and the animation is quite fast. 
For example, if you change the model frame each vbl this is useless. Default is true. */
void NE_ModelAnimInterpolate(NE_Model * model, bool interpolate);

/*! \fn    int NE_ModelLoadNEA(NE_Model * model, u32 * pointer);
 *  \brief Loads every frame of a NEA file in RAM to an animated model. Returns 1 if no error happened.
 *  \param model Pointer to the model. 
 *  \param pointer Pointer to the file. */
int NE_ModelLoadNEA(NE_Model * model, u32 * pointer);

/*! \fn    int NE_ModelLoadNEAFAT(NE_Model * model, char * path);
 *  \brief Loads every frame of a NEA file in FAT to an animated model. Returns 1 if no error happened.
 *  \param model Pointer to the model. 
 *  \param path Path to the file. */
int NE_ModelLoadNEAFAT(NE_Model * model, char * path);

/*! \fn    void NE_ModelDeleteAll(void);
 *  \brief Deletes all models. */
void NE_ModelDeleteAll(void);

/*! \fn    void NE_ModelSystemReset(int number_of_models);
 *  \brief Resets the model system and sets the maximun number of models.
 *  \param number_of_models Number of models. If it is less than 1, it will create space for NE_DEFAULT_MODELS.  */
void NE_ModelSystemReset(int number_of_models);

/*! \fn    void NE_ModelSystemEnd(void);
 *  \brief Ends model system and all memory used by it. */
void NE_ModelSystemEnd(void);

/*! @} */ //model_system

#endif //__NE_MODEL_H__
