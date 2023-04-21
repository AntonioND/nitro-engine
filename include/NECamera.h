// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, 2022 Antonio Niño Díaz
//
// This file is part of Nitro Engine

#ifndef NE_CAMERA_H__
#define NE_CAMERA_H__

#include <nds.h>

/// @file   NECamera.h
/// @brief  Structs and functions used to move/rotate/etc the camera.

/// @defgroup camera_system Camera System
///
/// Functions to manipulate cameras.
///
/// Be careful. If the 'look' vector of the camera is on the same axis as the
/// 'up' vector, projection will be incorrect. For example, if 'up' = (0, 1, 0),
/// avoid looking at (0, 1, 0) and (0, -1, 0) and very similar vectors.
///
/// @{

/// Holds information of the camera.
typedef struct {
    m4x4 matrix;   ///< Matrix that represents the transformation
    int32 from[3]; ///< Coordinates of location of the camera
    int32 to[3];   ///< Where the camera is looking at
    int32 up[3];   ///< Vector that points "up"
    bool matrix_is_updated; ///< Set to false when the matrix isn't up-to-date
} NE_Camera;

#define NE_DEFAULT_CAMERAS 16 ///< Default max number of cameras.

/// Creates a new camera object.
///
/// @return Pointer to the newly created camera.
NE_Camera *NE_CameraCreate(void);

/// Setup camera.
///
/// @param cam NE_Camera struct to hold the information.
/// @param xfrom (xfrom, yfrom, zfrom) Camera position coordinates (f32).
/// @param yfrom (xfrom, yfrom, zfrom) Camera position coordinates (f32).
/// @param zfrom (xfrom, yfrom, zfrom) Camera position coordinates (f32).
/// @param xto (xto, yto, zto) Where the camera looks at (f32).
/// @param yto (xto, yto, zto) Where the camera looks at (f32).
/// @param zto (xto, yto, zto) Where the camera looks at (f32).
/// @param xup (xup, yup, zup) Unit vector pointing up (f32).
/// @param yup (xup, yup, zup) Unit vector pointing up (f32).
/// @param zup (xup, yup, zup) Unit vector pointing up (f32).
void NE_CameraSetI(NE_Camera *cam, int xfrom, int yfrom, int zfrom,
                   int xto, int yto, int zto, int xup, int yup, int zup);

/// Setup camera.
///
/// All values are in float format.
///
/// @param c NE_Camera struct to hold the information.
/// @param x1 (xfrom, yfrom, zfrom) Camera position coordinates (float).
/// @param y1 (xfrom, yfrom, zfrom) Camera position coordinates (float).
/// @param z1 (xfrom, yfrom, zfrom) Camera position coordinates (float).
/// @param x2 (xto, yto, zto) Where the camera look at (float).
/// @param y2 (xto, yto, zto) Where the camera look at (float).
/// @param z2 (xto, yto, zto) Where the camera look at (float).
/// @param x3 (xup, yup, zup) Unit vector pointing up (float).
/// @param y3 (xup, yup, zup) Unit vector pointing up (float).
/// @param z3 (xup, yup, zup) Unit vector pointing up (float).
#define NE_CameraSet(c, x1, y1, z1, x2, y2, z2, x3, y3, z3) \
    NE_CameraSetI(c, floattof32(x1), floattof32(y1), floattof32(z1), \
                  floattof32(x2), floattof32(y2), floattof32(z2), \
                  floattof32(x3), floattof32(y3), floattof32(z3))

/// Set current view to the one of the specified camera.
///
/// @param cam Camera to be used.
void NE_CameraUse(NE_Camera *cam);

/// Moves a camera on the global x, y and z axes.
///
/// @param cam Camera to be moved.
/// @param x (x, y, z) Translation (f32).
/// @param y (x, y, z) Translation (f32).
/// @param z (x, y, z) Translation (f32).
void NE_CameraMoveI(NE_Camera *cam, int x, int y, int z);

/// Moves a camera on the global x, y and z axes.
///
/// @param c Camera to be moved.
/// @param x (x, y, z) Translation (float).
/// @param y (x, y, z) Translation (float).
/// @param z (x, y, z) Translation (float).
#define NE_CameraMove(c, x, y, z) \
    NE_CameraMoveI(c, floattof32(x), floattof32(y), floattof32(z))

/// Moves a camera on its local x, y and z axes.
///
/// @param cam Camera to be moved.
/// @param front Distance to be moved (f32).
/// @param right Distance to be moved (f32).
/// @param up Distance to be moved (f32).
void NE_CameraMoveFreeI(NE_Camera *cam, int front, int right, int up);

