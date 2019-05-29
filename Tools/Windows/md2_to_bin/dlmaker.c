// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2008-2011, 2019, Antonio Niño Díaz

#include <malloc.h>
#include "dlmaker.h"
#include "nds.h"

unsigned int DLSize[1024];
unsigned int * DLPointer[1024];
int DLcount = 0;

unsigned char command[4];
int commands;

unsigned int param[8];
int params;

void NewDL(void)
{
command[0] = command[1] = command[2] = command[3] = 0;
param[0] = param[1] = param[2] = param[3] = 0;
param[4] = param[5] = param[6] = param[7] = 0;
commands = 0; params = 0;

DLPointer[DLcount] = (unsigned int*)malloc(DEFAULT_DL_SIZE);
DLSize[DLcount] = 1;
}

void NewCommandDL(int id)
{
command[commands] = id;
commands++;

if(commands == 4) //Save data to display list
	{
	commands = 0; 
	unsigned int temp;
	temp = COMMAND_PACK(command[0],command[1],command[2],command[3]);
	command[0] = command[1] = command[2] = command[3] = 0;
	unsigned int * pointer = &((DLPointer[DLcount])[DLSize[DLcount]]);
	*pointer = temp; //Save commands
	DLSize[DLcount] ++;
	if(param > 0)
		{
		pointer = &((DLPointer[DLcount])[DLSize[DLcount]]);
		int a;
		for(a = 0; a < params; a ++)
			{
			pointer[a] = param[a]; //Save commands
			DLSize[DLcount] ++;
			}
		}
	param[0] = param[1] = param[2] = param[3] = 0;
	param[4] = param[5] = param[6] = param[7] = 0;
	params = 0;

	if(DLSize[DLcount] >= DEFAULT_DL_SIZE)
		{
		printf("\n\nDisplay list buffer overflow.\n\n");
		while(1); //TODO: Exit in a better way.
		}
	}
}

void NewParamDL(unsigned int param_)
{
param[params] = param_;
params++;
}

void FinishDL(void)
{
if(commands > 0) //Add NOP commands to fill packed commands
	{
	while(1)
		{
		NewCommandDL(ID_NOP);
		if(commands == 0) break;
		}
	}
*DLPointer[DLcount] = DLSize[DLcount] - 1; //DL real size in 4 bytes packs
DLcount++;
}

int GetDLSize(int num)
{
return DLSize[num];
}

unsigned int * GetDLPointer(int num)
{
return DLPointer[num];
}

