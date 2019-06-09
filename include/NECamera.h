// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#ifndef NE_CAMERA_H__
#define NE_CAMERA_H__

#include <nds.h>

/*! \file   NECamera.h
 *  \brief  Structs and functions used to move/rotate/etc the camera.
 */

/*! @defgroup camera_system Camera System.
 *
 * Functions to manipulate cameras.
 *
 * Be careful. If the 'look' vector of the camera is on the same axis as the
 * 'up' vector, projection will be incorrect. For example, if 'up' = (0, 1, 0),
 * avoid looking at (0, 1, 0) and (0, -1, 0) and very similar vectors (because
 * of unaccuracy of math functions).
 *
 * This could use hardware accelerated ways to calculate, but I prefer this way
 * to avoid getting an incorrect matrix and never being able to recuperate it
 * without reseting the camera.
 *
 * This is not meant to be your game's camera system. This is just for testing
 * levels and things like that.
 *
 * @{
*/

/*! \struct NE_Camera
 *  \brief  Holds information of the camera. The matrix is only updated when it
 *          is needed.
 */
typedef struct {
	m4x4 matrix;
	int32 from[3], to[3], up[3];
	bool matrix_is_updated;
} NE_Camera;

/*! \def #define NE_DEFAULT_CAMERAS 16 */
#define NE_DEFAULT_CAMERAS 16

/*! \fn     NE_Camera *NE_CameraCreate(void);
 *  \brief  Returns a pointer to a NE_Camera struct. */
NE_Camera *NE_CameraCreate(void);

/*! \fn    void NE_CameraSetI(NE_Camera *cam, int xfrom, int yfrom, int zfrom,
 *                            int xto, int yto, int zto,
 *                            int xup, int yup, int zup);
 *  \brief Set coordinates for a NE_Camera struct.
 *  \param cam NE_Camera struct to hold the information.
 *  \param xfrom (xfrom, yfrom, zfrom) Camera position coordinates.
 *  \param yfrom (xfrom, yfrom, zfrom) Camera position coordinates.
 *  \param zfrom (xfrom, yfrom, zfrom) Camera position coordinates.
 *  \param xto (xto, yto, zto) Where the camera look at.
 *  \param yto (xto, yto, zto) Where the camera look at.
 *  \param zto (xto, yto, zto) Where the camera look at.
 *  \param xup (xup, yup, zup) Unit vector pointing up for the camera.
 *  \param yup (xup, yup, zup) Unit vector pointing up for the camera.
 *  \param zup (xup, yup, zup) Unit vector pointing up for the camera. */
void NE_CameraSetI(NE_Camera *cam, int xfrom, int yfrom, int zfrom,
		   int xto, int yto, int zto, int xup, int yup, int zup);

/*! \def   NE_CameraSet(NE_Camera *cam, float xfrom, float yfrom, float zfrom,
 *                      float xto, float yto, float zto,
 *                      float xup, float yup, float zup);
 *  \brief Set coordinates for a NE_Camera struct.
 *  \param c NE_Camera struct to hold the information.
 *  \param x1 (xfrom, yfrom, zfrom) Camera position coordinates.
 *  \param y1 (xfrom, yfrom, zfrom) Camera position coordinates.
 *  \param z1 (xfrom, yfrom, zfrom) Camera position coordinates.
 *  \param x2 (xto, yto, zto) Where the camera look at.
 *  \param y2 (xto, yto, zto) Where the camera look at.
 *  \param z2 (xto, yto, zto) Where the camera look at.
 *  \param x3 (xup, yup, zup) Unit vector pointing up for the camera.
 *  \param y3 (xup, yup, zup) Unit vector pointing up for the camera.
 *  \param z3 (xup, yup, zup) Unit vector pointing up for the camera.
 */
#define NE_CameraSet(c, x1, y1, z1, x2, y2, z2, x3, y3, z3) \
	NE_CameraSetI(c, floattof32(x1), floattof32(y1), floattof32(z1), \
		      floattof32(x2), floattof32(y2), floattof32(z2), \
		      floattof32(x3), floattof32(y3), floattof32(z3))

/*! \fn    void NE_CameraUse(NE_Camera *cam);
 *  \brief Set view based on a NE_Camera struct.
 *  \param cam NE_Camera struct that holds the imformation to be used.
 */
void NE_CameraUse(NE_Camera *cam);

/*! \fn    void NE_CameraMoveI(NE_Camera *cam, int x, int y, int z);
 *  \brief Moves a camera on the global x, y and z axis.
 *  \param cam Camera to be moved.
 *  \param x (x, y, z) Vector.
 *  \param y (x, y, z) Vector.
 *  \param z (x, y, z) Vector.
 */
void NE_CameraMoveI(NE_Camera *cam, int x, int y, int z);

/*! \def   NE_CameraMove(NE_Camera * c, float x, float y, float z);
 *  \brief Moves a camera on the global x, y and z axis.
 *  \param c Camera to be moved.
 *  \param x (x, y, z) Vector.
 *  \param y (x, y, z) Vector.
 *  \param z (x, y, z) Vector.
 */
#define NE_CameraMove(c, x, y, z) \
	NE_CameraMoveI(c, floattof32(x), floattof32(y), floattof32(z))

/*! \fn    void NE_CameraMoveFreeI(NE_Camera *cam, int front, int right,
 *                                 int up);
 *  \brief Moves a camera on its own axis.
 *  \param cam Camera to be moved.
 *  \param front Distance to be moved.
 *  \param right Distance to be moved.
 *  \param up Distance to be moved.
 */
void NE_CameraMoveFreeI(NE_Camera *cam, int front, int right, int up);

