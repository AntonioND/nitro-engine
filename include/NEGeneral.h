// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, 2022 Antonio Niño Díaz
//
// This file is part of Nitro Engine

#ifndef NE_GENERAL_H__
#define NE_GENERAL_H__

#include <nds.h>

#include "NEMain.h"

/// @file   NEGeneral.h
/// @brief  Init 3D mode and process functions.

/// @defgroup general_utils General utilities
///
/// General functions to control Nitro Engine, setup screens, print debug
/// messages, etc.
///
/// If Nitro Engine is compiled with debug features, it will check a lot of
/// things and it will print error messages to an user-defined function. Check
/// the error handling example for more details. If you have finished testing
/// your code, just comment the define out and recompile Nitro Engine to save
/// RAM and CPU usage.
///
/// @{

/// Void function pointer used in NE_Process and NE_ProcessDual.
typedef void (*NE_Voidfunc)(void);

/// Holds information of keys and stylus input for internal use.
typedef struct {
    uint32 kdown;           ///< Keys that have just been pressed this frame.
    uint32 kheld;           ///< Keys that are pressed.
    uint32 kup;             ///< Keys that have just been released this frame.
    touchPosition touch;    ///< Touch screen state
} NE_Input;

/// Updates input data for internal use.
///
/// Use this if you are using NE_GUI or window system. You have to call
/// scanKeys() each frame for this to work.
void NE_UpdateInput(void);

///< List of all the possible initialization states of Nitro Engine.
typedef enum {
    NE_ModeUninitialized    = 0, ///< Nitro Engine hasn't been initialized.
    NE_ModeSingle3D         = 1, ///< Initialized in single 3D mode.
    NE_ModeDual3D           = 2, ///< Initialized in regular dual 3D mode.
    NE_ModeDual3D_FB        = 3, ///< Initialized in dual 3D FB mode (no debug console).
    NE_ModeDual3D_DMA       = 4  ///< Initialized in safe dual 3D mode.
} NE_ExecutionModes;

/// Returns the current execution mode.
///
/// @return Returns the execution mode.
NE_ExecutionModes NE_CurrentExecutionMode(void);

/// Ends Nitro Engine and frees all memory used by it.
void NE_End(void);

/// Inits Nitro Engine and 3D mode in one screen.
///
/// @return Returns 0 on success.
int NE_Init3D(void);

/// Draws a 3D scene.
///
/// @param drawscene Function that draws the screen.
void NE_Process(NE_Voidfunc drawscene);

/// Inits Nitro Engine to draw 3D to both screens.
///
/// VRAM banks C and D are used as framebuffers, which means there is only 50%
/// of the normally available VRAM for textures.
///
/// This way to display 3D is unsafe. If the framerate of the game drops below
/// 60 FPS the result will be having the same 3D output displayed on both
/// screens during the frames when it doesn't have time to draw anything new.
///
/// However, this mode allows you to use DMA in GFX FIFO mode to draw models,
/// which is more efficient.
///
/// In general, prefer NE_InitDual3D_DMA() over NE_InitDual3D().
///
/// @return Returns 0 on success.
int NE_InitDual3D(void);

/// Inits Nitro Engine to draw 3D to both screens.
///
/// VRAM banks C and D are used as framebuffers, which means there is only 50%
/// of the normally available VRAM for textures.
///
/// Direct VRAM display mode is used for the main engine, which means there is
/// no way to display the debug console on either screen in a stable way, so the
/// debug console is not supported in this mode.
///
/// This mode is stable. If the framerate drops below 60 FPS the screens will
/// remain stable.
///
/// @return Returns 0 on success.
int NE_InitDual3D_FB(void);

/// Inits Nitro Engine to draw 3D to both screens.
///
/// VRAM banks C and D are used as framebuffers, which means there is only 50%
/// of the normally available VRAM for textures.
///
/// VRAM bank I is used as a pseudo-framebuffer, so that there are three
/// framebuffers in total, which is required to make the output of both screens
/// stable if framerate drops below 60 FPS.
///
/// DMA 2 is used in HBL trigger mode to transfer data from VRAM banks C and D
/// to bank I. Because of this, it is unsafe to use the DMA In GFX FIFO mode to
/// draw models, which has a small performance hit.
///
/// This mode is stable. If the framerate drops below 60 FPS the screens will
/// remain stable.
///
/// In general, prefer NE_InitDual3D_DMA() over NE_InitDual3D().
///
/// @return Returns 0 on success.
int NE_InitDual3D_DMA(void);

