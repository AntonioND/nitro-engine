// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2008-2011, 2019, 2022, Antonio Niño Díaz

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "dynamic_list.h"
#include "framemaker.h"

DinamicList *vertices_list;
DinamicList *normal_list;
DinamicList *texcoords_list;

uint32_t FrameSize[1024];
uint16_t *FramePointer[1024];
int framecount = -1; // ------.
//                            |
void NewFrame(void) //        |
{ //                          |
	framecount++; // <----'  For first frame

	FramePointer[framecount] = (uint16_t *)malloc(DEFAULT_FRAME_SIZE);
	if (FramePointer[framecount] == NULL) {
		printf("\n\nCouldn't allocate memory.\n\n");
		while (1) ;	//TODO: Exit in a better way.
	}
	FrameSize[framecount] = 0;
}

void NewFrameData(uint16_t data)
{
	(FramePointer[framecount])[FrameSize[framecount]] = data;
	FrameSize[framecount]++;

	if (FrameSize[framecount] >= DEFAULT_FRAME_SIZE) {
		printf("\n\nFrame commands buffer overflow.\n\n");
		while (1) ;	//TODO: Exit in a better way.
	}
}

uint32_t GetFrameSize(int num)
{
	return FrameSize[num];
}

uint16_t *GetFramePointer(int num)
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

int AddVertex(uint16_t x, uint16_t y, uint16_t z)
{
	int64_t value = ((int64_t)x << 32) | ((int64_t)y << 16) | (int64_t)z;

	int result = DynamicListGetIndex(vertices_list, value);

	if (result != -1)
		return result;

	int index = DynamicListNewElement(vertices_list);

	DynamicListElementSet(vertices_list, index, value);

	return index;
}

void GetVertex(int index, uint16_t *x, uint16_t *y, uint16_t *z)
{
	uint64_t temp = DynamicListElementGet(vertices_list, index);
	*x = (uint16_t)((temp >> 32) & 0xFFFF);
	*y = (uint16_t)((temp >> 16) & 0xFFFF);
	*z = (uint16_t)(temp & 0xFFFF);
}

int AddNormal(uint16_t x, uint16_t y, uint16_t z)
{
	uint64_t value = ((uint64_t)x << 32) | ((uint64_t)y << 16) | (uint64_t)z;

	int result = DynamicListGetIndex(normal_list, value);

	if (result != -1)
		return result;

	int index = DynamicListNewElement(normal_list);

	DynamicListElementSet(normal_list, index, value);

	return index;
}

void GetNormal(int index, uint16_t *x, uint16_t *y, uint16_t *z)
{
	uint64_t temp = DynamicListElementGet(normal_list, index);
	*x = (uint16_t)((temp >> 32) & 0xFFFF);
	*y = (uint16_t)((temp >> 16) & 0xFFFF);
	*z = (uint16_t)(temp & 0xFFFF);
}

int AddTexCoord(uint16_t u, uint16_t v)
{
	int32_t value = ((int32_t)u << 16) | (int32_t)v;

	int result = DynamicListGetIndex(texcoords_list, value);

	if (result != -1)
		return result;

	int index = DynamicListNewElement(texcoords_list);

	DynamicListElementSet(texcoords_list, index, value);

	return index;
}

void GetTexCoord(int index, uint16_t *u, uint16_t *v)
{
	uint32_t temp = DynamicListElementGet(texcoords_list, index);
	*u = (uint16_t)((temp >> 16) & 0xFFFF);
	*v = (uint16_t)(temp & 0xFFFF);
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
