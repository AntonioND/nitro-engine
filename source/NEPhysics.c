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

#include "NEMain.h"

/*! \file   NEPhysics.c */

static NE_Physics ** NE_PhysicsPointers;
static bool ne_physics_system_inited = false;

static int NE_MAX_PHYSICS;

NE_Physics * NE_PhysicsCreate(NE_PhysicsTypes type)
{
	if(!ne_physics_system_inited) return NULL;

	//TODO:
	if(type == NE_BoundingSphere) 
	{
		NE_DebugPrint("Bounding spheres not yet supported!");
		return NULL;
	}
	if(type == NE_Dot) 
	{
		NE_DebugPrint("Dots not yet supported!");
		return NULL;
	}

	NE_Physics * temp = (NE_Physics*)malloc(sizeof(NE_Physics));
	NE_AssertPointer(temp,"NE_PhysicsCreate: Couldn't allocate physics object.");

	int a = 0;
	while(1)
	{
		if(a == NE_MAX_PHYSICS)
		{
			free(temp);
			NE_DebugPrint("NE_PhysicsCreate: No free slots...");
			return NULL;
		}
		if(NE_PhysicsPointers[a] == NULL) 
		{
			NE_PhysicsPointers[a] = temp;
			break;
		}
		a++;
	}

	temp->type = type;
	temp->xspeed = temp->yspeed = temp->zspeed = temp->xsize = temp->ysize = temp->zsize = 0;
	temp->radius = temp->gravity = temp->friction = 0;
	temp->keptpercent = 50;
	temp->enabled = true;
	temp->physicsgroup = 0;
	temp->iscolliding = false;
	temp->oncollition = NE_ColNothing;

	return temp;
}


void NE_PhysicsDelete(NE_Physics * pointer)
{
	if(!ne_physics_system_inited) return;

	NE_AssertPointer(pointer,"NE_PhysicsDelete: NULL pointer.");

	int a = 0;
	while(1)
	{
		if(a == NE_MAX_PHYSICS) break;
		if(NE_PhysicsPointers[a] == pointer) 
		{
			NE_PhysicsPointers[a] = NULL;
			free(pointer);
			break;
		}
		a++;
	}
	
	NE_DebugPrint("NE_PhysicsDelete: Pointer not found in array.");

	return;
}


void NE_PhysicsDeleteAll(void)
{
	if(!ne_physics_system_inited) return;
	int a;
	for(a = 0; a < NE_MAX_PHYSICS; a++) NE_PhysicsDelete(NE_PhysicsPointers[a]);
}



void NE_PhysicsSystemReset(int number_of_objects)
{
	if(ne_physics_system_inited) NE_PhysicsSystemEnd();

	if(number_of_objects < 1) NE_MAX_PHYSICS = NE_DEFAULT_PHYSICS;
	else NE_MAX_PHYSICS = number_of_objects;

	NE_PhysicsPointers = malloc(NE_MAX_PHYSICS * sizeof(NE_PhysicsPointers));
	NE_AssertPointer(NE_PhysicsPointers,"NE_PhysicsSystemReset: Not enough memory to allocate array.");

	int a;
	for(a = 0; a < NE_MAX_PHYSICS; a++)  NE_PhysicsPointers[a] = NULL;

	ne_physics_system_inited = true;
}

void NE_PhysicsSystemEnd(void)
{
	if(!ne_physics_system_inited) return;

	NE_PhysicsDeleteAll();

	free(NE_PhysicsPointers);

	ne_physics_system_inited = false;
}

inline void NE_PhysicsSetRadiusI(NE_Physics * pointer, int radius)
{
	NE_AssertPointer(pointer,"NE_PhysicsSetRadiusI: NULL pointer.");
	NE_Assert(pointer->type == NE_BoundingSphere,"NE_PhysicsSetRadiusI: Only bounding shperes allowed.");
	NE_Assert(radius >= 0,"NE_PhysicsSetRadiusI: Radius must be positive!!");
	pointer->radius = radius;
}


inline void NE_PhysicsSetSpeedI(NE_Physics * pointer, int x, int y, int z)
{
	NE_AssertPointer(pointer,"NE_PhysicsSetSpeedI: NULL pointer.");
	pointer->xspeed = x; pointer->yspeed = y; pointer->zspeed = z;
}