/// Draws 3D scenes in both screens.
///
/// By default, the main screen is the top screen and the sub screen is the
/// bottom screen. This can be changed with NE_MainScreenSetOnTop(),
/// NE_MainScreenSetOnBottom() and NE_SwapScreens(). To check the current
/// position of the main screen, use NE_MainScreenIsOnTop().
///
/// @param mainscreen Function that draws the main screen.
/// @param subscreen Function that draws the sub screen.
void NE_ProcessDual(NE_Voidfunc mainscreen, NE_Voidfunc subscreen);

/// Inits the console of libnds in the main screen.
///
/// It works in dual 3D mode as well, and it uses VRAM_F for the background
/// layer that contains the text.
///
/// Important note: When using safe dual 3D mode, use NE_InitConsoleSafeDual3D()
/// instead.
void NE_InitConsole(void);

/// Changes the color of the text of the console.
///
/// @param color New color.
void NE_SetConsoleColor(u32 color);

/// Set the top screen as main screen.
void NE_MainScreenSetOnTop(void);

/// Set the bottom screen as main screen.
void NE_MainScreenSetOnBottom(void);

/// Returns the current main screen.
///
/// @return Returns 1 if the top screen is the main screen, 0 otherwise.
int NE_MainScreenIsOnTop(void);

/// Swap top and bottom screen.
void NE_SwapScreens(void);

/// Setup the viewport.
///
/// The viewport is the part of the screen where the 3D scene is drawn.
///
/// NE_ProcessDual() sets the viewport is set to the full screen.
///
/// After NE_2DViewInit() the viewport is set to (0, 0, 255, 191).
///
/// NE_Process() sets the viewport back to the viewport set by NE_Viewport().
///
/// @param x1 (x1, y1) Bottom left pixel.
/// @param y1 (x1, y1) Bottom left pixel.
/// @param x2 (x2, y2) Top right pixel.
/// @param y2 (x2, y2) Top right pixel.
void NE_Viewport(int x1, int y1, int x2, int y2);

///  Set cameras field of view.
///
///  @param fovValue FOV [0, ...] in degrees.
void NE_SetFov(int fovValue);

/// Set near and far clipping planes.
///
/// @param znear Near plane (f32).
/// @param zfar Far plane (f32).
void NE_ClippingPlanesSetI(int znear, int zfar);

/// Set near and far clipping planes.
///
/// @param n Near plane (float).
/// @param f Far plane (float).
#define NE_ClippingPlanesSet(n, f) \
    NE_ClippingPlanesSetI(floattof32(n), floattof32(f))

/// Enable or disable antialiasing.
///
/// @param value true enables antialiasing, false disables it.
void NE_AntialiasEnable(bool value);

/// Returns the number of polygons drawn since the last glFlush().
///
/// @return Returns the number of polygons (0 - 2048).
int NE_GetPolygonCount(void);

/// Returns the number of vertices drawn since the last glFlush().
///
/// @return Returns the number of vertices (0 - 6144).
int NE_GetVertexCount(void);

/// Effects supported by NE_SpecialEffectSet().
typedef enum {
    NE_NONE,  ///< Disable effects
    NE_NOISE, ///< Horizontal noise
    NE_SINE   ///< Horizontal sine waves
} NE_SpecialEffects;

/// Vertical blank interrupt handler.
///
/// Internal use, must be called every vertical blank.
void NE_VBLFunc(void);

/// Pause or unpause special effects.
///
/// @param pause true pauses the effect, false unpauses it.
void NE_SpecialEffectPause(bool pause);

/// Horizontal blank interrupt handler.
///
/// Internal use, must be called every horizontal blank. It's only needed to
/// enable CPU usage measuring and things like special effects enabled by
/// NE_SpecialEffectSet().
void NE_HBLFunc(void);

