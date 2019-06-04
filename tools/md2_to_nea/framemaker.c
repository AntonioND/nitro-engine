// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2008-2011, 2019, Antonio Niño Díaz

#include <stdio.h>
#include <stdlib.h>

#include "dynamic_list.h"
#include "framemaker.h"

DinamicList *vertices_list;
DinamicList *normal_list;
DinamicList *texcoords_list;

unsigned int FrameSize[1024];
unsigned short *FramePointer[1024];
int framecount = -1; // ------.
//                            |
void NewFrame(void) //        |
{ //                          |
	framecount++; // <----'  For first frame

	FramePointer[framecount] = (unsigned short *)malloc(DEFAULT_FRAME_SIZE);
	if (FramePointer[framecount] == NULL) {
		printf("\n\nCouldn't allocate memory.\n\n");
		while (1) ;	//TODO: Exit in a better way.
	}
	FrameSize[framecount] = 0;
}

void NewFrameData(unsigned short data)
{
	(FramePointer[framecount])[FrameSize[framecount]] = data;
	FrameSize[framecount]++;

	if (FrameSize[framecount] >= DEFAULT_FRAME_SIZE) {
		printf("\n\nFrame commands buffer overflow.\n\n");
		while (1) ;	//TODO: Exit in a better way.
	}
}

unsigned int GetFrameSize(int num)
{
	return FrameSize[num];
}

unsigned short *GetFramePointer(int num)
{
	return FramePointer[num];
}

void InitDynamicLists(void)
{
	DynamicListNew(&vertices_list);
	DynamicListNew(&normal_list);
	DynamicListNew(&texcoords_list);
}

void EndDynamicLists(void)
{
	DynamicListDelete(vertices_list);
	DynamicListDelete(normal_list);
	DynamicListDelete(texcoords_list);
}

int AddVertex(unsigned short x, unsigned short y, unsigned short z)
{
	long long value =
	    ((long long)x << 32) | ((long long)y << 16) | (long long)z;

	int result = DynamicListGetIndex(vertices_list, value);

	if (result != -1)
		return result;

	int index = DynamicListNewElement(vertices_list);

	DynamicListElementSet(vertices_list, index, value);

	return index;
}

void GetVertex(int index, unsigned short *x, unsigned short *y,
	       unsigned short *z)
{
	unsigned long long temp = DynamicListElementGet(vertices_list, index);
	*x = (unsigned short)((temp >> 32) & 0xFFFF);
	*y = (unsigned short)((temp >> 16) & 0xFFFF);
	*z = (unsigned short)(temp & 0xFFFF);
}

int AddNormal(unsigned short x, unsigned short y, unsigned short z)
{
	long long value =
	    ((long long)x << 32) | ((long long)y << 16) | (long long)z;

	int result = DynamicListGetIndex(normal_list, value);

	if (result != -1)
		return result;

	int index = DynamicListNewElement(normal_list);

	DynamicListElementSet(normal_list, index, value);

	return index;
}

void GetNormal(int index, unsigned short *x, unsigned short *y,
	       unsigned short *z)
{
	unsigned long long temp = DynamicListElementGet(normal_list, index);
	*x = (unsigned short)((temp >> 32) & 0xFFFF);
	*y = (unsigned short)((temp >> 16) & 0xFFFF);
	*z = (unsigned short)(temp & 0xFFFF);
}

int AddTexCoord(unsigned short u, unsigned short v)
{
	long long value = ((long long)u << 16) | (long long)v;

	int result = DynamicListGetIndex(texcoords_list, value);

	if (result != -1)
		return result;

	int index = DynamicListNewElement(texcoords_list);

	DynamicListElementSet(texcoords_list, index, value);

	return index;
}

void GetTexCoord(int index, unsigned short *u, unsigned short *v)
{
	unsigned long long temp = DynamicListElementGet(texcoords_list, index);
	*u = (unsigned short)((temp >> 16) & 0xFFFF);
	*v = (unsigned short)(temp & 0xFFFF);
}

int GetVerticesNumber(void)
{
	return DynamicListLenghtGet(vertices_list);
}

int GetNormalNumber(void)
{
	return DynamicListLenghtGet(normal_list);
}

int GetTexcoordsNumber(void)
{
	return DynamicListLenghtGet(texcoords_list);
}
