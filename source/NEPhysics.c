// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2022 Antonio Niño Díaz
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
    {
        NE_DebugPrint("System not initialized");
        return NULL;
    }

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
    if (temp == NULL)
    {
        NE_DebugPrint("Not enough memory");
        return NULL;
    }

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
        {
            NE_DebugPrint("Object not found");
            return;
        }

        if (NE_PhysicsPointers[i] == pointer)
        {
            NE_PhysicsPointers[i] = NULL;
            free(pointer);
            return;
        }
        i++;
    }
}

void NE_PhysicsDeleteAll(void)
{
    if (!ne_physics_system_inited)
        return;

    for (int i = 0; i < NE_MAX_PHYSICS; i++)
        NE_PhysicsDelete(NE_PhysicsPointers[i]);
}

int NE_PhysicsSystemReset(int max_objects)
{
    if (ne_physics_system_inited)
        NE_PhysicsSystemEnd();

    if (max_objects < 1)
        NE_MAX_PHYSICS = NE_DEFAULT_PHYSICS;
    else
        NE_MAX_PHYSICS = max_objects;

    NE_PhysicsPointers = calloc(NE_MAX_PHYSICS, sizeof(NE_PhysicsPointers));
    if (NE_PhysicsPointers == NULL)
    {
        NE_DebugPrint("Not enough memory");
        return -1;
    }

    ne_physics_system_inited = true;
    return 0;
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

void NE_PhysicsSetModel(NE_Physics *physics, NE_Model *modelpointer)
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

bool NE_PhysicsIsColliding(const NE_Physics *pointer)
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

ARM_CODE void NE_PhysicsUpdate(NE_Physics *pointer)
{
    if (!ne_physics_system_inited)
        return;

    NE_AssertPointer(pointer, "NULL pointer");
    NE_AssertPointer(pointer->model, "NULL model pointer");
    NE_Assert(pointer->type != 0, "Object has no type");

    if (pointer->enabled == false)
        return;

    pointer->iscolliding = false;
    int32_t spd[3] = { pointer->xspeed, pointer->yspeed, pointer->zspeed };
    //above value should be chosen based on timestep
    int32_t nspd[3]={spd[0],spd[1],spd[2]};
    // We change Y speed depending on gravity.
    // doing it before applying friction makes it possible to have something stick to a wall
    nspd[1]-= pointer->gravity;
    //above update depends on timestep
    if (pointer->friction != 0)
    {
        // Now, we get the module of speed in order to apply friction.
        int64_t modsqrd = (int64_t)nspd[0] * nspd[0] + (int64_t)nspd[1] * nspd[1]
                        + (int64_t)nspd[2] * nspd[2];

        // This value should be chosen based on time since last update.
        int32_t friction = pointer->friction;
        int64_t diff = modsqrd + (int64_t)-friction * friction;

        // Computing the above is faster than waiting on hw

        // Check if module is very small -> speed = 0
        if (__builtin_expect(diff <= 0, 0))
        {
            nspd[0]=0;
            nspd[1]=0;
            nspd[2]=0;
        }
        else
        {
            uint32_t mod = sqrt64(modsqrd);
            div64_asynch((int64_t)(mod-friction) << 32, mod);
            // f < m therefore ((m - f) / m) < 1, therefore ((m - f) << 32 ) / m < (2^32)
            // i.e. the result fits in 32-bit
            uint32_t correction_factor = div64_result();
            #pragma GCC unroll 3
            for (int i = 0; i < 3; i++)
            {
                int32_t t = nspd[i];
                int32_t st = t;
                if (t < 0)
                    st = -st;
                st = ((uint64_t)(uint32_t)st * correction_factor) >> 32;
                if (t < 0)
                    st = -st;
                nspd[i] = st;
            }

        }
    }
    //here we could check if nspd=0 and then perform
    //a correction with a custom timestep dt=abs(velocity)/abs(acceleration)
    //but this is complex and slow so let's not do that

    //Now, let's move the object
    //below method for updating position is exact for constant acceleration
    //equivalent to s(t+dt)=s(t)+v(t)*dt+1/2*a*dt*dt
    //and probably good enough for 99 % of cases
    //can add 1 ulp of accuracy with round to nearest, not sure if worth it
    int posx, posy, posz;
    NE_Model *model = pointer->model;
    posx = model->x + ((spd[0]+nspd[0])>>1);
    posy = model->y + ((spd[1]+nspd[1])>>1);
    posz = model->z + ((spd[2]+nspd[2])>>1);
    // save position before movement
    int bposx,bposy,bposz;
    bposx = model->x;
    bposy = model->y;
    bposz = model->z;
    //write back updates
    //should be write back the updated position?
    //or only the speed?
    model->x =posx;
    model->y =posy;
    model->z =posz;
    pointer->xspeed=nspd[0];
    pointer->yspeed=nspd[1];
    pointer->zspeed=nspd[2];

    // Gravity and movement have been applied, time to check collisions...

    //below code should update the friction coefficient depending on
    //whether we're in contact with a surface
    //right now it doesnt
    //probably should be tested with a sloped surface
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
                int temp = (pointer->keptpercent << 12) / 100; // f32 format
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
}

bool NE_PhysicsCheckCollision(const NE_Physics *pointer1,
                              const NE_Physics *pointer2)
{
    NE_AssertPointer(pointer1, "NULL pointer 1");
    NE_AssertPointer(pointer2, "NULL pointer 2");
    NE_Assert(pointer1 != pointer2, "Both objects are the same one");

    // Get coordinates
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
