// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, 2022 Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include <nds/arm9/postest.h>

#include "dsma/dsma.h"

#include "NEMain.h"

/// @file NEModel.c

static NE_Model **NE_ModelPointers;
static int NE_MAX_MODELS;
static bool ne_model_system_inited = false;

NE_Model *NE_ModelCreate(NE_ModelType type)
{
    if (!ne_model_system_inited)
        return NULL;

    NE_Model *model = calloc(1, sizeof(NE_Model));
    NE_AssertPointer(model, "Not enough memory");

    int i = 0;
    while (1)
    {
        if (NE_ModelPointers[i] == NULL)
        {
            NE_ModelPointers[i] = model;
            break;
        }
        i++;
        if (i == NE_MAX_MODELS)
        {
            NE_DebugPrint("No free slots");
            free(model);
            return NULL;
        }
    }

    model->sx = model->sy = model->sz = inttof32(1);

    model->modeltype = type;
    model->meshdata = NULL;

    if (type == NE_Animated)
    {
        model->animinfo = calloc(sizeof(NE_AnimInfo), 1);
        NE_AssertPointer(model->animinfo, "Couldn't allocate animation info");
    }

    return model;
}

void NE_ModelDelete(NE_Model *model)
{
    if (!ne_model_system_inited)
        return;

    NE_AssertPointer(model, "NULL pointer");

    int i = 0;
    while (1)
    {
        if (i == NE_MAX_MODELS)
        {
            NE_DebugPrint("Model not found");
            return;
        }
        if (NE_ModelPointers[i] == model)
        {
            NE_ModelPointers[i] = NULL;
            break;
        }
        i++;
    }

    if (!model->iscloned && model->meshfromfat)
        free((void *)model->meshdata);

    if (type == NE_Animated)
        free(model->animinfo);

    free(model);
}

int NE_ModelLoadStaticMeshFAT(NE_Model *model, const char *path)
{
    if (!ne_model_system_inited)
        return 0;

    NE_AssertPointer(model, "NULL model pointer");
    NE_AssertPointer(path, "NULL path pointer");
    NE_Assert(model->modeltype == NE_Static, "Not a static model");
    NE_Assert(!model->iscloned, "Can't load a mesh to a cloned model");

    // Free previous data...
    if (model->meshfromfat && model->meshdata != NULL)
    {
        free((void *)model->meshdata);
        model->meshdata = NULL;
    }

    model->meshdata = (u32 *) NE_FATLoadData(path);
    NE_AssertPointer(model->meshdata, "Couldn't load file from FAT");

    if (model->meshdata == NULL)
        return 0;

    model->meshfromfat = true;

    return 1;
}

int NE_ModelLoadStaticMesh(NE_Model *model, const void *pointer)
{
    if (!ne_model_system_inited)
        return 0;

    NE_AssertPointer(model, "NULL model pointer");
    NE_AssertPointer(pointer, "NULL data pointer");
    NE_Assert(model->modeltype == NE_Static, "Not a static model");
    NE_Assert(!model->iscloned, "Can't load a mesh to a cloned model");

    // Free previous data...
    if (model->meshfromfat && model->meshdata != NULL)
    {
        free((void *)model->meshdata);
        model->meshdata = NULL;
    }

    model->meshdata = pointer;
    model->meshfromfat = false;
    return 1;
}

void NE_ModelSetMaterial(NE_Model *model, NE_Material *material)
{
    NE_AssertPointer(model, "NULL model pointer");
    NE_AssertPointer(material, "NULL material pointer");
    model->texture = material;
}

void NE_ModelSetAnimation(NE_Model *model, NE_Animation *anim)
{
    NE_AssertPointer(model, "NULL model pointer");
    NE_AssertPointer(anim, "NULL animation pointer");
    NE_Assert(model->modeltype == NE_Animated, "Not an animated model");
    model->animinfo->animation = anim;
    uint32_t frames = DSMA_GetNumFrames(anim->data);
    model->animinfo->numframes = frames;
}

