// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2022 Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include <nds/arm9/postest.h>

#include "NEMain.h"

/// @file NEAnimation.c

static NE_Animation **NE_AnimationPointers;
static int NE_MAX_ANIMATIONS;
static bool ne_animation_system_inited = false;

NE_Animation *NE_AnimationCreate(void)
{
    if (!ne_animation_system_inited)
    {
        NE_DebugPrint("System not initialized");
        return NULL;
    }

    NE_Animation *animation = calloc(1, sizeof(NE_Animation));
    if (animation == NULL)
    {
        NE_DebugPrint("Not enough memory");
        return NULL;
    }

    int i = 0;
    while (1)
    {
        if (NE_AnimationPointers[i] == NULL)
        {
            NE_AnimationPointers[i] = animation;
            break;
        }
        i++;
        if (i == NE_MAX_ANIMATIONS)
        {
            NE_DebugPrint("No free slots");
            free(animation);
            return NULL;
        }
    }

    return animation;
}

void NE_AnimationDelete(NE_Animation *animation)
{
    if (!ne_animation_system_inited)
        return;

    NE_AssertPointer(animation, "NULL pointer");

    int i = 0;
    while (1)
    {
        if (i == NE_MAX_ANIMATIONS)
        {
            NE_DebugPrint("Animation not found");
            return;
        }
        if (NE_AnimationPointers[i] == animation)
        {
            NE_AnimationPointers[i] = NULL;
            break;
        }
        i++;
    }

    if (animation->loadedfromfat)
        free((void *)animation->data);

    free(animation);
}

int NE_AnimationLoadFAT(NE_Animation *animation, const char *dsa_path)
{
    if (!ne_animation_system_inited)
        return 0;

    NE_AssertPointer(animation, "NULL animation pointer");
    NE_AssertPointer(dsa_path, "NULL path pointer");

    if (animation->loadedfromfat)
        free((void *)animation->data);

    animation->loadedfromfat = true;

    uint32_t *pointer = (uint32_t *)NE_FATLoadData(dsa_path);
    if (pointer == NULL)
    {
        NE_DebugPrint("Couldn't load file from FAT");
        return 0;
    }

    // Check version
    uint32_t version = pointer[0];
    if (version != 1)
    {
        NE_DebugPrint("file version is %ld, it should be 1", version);
        free(pointer);
        return 0;
    }

    animation->data = (void *)pointer;
    return 1;
}

int NE_AnimationLoad(NE_Animation *animation, const void *dsa_pointer)
{
    if (!ne_animation_system_inited)
        return 0;

    NE_AssertPointer(animation, "NULL animation pointer");
    NE_AssertPointer(dsa_pointer, "NULL data pointer");

    if (animation->loadedfromfat)
        free((void *)animation->data);

    animation->loadedfromfat = false;

    const u32 *pointer = dsa_pointer;

    // Check version
    uint32_t version = pointer[0];
    if (version != 1)
    {
        NE_DebugPrint("file version is %ld, it should be 1", version);
        free((void *)pointer);
        return 0;
    }

    animation->data = (void *)pointer;

    return 1;
}

void NE_AnimationDeleteAll(void)
{
    if (!ne_animation_system_inited)
        return;

    for (int i = 0; i < NE_MAX_ANIMATIONS; i++)
    {
        if (NE_AnimationPointers[i] != NULL)
            NE_AnimationDelete(NE_AnimationPointers[i]);
    }
}

int NE_AnimationSystemReset(int max_animations)
{
    if (ne_animation_system_inited)
        NE_AnimationSystemEnd();

    if (max_animations < 1)
        NE_MAX_ANIMATIONS = NE_DEFAULT_ANIMATIONS;
    else
        NE_MAX_ANIMATIONS = max_animations;

    NE_AnimationPointers = calloc(NE_MAX_ANIMATIONS, sizeof(NE_AnimationPointers));
    if (NE_AnimationPointers == NULL)
    {
        NE_DebugPrint("Not enough memory");
        return -1;
    }

    ne_animation_system_inited = true;
    return 0;
}

void NE_AnimationSystemEnd(void)
{
    if (!ne_animation_system_inited)
        return;

    NE_AnimationDeleteAll();

    free(NE_AnimationPointers);

    ne_animation_system_inited = false;
}
