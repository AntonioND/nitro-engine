// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2008-2011, 2019, 2022, Antonio Niño Díaz

#ifndef DLMAKER_H__
#define DLMAKER_H__

#include <stdint.h>

#define DEFAULT_DL_SIZE (512 * 1024)

void NewDL(void);
void NewCommandDL(int id);
void NewParamDL(uint32_t param_);
void FinishDL(void);

int GetDLSize(int num);
uint32_t *GetDLPointer(int num);

#endif // DLMAKER_H__
