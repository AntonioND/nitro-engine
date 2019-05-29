// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2008-2011, 2019, Antonio Niño Díaz

#ifndef _DLMAKER_H_
#define _DLMAKER_H_

#define DEFAULT_DL_SIZE (512*1024)

void NewDL(void);
void NewCommandDL(int id);
void NewParamDL(unsigned int param_);
void FinishDL(void);

int GetDLSize(int num);
unsigned int * GetDLPointer(int num);

#endif
