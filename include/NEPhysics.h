// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#ifndef NE_PHYSICS_H__
#define NE_PHYSICS_H__

/*! \file   NEPhysics.h
 *  \brief  Simple physics engine.
 */

/*! @defgroup physics Physics engine.
 *
 * A very simple physics engine. It only supports axis-aligned bounding boxes.
 *
 * This will be (maybe) removed from Nitro Engine in the future...
 *
 * @{
 */

/*! \def    #define NE_DEFAULT_PHYSICS  128 */
#define NE_DEFAULT_PHYSICS  128

/*! \def    #define NE_MIN_BOUNCE_SPEED (floattof32(0.01))
 *  \brief  Minimun speed that an object has to have to bounce. If it has less
 *          speed it will stop.
 */
#define NE_MIN_BOUNCE_SPEED (floattof32(0.01))

/*! \enum NE_PhysicsTypes
 *  \brief Object types used in physics engine.
 */
typedef enum {
	NE_BoundingBox		= 1,	/*!< Bounding box. */
	NE_BoundingSphere	= 2,	/*!< Bounding sphere. */
	NE_Dot			= 3	/*!< Use this with really small objects. */
} NE_PhysicsTypes;

/*! \enum NE_OnCollition
 *  \brief Possible actions that can be applied to an object after a collision.
 */
typedef enum {
	NE_ColNothing = 0,	/*!< Ignore the object. */
	NE_ColBounce,		/*!< Bounce against the object. */
	NE_ColStop		/*!< Stop. */
} NE_OnCollition;

/*! \struct NE_Physics
 *  \brief  Holds information of a physics object.
 */
typedef struct {
	NE_Model *model;

	u32 type;
	int xspeed, yspeed, zspeed;	// Coordinates are taken from the model.
	//EVERY FIXED POINT IS <f32>
	int radius;			// for spheres
	int xsize, ysize, zsize;	// for boxes

	int gravity, friction;
	int keptpercent;		// Percent of energy in an object after bouncing

	int on;				// What to do when a collision is detected.
	bool iscolliding;		// true if a  was detected last time.

	bool enabled;
	int physicsgroup;		// to avoid useless checking
} NE_Physics;

/*! \fn    NE_Physics *NE_PhysicsCreate(NE_PhysicsTypes type);
 *  \brief Creates a physics object of given type.
 *  \param type Object type.
 */
NE_Physics *NE_PhysicsCreate(NE_PhysicsTypes type);

/*! \fn    void NE_PhysicsDelete(NE_Physics *pointer);
 *  \brief Deletes a physics object.
 *  \param pointer Pointer to the object.
 */
void NE_PhysicsDelete(NE_Physics *pointer);

/*! \fn    void NE_PhysicsDeleteAll(void);
 *  \brief Deletes all physic objects.
 */
void NE_PhysicsDeleteAll(void);

/*! \fn    void NE_PhysicsSystemReset(int number_of_objects);
 *  \brief Resets the physics engine and sets the maximun number of objects.
 *  \param number_of_objects Number of objects. If it is less than 1, it will
 *         create space for NE_DEFAULT_PHYSICS.
 */
void NE_PhysicsSystemReset(int number_of_objects);

/*! \fn    void NE_PhysicsSystemEnd(void);
 *  \brief Ends physics engine and all memory used by it.
 */
void NE_PhysicsSystemEnd(void);

/*! \fn    void NE_PhysicsSetRadiusI(NE_Physics *pointer, int radius);
 *  \brief Set radius of a bounding sphere.
 *  \param pointer Pointer to the object.
 *  \param radius Sphere's radius.
 */
void NE_PhysicsSetRadiusI(NE_Physics *pointer, int radius);

/*! \def   NE_PhysicsSetRadius(NE_Physics *pointer, float radius);
 *  \brief Set radius of a bounding sphere.
 *  \param p Pointer to the object.
 *  \param r Sphere's radius.
 */
#define NE_PhysicsSetRadius(p, r) \
	NE_PhysicsSetRadiusI(p, floattof32(r))

/*! \fn    void NE_PhysicsSetSpeedI(NE_Physics *pointer, int x, int y, int z);
 *  \brief Set speed of a physics object.
 *  \param pointer Pointer to the object.
 *  \param x (x, y, z) Speed vector.
 *  \param y (x, y, z) Speed vector.
 *  \param z (x, y, z) Speed vector.
 */
void NE_PhysicsSetSpeedI(NE_Physics *pointer, int x, int y, int z);

/*! \def   NE_PhysicsSetSpeed(NE_Physics *pointer, float x, float y, float z);
 *  \brief Set speed of a physics object.
 *  \param p Pointer to the object.
 *  \param x (x, y, z) Speed vector.
 *  \param y (x, y, z) Speed vector.
 *  \param z (x, y, z) Speed vector.
 */
#define NE_PhysicsSetSpeed(p, x, y, z) \
	NE_PhysicsSetSpeedI(p, floattof32(x), floattof32(y), floattof32(z))