inline void NE_PhysicsSetSizeI(NE_Physics * pointer, int x, int y, int z)
{
	NE_AssertPointer(pointer,"NE_PhysicsSetSizeI: NULL pointer.");
	NE_Assert(pointer->type == NE_BoundingBox,"NE_PhysicsSetSizeI: Only bounding boxes allowed.");
	NE_Assert(x >= 0 && y >= 0 && z >= 0,"NE_PhysicsSetSizeI: Size must be positive!!");
	pointer->xsize = x; pointer->ysize = y; pointer->zsize = z;
}


inline void NE_PhysicsSetGravityI(NE_Physics * pointer, int gravity)
{
	NE_AssertPointer(pointer,"NE_PhysicsSetGravityI: NULL pointer.");
	pointer->gravity = gravity;
}


inline void NE_PhysicsSetFrictionI(NE_Physics * pointer, int friction)
{
	NE_AssertPointer(pointer,"NE_PhysicsSetFrictionI: NULL pointer.");
	NE_Assert(friction >= 0,"NE_PhysicsSetFrictionI: Friction mustn't be lower than 0.");
	pointer->friction = friction;
}


inline void NE_PhysicsSetBounceEnergy(NE_Physics * pointer, int percent)
{
	NE_AssertPointer(pointer,"NE_PhysicsSetBounceEnergy: NULL pointer.");
	NE_Assert(percent >= 0,"NE_PhysicsSetBounceEnergy: Percent must be higher than 0.");
	pointer->keptpercent = percent;
}

inline void NE_PhysicsEnable(NE_Physics * pointer, bool value)
{
	NE_AssertPointer(pointer,"NE_PhysicsEnable: NULL pointer.");
	pointer->enabled = value;
}

inline void NE_PhysicsSetModel(NE_Physics * physics, void * modelpointer)
{
	NE_AssertPointer(physics,"NE_PhysicsSetModel: NULL physics pointer.");
	NE_AssertPointer(modelpointer,"NE_PhysicsSetModel: NULL model pointer.");
	physics->model = modelpointer;
}

inline void NE_PhysicsSetGroup(NE_Physics * physics, int group)
{
	NE_AssertPointer(physics,"NE_PhysicsSetGroup: NULL pointer.");
	physics->physicsgroup = group;
}


inline void NE_PhysicsOnCollition(NE_Physics * physics, NE_OnCollition action)
{
	NE_AssertPointer(physics,"NE_PhysicsOnCollition: NULL pointer.");
	physics->oncollition = action;
}

inline bool NE_PhysicsIsColliding(NE_Physics * pointer)
{
	NE_AssertPointer(pointer,"NE_PhysicsIsColliding: NULL pointer.");
	return pointer->iscolliding;
}

void NE_PhysicsUpdateAll(void)
{
	if(!ne_physics_system_inited) return;
	int a;
	for(a = 0; a < NE_MAX_PHYSICS; a++) if(NE_PhysicsPointers[a] != NULL)
		NE_PhysicsUpdate(NE_PhysicsPointers[a]);
}



