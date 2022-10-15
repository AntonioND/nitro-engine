// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, 2022 Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include "NEMain.h"

/// @file NEPhysics.c

static NE_Physics **NE_PhysicsPointers;
static bool ne_physics_system_inited = false;

static int NE_MAX_PHYSICS;

NE_Physics *NE_PhysicsCreate(NE_PhysicsTypes type)
{
    if (!ne_physics_system_inited)
        return NULL;

    // TODO
    if (type == NE_BoundingSphere)
    {
        NE_DebugPrint("Bounding spheres not supported");
        return NULL;
    }
    if (type == NE_Dot)
    {
        NE_DebugPrint("Dots not supported");
        return NULL;
    }

    NE_Physics *temp = calloc(1, sizeof(NE_Physics));
    NE_AssertPointer(temp, "Not enough memory");

    int i = 0;
    while (1)
    {
        if (i == NE_MAX_PHYSICS)
        {
            free(temp);
            NE_DebugPrint("No free slots");
            return NULL;
        }
        if (NE_PhysicsPointers[i] == NULL)
        {
            NE_PhysicsPointers[i] = temp;
            break;
        }
        i++;
    }

    temp->type = type;
    temp->keptpercent = 50;
    temp->enabled = true;
    temp->physicsgroup = 0;
    temp->oncollision = NE_ColNothing;

    return temp;
}

void NE_PhysicsDelete(NE_Physics *pointer)
{
    if (!ne_physics_system_inited)
        return;

    NE_AssertPointer(pointer, "NULL pointer");

    int i = 0;
    while (1)
    {
        if (i == NE_MAX_PHYSICS)
            break;
        if (NE_PhysicsPointers[i] == pointer)
        {
            NE_PhysicsPointers[i] = NULL;
            free(pointer);
            break;
        }
        i++;
    }

    NE_DebugPrint("Object not found");
}

void NE_PhysicsDeleteAll(void)
{
    if (!ne_physics_system_inited)
        return;

    for (int i = 0; i < NE_MAX_PHYSICS; i++)
        NE_PhysicsDelete(NE_PhysicsPointers[i]);
}

void NE_PhysicsSystemReset(int max_objects)
{
    if (ne_physics_system_inited)
        NE_PhysicsSystemEnd();

    if (max_objects < 1)
        NE_MAX_PHYSICS = NE_DEFAULT_PHYSICS;
    else
        NE_MAX_PHYSICS = max_objects;

    NE_PhysicsPointers = calloc(NE_MAX_PHYSICS, sizeof(NE_PhysicsPointers));
    NE_AssertPointer(NE_PhysicsPointers, "Not enough memory");

    ne_physics_system_inited = true;
}

void NE_PhysicsSystemEnd(void)
{
    if (!ne_physics_system_inited)
        return;

    NE_PhysicsDeleteAll();

    free(NE_PhysicsPointers);

    ne_physics_system_inited = false;
}

void NE_PhysicsSetRadiusI(NE_Physics *pointer, int radius)
{
    NE_AssertPointer(pointer, "NULL pointer");
    NE_Assert(pointer->type == NE_BoundingSphere, "Not a bounding shpere");
    NE_Assert(radius >= 0, "Radius must be positive");
    pointer->radius = radius;
}

void NE_PhysicsSetSpeedI(NE_Physics *pointer, int x, int y, int z)
{
    NE_AssertPointer(pointer, "NULL pointer");
    pointer->xspeed = x;
    pointer->yspeed = y;
    pointer->zspeed = z;
}

void NE_PhysicsSetSizeI(NE_Physics *pointer, int x, int y, int z)
{
    NE_AssertPointer(pointer, "NULL pointer");
    NE_Assert(pointer->type == NE_BoundingBox, "Not a bounding box");
    NE_Assert(x >= 0 && y >= 0 && z >= 0, "Size must be positive!!");
    pointer->xsize = x;
    pointer->ysize = y;
    pointer->zsize = z;
}

void NE_PhysicsSetGravityI(NE_Physics *pointer, int gravity)
{
    NE_AssertPointer(pointer, "NULL pointer");
    pointer->gravity = gravity;
}

void NE_PhysicsSetFrictionI(NE_Physics *pointer, int friction)
{
    NE_AssertPointer(pointer, "NULL pointer");
    NE_Assert(friction >= 0, "Friction must be positive");
    pointer->friction = friction;
}

void NE_PhysicsSetBounceEnergy(NE_Physics *pointer, int percent)
{
    NE_AssertPointer(pointer, "NULL pointer");
    NE_Assert(percent >= 0, "Percentage must be positive");
    pointer->keptpercent = percent;
}

void NE_PhysicsEnable(NE_Physics *pointer, bool value)
{
    NE_AssertPointer(pointer, "NULL pointer");
    pointer->enabled = value;
}

