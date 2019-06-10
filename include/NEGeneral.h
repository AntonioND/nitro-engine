// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#ifndef NE_GENERAL_H__
#define NE_GENERAL_H__

#include <nds.h>
#include "NEMain.h"

/*! \file   NEGeneral.h
 *  \brief  Init 3D mode and process functions. */

/*! @defgroup general_ General.
 *
 * Functions to control Nitro Engine.
 *
 * @{
 */

/*! \typedef   typedef void (*NE_Voidfunc)(void);
 *  \brief     Void function pointer used in NE_Process and NE_ProcessDual.
 */
typedef void (*NE_Voidfunc)(void);

/*! \enum  NE_AnimationTypes
 *  \brief Possible animation types.
 */
typedef enum {
	NE_ANIM_LOOP,		/*!< When the end is reached jumps to the start. */
	NE_ANIM_ONESHOT,	/*!< When the end is reached stops. */
	NE_ANIM_UPDOWN		/*!< When the end or start are reached, direction is reverted. */
} NE_AnimationTypes;

#define NE_MAX_FRAMES 128	/*! \def #define NE_MAX_FRAMES 128 */

/*! \struct NE_AnimData
 *  \brief  Holds information of an animation.
 */
typedef struct {
	u32 *fileptrtr;		// Pointer to the file/data

	int animtype;
	int currframe;
	int startframe;
	int endframe;
	int speed[NE_MAX_FRAMES];	// Each frame can have different speed
	s8 direction;
	int nextframetime;
} NE_AnimData;

/*! \struct NE_Input
 *  \brief  Holds information of keys and stylus input for internal use.
 */
typedef struct {
	uint32 kdown, kheld, kup;
	touchPosition touch;
} NE_Input;

/*! \fn    void NE_UpdateInput();
 *  \brief Updates input data for internal use. Use this if you are using
 *         NE_GUI or window system. You have to call scanKeys() each frame for
 *         this to work.
 */
void NE_UpdateInput(void);

/*! \fn    void NE_End(void);
 *  \brief Ends Nitro Engine and frees all memory used by it.
 */
void NE_End(void);

/*! \fn    void NE_Init3D(void);
 *  \brief Inits Nitro Engine and 3D mode in one screen.
 */
void NE_Init3D(void);

/*! \fn    void NE_Process(NE_Voidfunc drawscene);
 *  \brief Draws a 3D scene.
 *  \param drawscene Pointer to a void function with the 3D scene.
 */
void NE_Process(NE_Voidfunc drawscene);

/*! \fn    void NE_InitDual3D(void);
 *  \brief Inits Nitro Engine and 3D mode both screens.
 */
void NE_InitDual3D(void);

/*! \fn    void NE_ProcessDual(NE_Voidfunc topscreen, NE_Voidfunc downscreen);
 *  \brief Draws a 3D scene in each screen.
 *  \param topscreen Pointer to a void function with the 3D scene of top screen.
 *  \param downscreen Pointer to a void function with the 3D scene of lower
 *         screen.
 */
void NE_ProcessDual(NE_Voidfunc topscreen, NE_Voidfunc downscreen);

/*! \fn    void NE_InitConsole(void);
 *  \brief Inits libnds' console in  main screen. Works in dual 3D mode. It uses
 *         VRAM_F.
 */
void NE_InitConsole(void);

/*! \fn    void NE_SetConsoleColor(u32 color);
 *  \brief Changes console text color.
 *  \param color New color.
 */
void NE_SetConsoleColor(u32 color);

/*! \fn    void NE_SwapScreens(void);
 *  \brief Swap screens.
 */
void NE_SwapScreens(void);

/*! \fn    void NE_Viewport(int x1, int y1, int x2, int y2);
 *  \brief Sets the part of screen which displays the 3D image and sets a new
 *         projection.
 *  \param x1 (x1, y1) Down-left pixel.
 *  \param y1 (x1, y1) Down-left pixel.
 *  \param x2 (x2, y2) Up-right pixel.
 *  \param y2 (x2, y2) Up-right pixel.
 *
 * In dual 3D mode it is reseted in NE_ProcessDual(), so you have to call it
 * inside your drawing function.
 *
 * After NE_2DViewInit() viewport is set to (0,0,255,191) for a while, in the
 * next NE_Process() it will go back to what you set (in Dual 3D it goes to
 * default values).
 *
 * You can't set another viewport for 2D objects with this function as it
 * changes the projection matrix, use glViewport(). This isn't saved anywhere,
 * you will have to use it after NE_2DViewInit() every frame.
 */
void NE_Viewport(int x1, int y1, int x2, int y2);

/*! \fn    void NE_ClippingPlanesSetI(int znear, int zfar);
 *  \brief Set near and far clipping planes.
 *  \param znear Near plane.
 *  \param zfar Far plane.
 */
void NE_ClippingPlanesSetI(int znear, int zfar);

/*! \def   NE_ClippingPlanesSet(n,f);
 *  \brief Set near and far clipping planes.
 *  \param n Near plane.
 *  \param f Far plane.
 */