//---------------------------------------------------------

// Internal use... see below
extern bool NE_TestTouch;

void NE_ModelDraw(NE_Model *model)
{
    NE_AssertPointer(model, "NULL pointer");
    if (model->meshdata == NULL)
        return;

    if (model->modeltype == NE_Animated)
    {
        if (model->animinfo->animation == NULL)
            return;
    }

    MATRIX_PUSH = 0;

    MATRIX_TRANSLATE = model->x;
    MATRIX_TRANSLATE = model->y;
    MATRIX_TRANSLATE = model->z;

    if (model->rx != 0)
        glRotateXi(model->rx << 6);
    if (model->ry != 0)
        glRotateYi(model->ry << 6);
    if (model->rz != 0)
        glRotateZi(model->rz << 6);

    MATRIX_SCALE = model->sx;
    MATRIX_SCALE = model->sy;
    MATRIX_SCALE = model->sz;

    if (NE_TestTouch)
    {
        PosTest_Asynch(0, 0, 0);
    }
    else
    {
        // If the texture pointer is NULL, this will set GFX_TEX_FORMAT
        // to 0 and GFX_COLOR to white
        NE_MaterialUse(model->texture);
    }

    if (model->modeltype == NE_Static)
    {
        glCallList(model->meshdata);
    }
    else // if(model->modeltype == NE_Animated)
    {
        DSMA_DrawModel(model->meshdata, model->animinfo->animation->data,
                       model->animinfo->currframe);
    }

    MATRIX_POP = 1;
}

void NE_ModelClone(NE_Model *dest, NE_Model *source)
{
    NE_AssertPointer(dest, "NULL dest pointer");
    NE_AssertPointer(source, "NULL source pointer");
    NE_Assert(dest->modeltype == source->modeltype,
              "Different model types");

    if (dest->modeltype == NE_Animated)
    {
        memcpy(dest->animinfo, source->animinfo, sizeof(NE_AnimInfo));
        dest->iscloned = true;
        dest->texture = source->texture;
    }
    else
    {
        dest->iscloned = true;
        dest->meshdata = source->meshdata;
        dest->texture = source->texture;
    }
}

void NE_ModelScaleI(NE_Model *model, int x, int y, int z)
{
    NE_AssertPointer(model, "NULL pointer");
    model->sx = x;
    model->sy = y;
    model->sz = z;
}

void NE_ModelTranslateI(NE_Model *model, int x, int y, int z)
{
    NE_AssertPointer(model, "NULL pointer");
    model->x += x;
    model->y += y;
    model->z += z;
}

void NE_ModelSetCoordI(NE_Model *model, int x, int y, int z)
{
    NE_AssertPointer(model, "NULL pointer");
    model->x = x;
    model->y = y;
    model->z = z;
}

void NE_ModelRotate(NE_Model *model, int rx, int ry, int rz)
{
    NE_AssertPointer(model, "NULL pointer");
    model->rx = (model->rx + rx + 512) & 0x1FF;
    model->ry = (model->ry + ry + 512) & 0x1FF;
    model->rz = (model->rz + rz + 512) & 0x1FF;
}

void NE_ModelSetRot(NE_Model *model, int rx, int ry, int rz)
{
    NE_AssertPointer(model, "NULL pointer");
    model->rx = rx;
    model->ry = ry;
    model->rz = rz;
}

void NE_ModelAnimateAll(void)
{
    if (!ne_model_system_inited)
        return;

    for (int i = 0; i < NE_MAX_MODELS; i++)
    {
        if (NE_ModelPointers[i] == NULL)
            continue;

        if (NE_ModelPointers[i]->modeltype != NE_Animated)
            continue;

        NE_AnimInfo *animinfo = NE_ModelPointers[i]->animinfo;

        animinfo->currframe += animinfo->speed;

        if (animinfo->type ==  NE_ANIM_LOOP)
        {
            int32_t endval = inttof32(animinfo->numframes);
            if (animinfo->currframe >= endval)
                animinfo->currframe -= endval;
            else if (animinfo->currframe < 0)
                animinfo->currframe += endval;
        }
        else if (animinfo->type ==  NE_ANIM_ONESHOT)
        {
            int32_t endval = inttof32(animinfo->numframes - 1);
            if (animinfo->currframe > endval)
            {
                animinfo->currframe = endval;
                animinfo->speed = 0;
            }
            else if (animinfo->currframe < 0)
            {
                animinfo->currframe = 0;
                animinfo->speed = 0;
            }
        }
    }
}