void NE_PhysicsUpdate(NE_Physics * pointer)
{
	if(!ne_physics_system_inited) return;

	NE_AssertPointer(pointer,"NE_PhysicsUpdate: NULL pointer.");
	NE_AssertPointer(pointer->model,"NE_PhysicsUpdate: NULL model pointer.");
	NE_Assert(pointer->type != 0,"NE_PhysicsUpdate: No object type selected.");

	if(pointer->enabled == false) return;

	pointer->iscolliding = false;

	//We change Y speed depending on gravity.
	pointer->yspeed -= pointer->gravity; //EASY :P

	//Now, let's move the object
	int posx = 0, posy = 0, posz = 0; //Will use this in collition checking (to simplify code).
	int bposx = 0, bposy = 0, bposz = 0; //Position before movement

	NE_Model * model = pointer->model;
	bposx = model->x; bposy = model->y; bposz = model->z; 
	posx = model->x = model->x + pointer->xspeed;
	posy = model->y = model->y + pointer->yspeed;
	posz = model->z = model->z + pointer->zspeed;

	//Gravity and movement have been applied, time to check collitions...
	bool xenabled = true, yenabled = true, zenabled = true; //To check some things
	if (bposx == posx) xenabled = false;
	if (bposy == posy) yenabled = false;
	if (bposz == posz) zenabled = false;
	//int xlastcol = 0, ylastcol = 0, zlastcol = 0;  //To fix other things...
	int a;
	for(a = 0; a < NE_MAX_PHYSICS; a++) if(NE_PhysicsPointers[a] != NULL && //If exists
	NE_PhysicsPointers[a] != pointer && //and is NOT the same object
	NE_PhysicsPointers[a]->physicsgroup == pointer->physicsgroup) //and is same group...
	{
		NE_Physics * otherpointer = NE_PhysicsPointers[a];
		//Get coordinates
		int otherposx = 0, otherposy = 0, otherposz = 0;

		model = otherpointer->model;
		otherposx = model->x; otherposy = model->y; otherposz = model->z;
	
		//Both are boxes
//		if(pointer->type == NE_BoundingBox && otherpointer->type == NE_BoundingBox)
//		{
			//If collition...
			if((abs(posx - otherposx) < (pointer->xsize + otherpointer->xsize) >> 1) &&
			   (abs(posy - otherposy) < (pointer->ysize + otherpointer->ysize) >> 1) &&
			   (abs(posz - otherposz) < (pointer->zsize + otherpointer->zsize) >> 1))
			{
				pointer->iscolliding = true;
				
				if(pointer->oncollition == NE_ColBounce)
				{
					//Used to reduce speed:
					int temp = divf32(inttof32(pointer->keptpercent),inttof32(100));
					if((yenabled) && ((abs(bposy - otherposy) >= (pointer->ysize + otherpointer->ysize) >> 1)))
					{
						yenabled = false;
						pointer->yspeed += pointer->gravity;
						
						if(posy > otherposy) (pointer->model)->y = 
								otherposy + ((pointer->ysize + otherpointer->ysize) >> 1);
						if(posy < otherposy) (pointer->model)->y = 
								otherposy - ((pointer->ysize + otherpointer->ysize) >> 1);
							
						if(pointer->gravity == 0) pointer->yspeed = -mulf32(temp,pointer->yspeed);
						else 
						{
							if(abs(pointer->yspeed) > NE_MIN_BOUNCE_SPEED) //UGLY FIX
							{
								//pointer->yspeed = -mulf32(temp,pointer->yspeed);
								/*
								if(pointer->yspeed > 0) 
									{
									pointer->yspeed = -mulf32(temp,pointer->yspeed);
									//pointer->yspeed += pointer->gravity;
									}
								else
									pointer->yspeed = -mulf32(temp,pointer->yspeed-pointer->gravity);
								*/	
								pointer->yspeed = -mulf32(temp,pointer->yspeed-pointer->gravity);
							}
							else pointer->yspeed = 0;
						}
					}
					else if((xenabled) && ((abs(bposx - otherposx) >= (pointer->xsize + otherpointer->xsize) >> 1)))
					{
						xenabled = false;
						
						if(posx > otherposx) (pointer->model)->x = 
								otherposx + ((pointer->xsize + otherpointer->xsize) >> 1);
						if(posx < otherposx) (pointer->model)->x = 
								otherposx - ((pointer->xsize + otherpointer->xsize) >> 1);
								
						pointer->xspeed = -mulf32(temp,pointer->xspeed);
					}
					else if((zenabled) && ((abs(bposz - otherposz) >= (pointer->zsize + otherpointer->zsize) >> 1)))
					{
						zenabled = false;
						
						if(posz > otherposz) (pointer->model)->z = 
								otherposz + ((pointer->zsize + otherpointer->zsize) >> 1);
						if(posz < otherposz) (pointer->model)->z = 
								otherposz - ((pointer->zsize + otherpointer->zsize) >> 1);
							
						pointer->zspeed = -mulf32(temp,pointer->zspeed);
					}
				}
				else if(pointer->oncollition == NE_ColStop)
				{
					if((yenabled) && ((abs(bposy - otherposy) >= (pointer->ysize + otherpointer->ysize) >> 1)))
					{
						yenabled = false;
						
						if(posy > otherposy) (pointer->model)->y = 
								otherposy + ((pointer->ysize + otherpointer->ysize) >> 1);
						if(posy < otherposy) (pointer->model)->y = 
								otherposy - ((pointer->ysize + otherpointer->ysize) >> 1);
					}
					if((xenabled) && ((abs(bposx - otherposx) >= (pointer->xsize + otherpointer->xsize) >> 1)))
					{
						xenabled = false;
						
						if(posx > otherposx) (pointer->model)->x = 
								otherposx + ((pointer->xsize + otherpointer->xsize) >> 1);
						if(posx < otherposx) (pointer->model)->x = 
								otherposx - ((pointer->xsize + otherpointer->xsize) >> 1);
					}
					if((zenabled) && ((abs(bposz - otherposz) >= (pointer->zsize + otherpointer->zsize) >> 1)))
					{
						zenabled = false;
						
						if(posz > otherposz) (pointer->model)->z = 
								otherposz + ((pointer->zsize + otherpointer->zsize) >> 1);
						if(posz < otherposz) (pointer->model)->z = 
								otherposz - ((pointer->zsize + otherpointer->zsize) >> 1);
					}
					pointer->xspeed = pointer->yspeed = pointer->zspeed = 0;
				}
			}
//		}
/*		//Both spheres
		else if(pointer->type == NE_BoundingSphere && otherpointer->type == NE_BoundingSphere)
		{
		
		}
		//Box - sphere
		else if(pointer->type == NE_BoundingBox && otherpointer->type == NE_BoundingSphere)
		{
				int radius; //for shpere
		}
		//Sphere - box
		else if(pointer->type == NE_BoundingSphere && otherpointer->type == NE_BoundingBox)
		{
		
		}*/
	}

	//Now, we get the module of speed in order to apply friction.
	if(pointer->friction != 0)
	{
		pointer->xspeed <<= 10; pointer->yspeed <<= 10; pointer->zspeed <<= 10;
		int _mod_ = mulf32(pointer->xspeed, pointer->xspeed);
		_mod_ += mulf32(pointer->yspeed, pointer->yspeed);
		_mod_ += mulf32(pointer->zspeed, pointer->zspeed);
		_mod_ = sqrtf32(_mod_); //THANKS, HARDWARE ACCELERATION XD
		
		//check if module is very small -> speed = 0
		if(_mod_ < pointer->friction)
		{
			pointer->xspeed = pointer->yspeed = pointer->zspeed = 0;
		}
		else
		{
			int newmod = _mod_ - pointer->friction;
			// mod   --  newmod    ->  newspeed = speed * newmod / mod
			// speed --  newspeed
			int number = divf32(newmod,_mod_);
			pointer->xspeed = mulf32(pointer->xspeed, number);
			pointer->yspeed = mulf32(pointer->yspeed, number);
			pointer->zspeed = mulf32(pointer->zspeed, number);
			pointer->xspeed >>= 10; pointer->yspeed >>= 10; pointer->zspeed >>= 10;
		}
	}
}