/*! \fn    void NE_PhysicsSetSizeI(NE_Physics *pointer, int x, int y, int z);
 *  \brief Set size of a bounding box.
 *  \param pointer Pointer to the physics object.
 *  \param x (x, y, z) Size.
 *  \param y (x, y, z) Size.
 *  \param z (x, y, z) Size.
 */
void NE_PhysicsSetSizeI(NE_Physics *pointer, int x, int y, int z);

/*! \def    NE_PhysicsSetSize(NE_Physics *pointer, float x, float y, float z);
 *  \brief Set size of a bounding box.
 *  \param p Pointer to the physics object.
 *  \param x (x, y, z) Size.
 *  \param y (x, y, z) Size.
 *  \param z (x, y, z) Size.
 */
#define NE_PhysicsSetSize(p, x, y, z) \
	NE_PhysicsSetSizeI(p, floattof32(x), floattof32(y), floattof32(z))

/*! \fn    void NE_PhysicsSetGravityI(NE_Physics *pointer, int gravity);
 *  \brief Set gravity for a physics object.
 *  \param pointer Pointer to the physics object.
 *  \param gravity Gravity on the Y axis.
 */
void NE_PhysicsSetGravityI(NE_Physics *pointer, int gravity);

/*! \def   NE_PhysicsSetGravity(NE_Physics *pointer, float gravity);
 *  \brief Set gravity for a physics object.
 *  \param p Pointer to the physics object.
 *  \param g Gravity on the Y axis.
 */
#define NE_PhysicsSetGravity(p, g) \
	NE_PhysicsSetGravityI(p, floattof32(g))

/*! \fn    void NE_PhysicsSetFrictionI(NE_Physics *pointer, int friction);
 *  \brief Set friction for a physics object.
 *  \param pointer Pointer to the physics object.
 *  \param friction Friction.
 */
void NE_PhysicsSetFrictionI(NE_Physics *pointer, int friction);

/*! \def   NE_PhysicsSetFriction(NE_Physics *pointer, float friction);
 *  \brief Set friction for a physics object.
 *  \param p Pointer to the physics object.
 *  \param f Friction.
 */
#define NE_PhysicsSetFriction(p, f) \
	NE_PhysicsSetFrictionI(p, floattof32(f))

/*! \fn    void NE_PhysicsSetBounceEnergy(NE_Physics *pointer, int percent);
 *  \brief Set percent of energy kept after a bounce.
 *  \param pointer Pointer to the physics object.
 *  \param percent Percent of energy kept.
 */
void NE_PhysicsSetBounceEnergy(NE_Physics *pointer, int percent);

/*! \fn    void NE_PhysicsEnable(NE_Physics *pointer, bool value);
 *  \brief Enable movement for a physics object.
 *  \param pointer Pointer to the physics object.
 *  \param value True enables physics, false disables it.
 *
 * If false, it won't update anything of this object.
 * Use this to save some time if given object is, for example, the floor.
 */
void NE_PhysicsEnable(NE_Physics *pointer, bool value);

/*! \fn    void NE_PhysicsSetModel(NE_Physics *physics, void *modelpointer);
 *  \brief Assign a model object to a physics object.
 *  \param physics Pointer to the physics object.
 *  \param modelpointer Pointer to the model (animated or not).
 */
void NE_PhysicsSetModel(NE_Physics *physics, void *modelpointer);

/*! \fn    void NE_PhysicsSetGroup(NE_Physics *physics, int group);
 *  \brief Sets phisics group of an object.
 *  \param physics Pointer to the object.
 *  \param group New physics group.
 *
 * NOTE: Collitions do only work between objects with the same group.
 */
void NE_PhysicsSetGroup(NE_Physics *physics, int group);

/*! \fn    void NE_PhysicsOnCollition(NE_Physics *physics,
 *                                    NE_OnCollition action);
 *  \brief Set action to do if collision.
 *  \param physics Pointer to the object.
 *  \param action Action. */
void NE_PhysicsOnCollition(NE_Physics *physics, NE_OnCollition action);

/*! \fn    bool NE_PhysicsIsColliding(NE_Physics *pointer);
 *  \brief Returns true if given object was colliding with other object last
 *         time it updated.
 *  \param pointer Pointer to the object.
 *
 * This doesn't work with disabled objects.
 */
bool NE_PhysicsIsColliding(NE_Physics *pointer);

/*! \fn    void NE_PhysicsUpdateAll(void);
 *  \brief Updates every physics object.
 */
void NE_PhysicsUpdateAll(void);

/*! \fn    void NE_PhysicsUpdate(NE_Physics *pointer);
 *  \brief Updates given physics object.
 *  \param pointer Pointer to the object.
 */
void NE_PhysicsUpdate(NE_Physics *pointer);

/*! \fn    bool NE_PhysicsCheckCollition(NE_Physics *pointer1,
 *                                       NE_Physics *pointer2);
 *  \brief Returns true if given objects are colliding without checking physics
 *         groups.
 *  \param pointer1 Pointer to first object.
 *  \param pointer2 Pointer to second object.
 */
bool NE_PhysicsCheckCollition(NE_Physics *pointer1, NE_Physics *pointer2);

/*! @} */

#endif // NE_PHYSICS_H__