/// Moves a camera on its local x, y and z axes.
///
/// @param c Camera to be moved.
/// @param f Distance to be moved (float).
/// @param r Distance to be moved (float).
/// @param u Distance to be moved (float).
#define NE_CameraMoveFree(c, f, r, u) \
    NE_CameraMoveFreeI(c, floattof32(f), floattof32(r), floattof32(u))

/// Rotates a camera by its center by the global x, y and z axes.
///
/// @param cam Camera to be rotated.
/// @param rx Angle to rotate on the X axis (0 - 511).
/// @param ry Angle to rotate on the Y axis.
/// @param rz Angle to rotate on the Z axis.
void NE_CameraRotate(NE_Camera *cam, int rx, int ry, int rz);

/// Rotates a camera by its center by a custom axis.
///
/// @param cam Camera to be rotated.
/// @param angle Angle to rotate (0 - 511).
/// @param x (x, y, z) Axis vector (f32).
/// @param y (x, y, z) Axis vector (f32).
/// @param z (x, y, z) Axis vector (f32).
void NE_CameraRotateAxisI(NE_Camera *cam, int angle, int x, int y, int z);

/// Rotates a camera by its center by a custom axis.
///
/// @param c Camera to be rotated.
/// @param a Angle to rotate (0 - 511).
/// @param x (x, y, z) Axis vector (float).
/// @param y (x, y, z) Axis vector (float).
/// @param z (x, y, z) Axis vector (float).
#define NE_CameraRotateAxis(c, a, x, y, z) \
    NE_CameraRotateAxisI(c, a, floattof32(x), floattof32(y), floattof32(z))

/// Rotates a camera by its center by its local x, y and z axes.
///
/// @param cam Camera to be rotated.
/// @param rx Angle to rotate on the X axis (Up-Down) (0 - 511).
/// @param ry Angle to rotate on the Y axis (Right-Left).
/// @param rz Angle to rotate on the Z axis (Rotate view).
void NE_CameraRotateFree(NE_Camera *cam, int rx, int ry, int rz);

/// Resets the camera system and sets the maximun number of cameras.
///
/// @param max_cameras Number of cameras. If it is less than 1, it will create
///                    space for NE_DEFAULT_CAMERAS.
/// @return Returns 0 on success.
int NE_CameraSystemReset(int max_cameras);

/// Ends camera system and all memory used by it.
void NE_CameraSystemEnd(void);

/// Deletes a camera object.
///
/// @param cam Camera to be deleted.
void NE_CameraDelete(NE_Camera *cam);

/// @}

/// @defgroup view_system 3D View Helpers
///
/// Functions to move the current view without using cameras. The view isn't
/// saved anywhere because it isn't asociated to any object.
///
/// @{

/// Pushes current view to the stack.
void NE_ViewPush(void);

/// Pops last pushed view.
void NE_ViewPop(void);

/// Translate the view.
///
/// @param x (x, y, z) Translation vector (f32).
/// @param y (x, y, z) Translation vector (f32).
/// @param z (x, y, z) Translation vector (f32).
void NE_ViewMoveI(int x, int y, int z);

/// Translate the view.
///
/// @param x (x, y, z) Translation vector (float).
/// @param y (x, y, z) Translation vector (float).
/// @param z (x, y, z) Translation vector (float).
#define NE_ViewMove(x, y, z) \
    NE_ViewMoveI(floattof32(x), floattof32(y), floattof32(z))

/// Rotates the view.
//
/// @param rx Degrees (0 - 511) of rotation by X axis.
/// @param ry Degrees (0 - 511) of rotation by Y axis.
/// @param rz Degrees (0 - 511) of rotation by Z axis.
void NE_ViewRotate(int rx, int ry, int rz);

/// Scale the view.
///
/// @param x Factor to scale the X axis by (f32).
/// @param y Factor to scale the Y axis by (f32).
/// @param z Factor to scale the Z axis by (f32).
void NE_ViewScaleI(int x, int y, int z);

/// Scale the view.
///
/// @param x Factor to scale the X axis by (float).
/// @param y Factor to scale the Y axis by (float).
/// @param z Factor to scale the Z axis by (float).
#define NE_ViewScale(x,y,z) \
    NE_ViewScaleI(floattof32(x), floattof32(y), floattof32(z))

/// @}

#endif // NE_CAMERA_H__
