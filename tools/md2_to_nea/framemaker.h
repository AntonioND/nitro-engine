// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2008-2011, 2019, 2022, Antonio Niño Díaz

#ifndef FRAMEMAKER_H__
#define FRAMEMAKER_H__

#include <stdint.h>

#define DEFAULT_FRAME_SIZE (512 * 1024)

void NewFrame(void);
void NewFrameData(uint16_t data);

unsigned int GetFrameSize(int num);
uint16_t *GetFramePointer(int num);

void InitDynamicLists(void);
void EndDynamicLists(void);

int AddVertex(uint16_t x, uint16_t y, uint16_t z);
int AddNormal(uint16_t x, uint16_t y, uint16_t z);
int AddTexCoord(uint16_t u, uint16_t v);

int GetVerticesNumber(void);
int GetNormalNumber(void);
int GetTexcoordsNumber(void);

void GetVertex(int index, uint16_t *x, uint16_t *y, uint16_t *z);
void GetNormal(int index, uint16_t *x, uint16_t *y, uint16_t *z);
void GetTexCoord(int index, uint16_t *u, uint16_t *v);

#endif // FRAMEMAKER_H__