bool NE_PhysicsCheckCollition(NE_Physics * pointer1, NE_Physics * pointer2)
{
	NE_AssertPointer(pointer1,"NE_PhysicsCheckCollition: NULL pointer 1.");
	NE_AssertPointer(pointer2,"NE_PhysicsCheckCollition: NULL pointer 2.");
	NE_Assert(pointer1 != pointer2,"NE_PhysicsCheckCollition: Both objects are the same!");

	//Get coordinates
	int posx = 0, posy = 0, posz = 0;

	NE_Model * model = pointer1->model;
	posx = model->x; posy = model->y; posz = model->z; 


	int otherposx = 0, otherposy = 0, otherposz = 0;

	model = pointer2->model;
	otherposx = model->x; otherposy = model->y; otherposz = model->z;

		
//Both are boxes
//	if(pointer->type == NE_BoundingBox && otherpointer->type == NE_BoundingBox)
//	{
		if((abs(posx - otherposx) < (pointer1->xsize + pointer2->xsize) >> 1) &&
		   (abs(posy - otherposy) < (pointer1->ysize + pointer2->ysize) >> 1) &&
		   (abs(posz - otherposz) < (pointer1->zsize + pointer2->zsize) >> 1))
		{
			return true;
		}
/*	}
	else if...         TODO: SPHERES */

	return false;
}