void NE_PhysicsSetModel(NE_Physics *physics, void *modelpointer)
{
    NE_AssertPointer(physics, "NULL physics pointer");
    NE_AssertPointer(modelpointer, "NULL model pointer");
    physics->model = modelpointer;
}

void NE_PhysicsSetGroup(NE_Physics *physics, int group)
{
    NE_AssertPointer(physics, "NULL pointer");
    physics->physicsgroup = group;
}

void NE_PhysicsOnCollision(NE_Physics *physics, NE_OnCollision action)
{
    NE_AssertPointer(physics, "NULL pointer");
    physics->oncollision = action;
}

bool NE_PhysicsIsColliding(NE_Physics *pointer)
{
    NE_AssertPointer(pointer, "NULL pointer");
    return pointer->iscolliding;
}

void NE_PhysicsUpdateAll(void)
{
    if (!ne_physics_system_inited)
        return;

    for (int i = 0; i < NE_MAX_PHYSICS; i++)
    {
        if (NE_PhysicsPointers[i] != NULL)
            NE_PhysicsUpdate(NE_PhysicsPointers[i]);
    }
}

void NE_PhysicsUpdate(NE_Physics *pointer)
{
    if (!ne_physics_system_inited)
        return;

    NE_AssertPointer(pointer, "NULL pointer");
    NE_AssertPointer(pointer->model, "NULL model pointer");
    NE_Assert(pointer->type != 0, "Object has no type");

    if (pointer->enabled == false)
        return;

    pointer->iscolliding = false;

    // We change Y speed depending on gravity.
    pointer->yspeed -= pointer->gravity;

    // Now, let's move the object

    // Used in collision checking to simplify the code
    int posx = 0, posy = 0, posz = 0;
    // Position before movement
    int bposx = 0, bposy = 0, bposz = 0;

    NE_Model *model = pointer->model;
    bposx = model->x;
    bposy = model->y;
    bposz = model->z;
    posx = model->x = model->x + pointer->xspeed;
    posy = model->y = model->y + pointer->yspeed;
    posz = model->z = model->z + pointer->zspeed;

    // Gravity and movement have been applied, time to check collisions...
    bool xenabled = true, yenabled = true, zenabled = true;
    if (bposx == posx)
        xenabled = false;
    if (bposy == posy)
        yenabled = false;
    if (bposz == posz)
        zenabled = false;

    for (int i = 0; i < NE_MAX_PHYSICS; i++)
    {
        if (NE_PhysicsPointers[i] == NULL)
            continue;

        // Check that we aren't checking an object with itself
        if (NE_PhysicsPointers[i] == pointer)
            continue;

        // Check that both objects are in the same group
        if (NE_PhysicsPointers[i]->physicsgroup != pointer->physicsgroup)
            continue;

        NE_Physics *otherpointer = NE_PhysicsPointers[i];
        //Get coordinates
        int otherposx = 0, otherposy = 0, otherposz = 0;

        model = otherpointer->model;
        otherposx = model->x;
        otherposy = model->y;
        otherposz = model->z;

        // Both are boxes
        if (pointer->type == NE_BoundingBox && otherpointer->type == NE_BoundingBox)
        {
            bool collision =
                ((abs(posx - otherposx) < (pointer->xsize + otherpointer->xsize) >> 1) &&
                (abs(posy - otherposy) < (pointer->ysize + otherpointer->ysize) >> 1) &&
                (abs(posz - otherposz) < (pointer->zsize + otherpointer->zsize) >> 1));

            if (!collision)
                continue;

            pointer->iscolliding = true;

            if (pointer->oncollision == NE_ColBounce)
            {
                // Used to reduce speed:
                int temp = divf32(inttof32(pointer->keptpercent), inttof32(100));
                if ((yenabled) && ((abs(bposy - otherposy) >= (pointer->ysize + otherpointer->ysize) >> 1)))
                {
                    yenabled = false;
                    pointer->yspeed += pointer->gravity;

                    if (posy > otherposy)
                        (pointer->model)->y = otherposy + ((pointer->ysize + otherpointer->ysize) >> 1);
                    if (posy < otherposy)
                        (pointer->model)->y = otherposy - ((pointer->ysize + otherpointer->ysize) >> 1);

                    if (pointer->gravity == 0)
                    {
                        pointer->yspeed =
                            -mulf32(temp, pointer->yspeed);
                    }
                    else
                    {
                        if (abs(pointer->yspeed) > NE_MIN_BOUNCE_SPEED)
                        {
                            pointer->yspeed = -mulf32(temp, pointer->yspeed - pointer->gravity);
                        }
                        else
                        {
                            pointer->yspeed = 0;
                        }
                    }
                }
                else if ((xenabled) && ((abs(bposx - otherposx) >= (pointer->xsize + otherpointer->xsize) >> 1)))
                {
                    xenabled = false;

                    if (posx > otherposx)
                        (pointer->model)->x = otherposx + ((pointer->xsize + otherpointer->xsize) >> 1);
                    if (posx < otherposx)
                        (pointer->model)->x = otherposx - ((pointer->xsize + otherpointer->xsize) >> 1);

                    pointer->xspeed = -mulf32(temp, pointer->xspeed);
                }
                else if ((zenabled) && ((abs(bposz - otherposz) >= (pointer->zsize + otherpointer->zsize) >> 1)))
                {
                    zenabled = false;

                    if (posz > otherposz)
                        (pointer->model)->z = otherposz + ((pointer->zsize + otherpointer->zsize) >> 1);
                    if (posz < otherposz)
                        (pointer->model)->z = otherposz - ((pointer->zsize + otherpointer->zsize) >> 1);

                    pointer->zspeed = -mulf32(temp, pointer->zspeed);
                }
            }
            else if (pointer->oncollision == NE_ColStop)
            {
                if ((yenabled) && ((abs(bposy - otherposy) >= (pointer->ysize + otherpointer->ysize) >> 1)))
                {
                    yenabled = false;

                    if (posy > otherposy)
                        (pointer->model)->y = otherposy + ((pointer->ysize + otherpointer->ysize) >> 1);
                    if (posy < otherposy)
                        (pointer->model)->y = otherposy - ((pointer->ysize + otherpointer->ysize) >> 1);
                }
                if ((xenabled) && ((abs(bposx - otherposx) >= (pointer->xsize + otherpointer->xsize) >> 1)))
                {
                    xenabled = false;

                    if (posx > otherposx)
                        (pointer->model)->x = otherposx + ((pointer->xsize + otherpointer->xsize) >> 1);
                    if (posx < otherposx)
                        (pointer->model)->x = otherposx - ((pointer->xsize + otherpointer->xsize) >> 1);
                }
                if ((zenabled) && ((abs(bposz - otherposz) >= (pointer->zsize + otherpointer->zsize) >> 1)))
                {
                    zenabled = false;

                    if (posz > otherposz)
                        (pointer->model)->z = otherposz + ((pointer->zsize + otherpointer->zsize) >> 1);
                    if (posz < otherposz)
                        (pointer->model)->z = otherposz - ((pointer->zsize + otherpointer->zsize) >> 1);
                }
                pointer->xspeed = pointer->yspeed = pointer->zspeed = 0;
            }
        }
    }

    //Now, we get the module of speed in order to apply friction.
    if (pointer->friction != 0)
    {
        pointer->xspeed <<= 10;
        pointer->yspeed <<= 10;
        pointer->zspeed <<= 10;
        int _mod_ = mulf32(pointer->xspeed, pointer->xspeed);
        _mod_ += mulf32(pointer->yspeed, pointer->yspeed);
        _mod_ += mulf32(pointer->zspeed, pointer->zspeed);
        _mod_ = sqrtf32(_mod_);

        //check if module is very small -> speed = 0
        if (_mod_ < pointer->friction)
        {
            pointer->xspeed = pointer->yspeed = pointer->zspeed = 0;
        }
        else
        {
            int newmod = _mod_ - pointer->friction;
            // mod   --  newmod    ->  newspeed = speed * newmod / mod
            // speed --  newspeed
            int number = divf32(newmod, _mod_);
            pointer->xspeed = mulf32(pointer->xspeed, number);
            pointer->yspeed = mulf32(pointer->yspeed, number);
            pointer->zspeed = mulf32(pointer->zspeed, number);
            pointer->xspeed >>= 10;
            pointer->yspeed >>= 10;
            pointer->zspeed >>= 10;
        }
    }
}

bool NE_PhysicsCheckCollision(NE_Physics *pointer1, NE_Physics *pointer2)
{
    NE_AssertPointer(pointer1, "NULL pointer 1");
    NE_AssertPointer(pointer2, "NULL pointer 2");
    NE_Assert(pointer1 != pointer2, "Both objects are the same one");

    //Get coordinates
    int posx = 0, posy = 0, posz = 0;

    NE_Model *model = pointer1->model;
    posx = model->x;
    posy = model->y;
    posz = model->z;

    int otherposx = 0, otherposy = 0, otherposz = 0;

    model = pointer2->model;
    otherposx = model->x;
    otherposy = model->y;
    otherposz = model->z;

    // Both are boxes
    //if(pointer->type == NE_BoundingBox && otherpointer->type == NE_BoundingBox)
    //{
    if ((abs(posx - otherposx) < (pointer1->xsize + pointer2->xsize) >> 1) &&
        (abs(posy - otherposy) < (pointer1->ysize + pointer2->ysize) >> 1) &&
        (abs(posz - otherposz) < (pointer1->zsize + pointer2->zsize) >> 1))
    {
        return true;
    }
    // else if (...) TODO: Spheres and dots

    return false;
}
