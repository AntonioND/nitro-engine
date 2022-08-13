// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2008-2011, 2019, 2022, Antonio Niño Díaz

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dlmaker.h"
#include "nds.h"

uint32_t DLSize[1024];
uint32_t *DLPointer[1024];
int DLcount = 0;

uint8_t command[4];
int commands;

uint32_t param[8];
int params;

void NewDL(void)
{
    memset(command, 0, sizeof(command));
    memset(param, 0, sizeof(param));
    commands = 0;
    params = 0;

    DLPointer[DLcount] = (uint32_t *)malloc(DEFAULT_DL_SIZE);
    DLSize[DLcount] = 1;
}

void NewCommandDL(int id)
{
    command[commands] = id;
    commands++;

    if (commands == 4) {
        // Save data to display list
        commands = 0;
        uint32_t temp;
        temp =
            COMMAND_PACK(command[0], command[1], command[2],
                 command[3]);
        command[0] = command[1] = command[2] = command[3] = 0;
        uint32_t *pointer =
            &((DLPointer[DLcount])[DLSize[DLcount]]);
        // Save commands
        *pointer = temp;
        DLSize[DLcount]++;
        if (param > 0) {
            pointer = &((DLPointer[DLcount])[DLSize[DLcount]]);
            int a;
            for (a = 0; a < params; a++) {
                // Save commands
                pointer[a] = param[a];
                DLSize[DLcount]++;
            }
        }
        param[0] = param[1] = param[2] = param[3] = 0;
        param[4] = param[5] = param[6] = param[7] = 0;
        params = 0;

        if (DLSize[DLcount] >= DEFAULT_DL_SIZE) {
            printf("\n\nDisplay list buffer overflow.\n\n");
            // TODO: Exit in a better way.
            while (1) ;
        }
    }
}

void NewParamDL(uint32_t param_)
{
    param[params] = param_;
    params++;
}

void FinishDL(void)
{
    if (commands > 0) {
        // Add NOP commands to fill packed commands
        while (1) {
            NewCommandDL(ID_NOP);
            if (commands == 0)
                break;
        }
    }

    // Save DL real size in 4 bytes packs
    *DLPointer[DLcount] = DLSize[DLcount] - 1;
    DLcount++;
}

int GetDLSize(int num)
{
    return DLSize[num];
}

uint32_t *GetDLPointer(int num)
{
    return DLPointer[num];
}