void NE_ModelAnimStart(NE_Model *model, NE_AnimationType type, int32_t speed)
{
    NE_AssertPointer(model, "NULL pointer");
    NE_Assert(model->modeltype == NE_Animated, "Not an animated model");
    model->animinfo->type = type;
    model->animinfo->speed = speed;
    model->animinfo->currframe = 0;
}

void NE_ModelAnimSetSpeed(NE_Model *model, int32_t speed)
{
    NE_AssertPointer(model, "NULL pointer");
    NE_Assert(model->modeltype == NE_Animated, "Not an animated model");
    model->animinfo->speed = speed;
}

int32_t NE_ModelAnimGetFrame(NE_Model *model)
{
    NE_AssertPointer(model, "NULL pointer");
    NE_Assert(model->modeltype == NE_Animated, "Not an animated model");
    return model->animinfo->currframe;
}

void NE_ModelAnimSetFrame(NE_Model *model, int32_t frame)
{
    NE_AssertPointer(model, "NULL pointer");
    NE_Assert(model->modeltype == NE_Animated, "Not an animated model");
    // TODO: Check if it is off bounds
    model->animinfo->currframe = frame;
}

int NE_ModelLoadDSMFAT(NE_Model *model, const char *path)
{
    if (!ne_model_system_inited)
        return 0;

    NE_AssertPointer(model, "NULL model pointer");
    NE_AssertPointer(path, "NULL path pointer");
    NE_Assert(model->modeltype == NE_Animated, "Not an animated model");

    if (model->meshfromfat)
        free((void *)model->meshdata);

    model->iscloned = 0;
    model->meshfromfat = true;

    void *pointer = (void *)NE_FATLoadData(path);
    NE_AssertPointer(pointer, "Couldn't load file from FAT");

    model->meshdata = (void *)pointer;

    return 1;
}

int NE_ModelLoadDSM(NE_Model *model, const void *pointer)
{
    if (!ne_model_system_inited)
        return 0;

    NE_AssertPointer(model, "NULL model pointer");
    NE_AssertPointer(pointer, "NULL data pointer");
    NE_Assert(model->modeltype == NE_Animated, "Not an animated model");

    if (model->meshfromfat)
        free((void *)model->meshdata);

    model->iscloned = 0;
    model->meshfromfat = false;
    model->meshdata = pointer;

    return 1;
}

void NE_ModelDeleteAll(void)
{
    if (!ne_model_system_inited)
        return;

    for (int i = 0; i < NE_MAX_MODELS; i++)
    {
        if (NE_ModelPointers[i] != NULL)
            NE_ModelDelete(NE_ModelPointers[i]);
    }
}

void NE_ModelSystemReset(int max_models)
{
    if (ne_model_system_inited)
        NE_ModelSystemEnd();

    if (max_models < 1)
        NE_MAX_MODELS = NE_DEFAULT_MODELS;
    else
        NE_MAX_MODELS = max_models;

    NE_ModelPointers = malloc(NE_MAX_MODELS * sizeof(NE_ModelPointers));
    NE_AssertPointer(NE_ModelPointers, "Not enough memory");

    for (int i = 0; i < NE_MAX_MODELS; i++)
        NE_ModelPointers[i] = NULL;

    ne_model_system_inited = true;
}

void NE_ModelSystemEnd(void)
{
    if (!ne_model_system_inited)
        return;

    NE_ModelDeleteAll();

    free(NE_ModelPointers);

    ne_model_system_inited = false;
}