#define NE_ClippingPlanesSet(n, f) \
	NE_ClippingPlanesSetI(floattof32(n), floattof32(f))

/*! \fn    void NE_AntialiasEnable(bool value);
 *  \brief Set antialias on/off.
 *  \param value True/false for on/off.
 */
void NE_AntialiasEnable(bool value);

/*! \fn    int NE_GetPolygonCount(void);
 *  \brief Returns polygon RAM count (0 - 2048).
 */
int NE_GetPolygonCount(void);

/*! \fn    int NE_GetVertexCount(void);
 *  \brief Returns vertex RAM count (0 - 6144).
 */
int NE_GetVertexCount(void);

/*! \enum  NE_SpecialEffects
 *  \brief Enum with the possible effects of NE_SpecialEffectSet().
 */
typedef enum {
	NE_NONE = 0,		/*!< None. */
	NE_NOISE = 1,		/*!< Noise. */
	NE_SINE = 2		/*!< Waves. */
} NE_SpecialEffects;

// TODO: Fix workaround
#ifdef REG_BGOFFSETS
#undef REG_BGOFFSETS
#endif
#define REG_BGOFFSETS (*(vu16*)0x4000010)

/*! \fn    void NE_VBLFunc(void)
 *  \brief Internal use, must be called every vblank.
 */
void NE_VBLFunc(void);

/*! \fn    void NE_SpecialEffectPause(bool pause);
 *  \brief Pause/unpause special effect.
 *  \param pause true/false for pause/unpause.
 */
void NE_SpecialEffectPause(bool pause);

/*! \fn    void NE_HBLFunc(void);
 *  \brief Internal use special. Controls screen effects.
 */
void NE_HBLFunc(void);

/*! \fn    void NE_SpecialEffectSet(NE_SpecialEffects effect);
 *  \brief Set special effect to 3D screen(s).
 *  \param effect NE_NOISE/NE_SINE. 0 = Disable.
 */
void NE_SpecialEffectSet(NE_SpecialEffects effect);

/*! \fn    void NE_SpecialEffectNoiseConfig(int value);
 *  \brief Sets value for noise effect.
 *  \param value Must be ((power of 2) - 1). Default is 15 (0xF).
 *
 * Don't use it if the noise effect is paused, the effect won't be updated until
 * it is unpaused.
 */
void NE_SpecialEffectNoiseConfig(int value);

/*! \fn    void NE_SpecialEffectSineConfig(int mult, int shift);
 *  \brief Sets values for sine effect.
 *  \param mult Frecuency. Default is 10.
 *  \param shift Amplitude. Default is 9. If you want a bigger wave, use lower
 *         numbers.
 */
void NE_SpecialEffectSineConfig(int mult, int shift);

/*! \enum  NE_UPDATE_FLAGS
 *  \brief Arguments for NE_WaitForVBL().
 */
typedef enum {
	NE_UPDATE_GUI = 1,		/*!< Updates NE_GUI. */
	NE_UPDATE_ANIMATIONS = 1 << 1,	/*!< Updates animated models. */
	NE_UPDATE_PHYSICS = 1 << 2,	/*!< Updates the physics engine. */
	NE_CAN_SKIP_VBL = 1 << 3	/*!< Allows Nitro Engine to skip the VBL wait if CPU load is greater than 100. */
} NE_UPDATE_FLAGS;

/*! \fn    void NE_WaitForVBL(NE_UPDATE_FLAGS flags);
 *  \brief Updates selected systems and waits for vertical blank.
 *  \param flags Look at NE_UPDATE_FLAGS.
 *
 * For example, NE_WaitForVBL(NE_UPDATE_GUI | NE_UPDATE_ANIMATIONS);
 *
 * NE_CAN_SKIP_VBL allows the function skip the VBL wait if CPU load is greater
 * than 100. You should use this if you don't need to load textures or do
 * anything else during VBL. You have to set NE_HBLFunc() to HBL interrupt
 * function for this to work.
 */
void NE_WaitForVBL(NE_UPDATE_FLAGS flags);

/*! \fn    int NE_GetCPUPercent(void);
 *  \brief Returns CPU usage percent.
 *
 * You need to use NE_WaitForVBL() and set NE_HBLFunc() to HBL interrupt
 * function for this to work.
 */
int NE_GetCPUPercent(void);

/*! \fn    bool NE_GPUIsRendering(void);
 *  \brief Returns false if VCOUNT is between 192 and 213. If not, true.
 *
 * If true, you should't load textures (during the game). If you try to load
 * textures, there is a moment when the GPU can't access that data, so there
 * will be glitches.
 */
bool NE_GPUIsRendering(void);

//------------------------------------------------------------------------------

#ifdef NE_DEBUG

// TODO: Replace sprintf by snprintf