/// Specify which special effect to display in the 3D screens.
///
/// Note: In safe dual 3D mode, DesMuME doesn't emulate the effects correctly.
/// It doesn't seem to emulate capturing the 3D output scrolled by a horizontal
/// offset.
///
/// @param effect One effect out of NE_NOISE, NE_SINE or NE_NONE.
void NE_SpecialEffectSet(NE_SpecialEffects effect);

/// Configures the special effect NE_NOISE.
///
/// If the effect is paused, the values won't be refreshed until it is unpaused.
///
/// @param value The value must be a power of two minus one. The default is 15.
void NE_SpecialEffectNoiseConfig(int value);

/// Configures the special effect NE_SINE.
///
/// @param mult Frecuency of the wave. The default value is 10.
/// @param shift Amplitude of the wave. The default value is 9. Bigger values
///              result in smaller waves.
void NE_SpecialEffectSineConfig(int mult, int shift);

/// Arguments for NE_WaitForVBL().
typedef enum {
    /// Update the GUI implemented by Nitro Engine.
    NE_UPDATE_GUI = BIT(0),
    /// Update all animated models.
    NE_UPDATE_ANIMATIONS = BIT(1),
    /// Updates the physics engine.
    NE_UPDATE_PHYSICS = BIT(2),
    /// Allows Nitro Engine to skip the wait to the vertical blank if CPU load
    /// is greater than 100%. You can use this if you don't need to load
    /// textures or do anything else during the VBL. It is needed to set
    /// NE_HBLFunc() as a HBL interrupt handler for this flag to work.
    NE_CAN_SKIP_VBL = BIT(3)
} NE_UpdateFlags;

/// Waits for the vertical blank and updates the selected systems.
///
/// You should OR all the flags that you need. For example, you can call this
/// function like NE_WaitForVBL(NE_UPDATE_GUI | NE_UPDATE_ANIMATIONS);
///
/// @param flags Look at NE_UpdateFlags.
void NE_WaitForVBL(NE_UpdateFlags flags);

/// Returns the approximate CPU usage in the previous frame.
///
/// You need to set NE_WaitForVBL() as a VBL interrupt handler and NE_HBLFunc()
/// as a HBL interrupt handler for the CPU meter to work.
///
/// @return CPU usage (0 - 100).
int NE_GetCPUPercent(void);

/// Returns true if the GPU is in a rendering period.
///
/// The period when the GPU isn't drawing is when VCOUNT is between 192 and 213.
///
/// During the drawing period you should't load textures. If you try to load
/// textures, there is a moment when the GPU can't access that data, so there
/// will be glitches in the 3D output.
///
/// @return Returns true if the GPU is in the rendering period.
bool NE_GPUIsRendering(void);

#ifdef NE_DEBUG

// TODO: Replace sprintf by snprintf