/*! \def   NE_CameraMoveFree(NE_Camera *c, float f, float r, float u);
 *  \brief Moves a camera on its own axis.
 *  \param c Camera to be moved.
 *  \param f Distance to be moved.
 *  \param r Distance to be moved.
 *  \param u Distance to be moved.
 */
#define NE_CameraMoveFree(c, f, r, u) \
	NE_CameraMoveFreeI(c, floattof32(f), floattof32(r), floattof32(u))

/*! \fn    void NE_CameraRotate(NE_Camera *cam, int rx, int ry, int rz);
 *  \brief Rotates a camera by global x, y and z axis on its position.
 *  \param cam Camera to be rotated.
 *  \param rx Angle to rotate on the X axis (0 - 511).
 *  \param ry Angle to rotate on the Y axis.
 *  \param rz Angle to rotate on the Z axis.
 */
void NE_CameraRotate(NE_Camera *cam, int rx, int ry, int rz);

/*! \def   NE_CameraRotateAxisI(NE_Camera *cam, int angle, int x, int y, int z);
 *  \brief Rotates a camera by a custom axis.
 *  \param c Camera to be rotated.
 *  \param a Angle to rotate (0 - 511).
 *  \param x (x, y, z) Axis vector (f32).
 *  \param y (x, y, z) Axis vector.
 *  \param z (x, y, z) Axis vector.
 */
void NE_CameraRotateAxisI(NE_Camera *cam, int angle, int x, int y, int z);

/*! \def   NE_CameraRotateAxis(NE_Camera * c, int a, float x, float y, float z);
 *  \brief Rotates a camera by a custom axis.
 *  \param c Camera to be rotated.
 *  \param a Angle to rotate (0 - 511).
 *  \param x (x,y,z) Axis vector.
 *  \param y (x,y,z) Axis vector.
 *  \param z (x,y,z) Axis vector.
 */
#define NE_CameraRotateAxis(c, a, x, y, z) \
	NE_CameraRotateAxisI(c, a, floattof32(x), floattof32(y), floattof32(z))

/*! \fn    void NE_CameraRotate(NE_Camera *cam, int rx, int ry, int rz);
 *  \brief Rotates a camera by its relative x, y and z axis on its position.
 *  \param cam Camera to be rotated.
 *  \param rx Angle to rotate on the X axis (Up-Down) (0 - 511).
 *  \param ry Angle to rotate on the Y axis (Right-Left).
 *  \param rz Angle to rotate on the Z axis (Rotate view).
 */
void NE_CameraRotateFree(NE_Camera *cam, int rx, int ry, int rz);

/*! \fn    void NE_CameraSystemReset(int numcameras);
 *  \brief Resets the camera system and sets the maximun number of cameras.
 *  \param numcameras Number of cameras. If it is less than 1, it will create
 *         space for NE_DEFAULT_CAMERAS.
 */
void NE_CameraSystemReset(int numcameras);

/*! \fn    void NE_CameraSystemEnd(void);
 *  \brief Ends camera system and all memory used by it.
 */
void NE_CameraSystemEnd(void);

/*! \fn    void NE_CameraDelete(NE_Camera *cam);
 *  \brief Frees memory used by a NE_Camera struct.
 *  \param cam NE_Camera struct to be freed.
 */
void NE_CameraDelete(NE_Camera *cam);

/*! \fn    void NE_ViewPush(void);
 *  \brief Pushes current view to the stack.
 */
void NE_ViewPush(void);

/*! \fn    void NE_ViewPop(void);
 *  \brief Pops last pushed view.
 */
void NE_ViewPop(void);

/*! \fn    void NE_ViewMoveI(int x, int y, int z);
 *  \brief Translate the camera. It doesn't save the information anywhere.
 *  \param x (x, y, z) Vector to move the camera.
 *  \param y (x, y, z) Vector to move the camera.
 *  \param z (x, y, z) Vector to move the camera.
 */
void NE_ViewMoveI(int x, int y, int z);

/*! \def   NE_ViewMove(float x, float y, float z);
 *  \brief Translate the camera. It doesn't save the information anywhere.
 *  \param x (x, y, z) Vector to move the camera.
 *  \param y (x, y, z) Vector to move the camera.
 *  \param z (x, y, z) Vector to move the camera.
 */
#define NE_ViewMove(x, y, z) \
	NE_ViewMoveI(floattof32(x), floattof32(y), floattof32(z))

/*! \fn    void NE_ViewRotate(int rx, int ry, int rz);
 *  \brief Rotates the camera. It doesn't save the information anywhere.
 *  \param rx Degrees (0 - 511) to rotate the camera by x axis.
 *  \param ry Degrees (0 - 511) to rotate the camera by y axis.
 *  \param rz Degrees (0 - 511) to rotate the camera by z axis.
 */
void NE_ViewRotate(int rx, int ry, int rz);

/*! \fn    void NE_ViewScaleI(int x, int y, int z);
 *  \brief Scale the camera. It doesn't save the information anywhere.
 *  \param x (x, y, z) Vector to scale the view by.
 *  \param y (x, y, z) Vector to scale the view by.
 *  \param z (x, y, z) Vector to scale the view by.
 */
void NE_ViewScaleI(int x, int y, int z);

/*! \def    NE_ViewScale(float x, float y, float z);
 *  \brief Scale the camera. It doesn't save the information anywhere.
 *  \param x (x, y, z) Vector to scale the view by.
 *  \param y (x, y, z) Vector to scale the view by.
 *  \param z (x, y, z) Vector to scale the view by.
 */
#define NE_ViewScale(x,y,z) \
	NE_ViewScaleI(floattof32(x), floattof32(y), floattof32(z))

/*! @} */

#endif // NE_CAMERA_H__