#define NE_AssertMinMax(min, value, max, format...)		\
	do {							\
		if (((min) > (value)) || ((max) < (value))) {	\
			char string[256];			\
			sprintf(string, "%s:%d:",		\
				__func__, __LINE__);		\
			__NE_debugprint(string);		\
			sprintf(string, ##format);		\
			__NE_debugprint(string);		\
			__NE_debugprint("\n");			\
		}						\
	} while (0)

#define NE_AssertPointer(ptr, format...)			\
	do {							\
		if (!(ptr)) {					\
			char string[256];			\
			sprintf(string, "%s:%d:",		\
				__func__, __LINE__);		\
			__NE_debugprint(string);		\
			sprintf(string, ##format);		\
			__NE_debugprint(string);		\
			__NE_debugprint("\n");			\
		}						\
	} while (0)

#define NE_Assert(cond, format...)				\
	do {							\
		if (!(cond)) {					\
			char string[256];			\
			sprintf(string, "%s:%d:",		\
				__func__, __LINE__);		\
			__NE_debugprint(string);		\
			sprintf(string, ##format);		\
			__NE_debugprint(string);		\
			__NE_debugprint("\n");			\
		}						\
	} while (0)

#define NE_DebugPrint(format...)				\
	do {							\
		char string[256];				\
		sprintf(string, "%s:%d:", __func__, __LINE__);	\
		__NE_debugprint(string);			\
		sprintf(string, ##format);			\
		__NE_debugprint(string);			\
		__NE_debugprint("\n");				\
	} while (0)

/*! \fn    void __NE_debugprint(const char * text);
 *  \brief Function used internally by Nitro Engine to report error messages
 *         when NE_DEBUG is defined.
 *  \param text Text to output.
 */
void __NE_debugprint(const char *text);

/*! \fn    void NE_DebugSetHandler(void (*fn)(const char*));
 *  \brief Sets the debug handler where Nitro Engine will send debug
 *         information.
 *  \param fn Handler function.
 */
void NE_DebugSetHandler(void (*fn)(const char *));

/*! \fn    void NE_DebugSetHandlerConsole(void);
 *  \brief Sets libnds's console as debug handler where Nitro Engine will send
 *         debug information.
 */
void NE_DebugSetHandlerConsole(void);

#else // #ifndef NE_DEBUG

#define NE_AssertMinMax(min, value, max, format...)
#define NE_AssertPointer(ptr, format...)
#define NE_Assert(cond, format...)
#define NE_DebugPrint(format...)
#define NE_DebugSetHandler(fn)
#define NE_DebugSetHandlerConsole()

#endif

//------------------------------------------------------------------------------

/*! @} */

/*! @defgroup touch_test Touch test.

 * If you wanted to know if you are touching something, this is for you! These
 * functions perform tests to know if an object is under the stylus coordinates
 * and return its distance from the camera.
 *
 * Note: There is a bug when first two coordinates of viewport are different
 * from (0,0) in gluPickMatrix.
 *
 * Note2: This uses last stylus coordinates even if it has been released. You
 * will have to check by yourself if it is really touching the screen.
 *
 * Note3: It two objects are overlaping the test may fail to diferenciate which
 * of them is closer to the camera.
 *
 * Note4: If you want to draw something without using NE_ModelDraw(), you will
 * have to move the view to the center of your model and use
 * PosTest_Asynch(0, 0, 0).
 *
 * How to use this:
 *
 * 1 - Init a 'touch test mode' with NE_TouchTestStart() to prepare the hardware
 *     to perform this test.  During this mode, polygons are not drawn. Because
 *     of that, you could (and should) use models with less details than
 *     originals (no texture, no normals, less polygons...).
 *
 * 2 - Call NE_TouchTestObject().
 *
 * 3 - Draw the model with NE_ModelDraw().
 *
 * 4 - Call NE_TouchTestResult() to know if it is being touched. If it is
 *     touched, it returns the distance from the camera.
 *
 * 5 - Repeat 2-4 for each model you want to test.
 *
 * 6 - Call NE_TouchTestEnd() to return back to normal. Polygons are drawn on
 *     screen after this.
 *
 * Adapted from the Picking example of libNDS by Gabe Ghearing.
 *
 * @{
 */

/*! \fn    void NE_TouchTestStart(void);
 *  \brief Start a 'touch test mode' and saves current matrices.
 *
 * Polygons drawn after this won't be displayed on screen. It is a good idea to
 * use models with less details than the original to this test.
 *
 * This doesn't work in emulators.
 *
 * gluPickMatrix has a bug that appears when the two first coordinates of
 * viewport are different from (0, 0).
 */
void NE_TouchTestStart(void);

/*! \fn    void NE_TouchTestObject(void);
 *  \brief Starts a test for a model.
 */
void NE_TouchTestObject(void);

/*! \fn    int NE_TouchTestResult(void);
 *  \brief Returns -1 if the model is NOT being touched. If it is touched,
 *         returns the distance from the camera.
 */
int NE_TouchTestResult(void);

/*! \fn    void NE_TouchTestEnd(void);
 *  \brief Ends the 'touch test mode' and sets the matrices that were saved
 *         before. Polygons drawn after this will be displayed on screen.
 */
void NE_TouchTestEnd(void);

/*! @} */

#endif // NE_GENERAL_H__