/// TODO: Document
#define NE_AssertMinMax(min, value, max, format...)         \
    do                                                      \
    {                                                       \
        if (((min) > (value)) || ((max) < (value)))         \
        {                                                   \
            char string[256];                               \
            sprintf(string, "%s:%d:", __func__, __LINE__);  \
            __NE_debugprint(string);                        \
            sprintf(string, ##format);                      \
            __NE_debugprint(string);                        \
            __NE_debugprint("\n");                          \
        }                                                   \
    } while (0)

/// TODO: Document
#define NE_AssertPointer(ptr, format...)                    \
    do                                                      \
    {                                                       \
        if (!(ptr))                                         \
        {                                                   \
            char string[256];                               \
            sprintf(string, "%s:%d:",  __func__, __LINE__); \
            __NE_debugprint(string);                        \
            sprintf(string, ##format);                      \
            __NE_debugprint(string);                        \
            __NE_debugprint("\n");                          \
        }                                                   \
    } while (0)

/// TODO: Document
#define NE_Assert(cond, format...)                          \
    do                                                      \
    {                                                       \
        if (!(cond))                                        \
        {                                                   \
            char string[256];                               \
            sprintf(string, "%s:%d:", __func__, __LINE__);  \
            __NE_debugprint(string);                        \
            sprintf(string, ##format);                      \
            __NE_debugprint(string);                        \
            __NE_debugprint("\n");                          \
        }                                                   \
    } while (0)

/// TODO: Document
#define NE_DebugPrint(format...)                            \
    do                                                      \
    {                                                       \
        char string[256];                                   \
        sprintf(string, "%s:%d:", __func__, __LINE__);      \
        __NE_debugprint(string);                            \
        sprintf(string, ##format);                          \
        __NE_debugprint(string);                            \
        __NE_debugprint("\n");                              \
    } while (0)

/// Function used internally by Nitro Engine to report error messages.
///
/// It is only used when NE_DEBUG is defined.
///
/// @param text Text to print.
void __NE_debugprint(const char *text);

/// Sets a debug handler where Nitro Engine will send debug information.
///
/// @param fn Handler where Nitro Engine will send debug information.
void NE_DebugSetHandler(void (*fn)(const char *));

/// Sets the console of libnds as destination of the debug information.
void NE_DebugSetHandlerConsole(void);

#else // #ifndef NE_DEBUG

#define NE_AssertMinMax(min, value, max, format...) \
    do {                                            \
        (void)(min);                                \
        (void)(value);                              \
        (void)(max);                                \
    } while (0);

#define NE_AssertPointer(ptr, format...)            \
    do {                                            \
        (void)(ptr);                                \
    } while (0);

#define NE_Assert(cond, format...)                  \
    do {                                            \
        (void)(cond);                               \
    } while (0);

#define NE_DebugPrint(format...)

#define NE_DebugSetHandler(fn)                      \
    do {                                            \
        (void)(fn);                                 \
    } while (0);

#define NE_DebugSetHandlerConsole()

#endif

/// @}

/// @defgroup touch_test Touch test
///
/// Functions to detect if an object is being pressed by the stylus.
///
/// If you want to know if you are touching something in the touch screen you
/// can use the functions of this group. They perform tests to know if an object
/// is under the coordinates of the stylus, and they return its distance from
/// the camera.
///
/// TODO: There is a bug in gluPickMatrix() when first two coordinates of
/// viewport are different from (0,0).
///
/// Note: This uses last stylus coordinates even if it has been released. You
/// will have to check by yourself if the stylus is really in contact with the
/// screen.
///
/// Note: It two objects overlap, the test may fail to diferenciate which of
/// them is closer to the camera.
///
/// Note: If you want to draw something without using NE_ModelDraw(), you will
/// have to move the view to the center of your model and use
/// PosTest_Asynch(0, 0, 0).
///
/// This doesn't work in most emulators, but it works in melonDS.
///
/// How to use this test:
///
/// 1. Init the "touch test mode" with NE_TouchTestStart() to prepare the
///    hardware. During this mode, polygons are not drawn on the screen. For
///    this reason, it is possible to use simplified models (without texture,
///    normals, with fewer polygons, etc) to speed up the process.
///
/// 2. Call NE_TouchTestObject().
///
/// 3. Draw the model with NE_ModelDraw(), for example.
///
/// 4. Call NE_TouchTestResult() to know if it is being touched. If it is being
///    touched, it returns the distance from the camera.
///
/// 5. Repeat 2-4 for each model you want to test.
///
/// 6. Call NE_TouchTestEnd() to exit "touch test mode".
///
/// Adapted from the Picking example of libnds by Gabe Ghearing.
///
/// @{

/// Starts "touch test mode" and saves all current matrices.
///
/// Polygons drawn after this won't be displayed on screen. It is a good idea to
/// use models with less details than the original to perform this test.
void NE_TouchTestStart(void);

/// Starts a test for a model.
void NE_TouchTestObject(void);

/// Gets the result of the touch test.
///
/// @return It returns -1 if the model is NOT being touched. If it is touched,
///         it returns the distance to the camera.
int NE_TouchTestResult(void);

/// Ends "touch test mode" and restores the previous matrices.
///
/// Polygons drawn after this will be displayed on screen.
void NE_TouchTestEnd(void);

/// @}

#endif // NE_GENERAL_H__
