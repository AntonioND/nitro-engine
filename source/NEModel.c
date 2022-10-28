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
    model->free_mesh = false;

    if (type == NE_Animated)
    {
        for (int i = 0; i < 2; i++)
        {
            model->animinfo[i] = calloc(sizeof(NE_AnimInfo), 1);
            NE_AssertPointer(model->animinfo[i],
                             "Couldn't allocate animation info");
        }
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

    if (model->free_mesh)
        free((void *)model->meshdata);

    if (model->modeltype == NE_Animated)
    {
        for (int i = 0; i < 2; i++)
            free(model->animinfo[i]);
    }

    free(model);
}

int NE_ModelLoadStaticMeshFAT(NE_Model *model, const char *path)
{
    if (!ne_model_system_inited)
        return 0;

    NE_AssertPointer(model, "NULL model pointer");
    NE_AssertPointer(path, "NULL path pointer");
    NE_Assert(model->modeltype == NE_Static, "Not a static model");

    void *pointer = NE_FATLoadData(path);
    if (pointer == NULL)
        return 0;

    // Free previous data...
    if (model->free_mesh)
        free((void *)model->meshdata);

    model->meshdata = pointer;
    model->free_mesh = true;

    return 1;
}

int NE_ModelLoadStaticMesh(NE_Model *model, const void *pointer)
{
    if (!ne_model_system_inited)
        return 0;

    NE_AssertPointer(model, "NULL model pointer");
    NE_AssertPointer(pointer, "NULL data pointer");
    NE_Assert(model->modeltype == NE_Static, "Not a static model");

    // Free previous data...
    if (model->free_mesh)
        free((void *)model->meshdata);

    model->meshdata = pointer;
    model->free_mesh = false;

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
    model->animinfo[0]->animation = anim;
    uint32_t frames = DSMA_GetNumFrames(anim->data);
    model->animinfo[0]->numframes = frames;
}

void NE_ModelSetAnimationSecondary(NE_Model *model, NE_Animation *anim)
{
    NE_AssertPointer(model, "NULL model pointer");
    NE_AssertPointer(anim, "NULL animation pointer");
    NE_Assert(model->modeltype == NE_Animated, "Not an animated model");
    model->animinfo[1]->animation = anim;
    uint32_t frames = DSMA_GetNumFrames(anim->data);
    model->animinfo[1]->numframes = frames;
}

//---------------------------------------------------------

// Internal use... see below
extern bool NE_TestTouch;

void NE_ModelDraw(const NE_Model *model)
{
    NE_AssertPointer(model, "NULL pointer");
    if (model->meshdata == NULL)
        return;

    if (model->modeltype == NE_Animated)
    {
        // The base animation must always be present. The secondary animation
        // isn't required to draw the model.
        if (model->animinfo[0]->animation == NULL)
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
        if (model->animinfo[0]->animation && model->animinfo[1]->animation)
        {
            int ret = DSMA_DrawModelBlendAnimation(model->meshdata,
                    model->animinfo[0]->animation->data,
                    model->animinfo[0]->currframe,
                    model->animinfo[1]->animation->data,
                    model->animinfo[1]->currframe,
                    model->anim_blend);
            NE_Assert(ret == DSMA_SUCCESS, "Failed to draw animated model");
        }
        else // if (model->animinfo[0]->animation)
        {
            int ret = DSMA_DrawModel(model->meshdata,
                                     model->animinfo[0]->animation->data,
                                     model->animinfo[0]->currframe);
            NE_Assert(ret == DSMA_SUCCESS, "Failed to draw animated model");
        }
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
        memcpy(dest->animinfo[0], source->animinfo[0], sizeof(NE_AnimInfo));
        memcpy(dest->animinfo[1], source->animinfo[1], sizeof(NE_AnimInfo));
    }

    dest->meshdata = source->meshdata;
    dest->texture = source->texture;

    dest->free_mesh = false;
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

        for (int j = 0; j < 2; j++)
        {
            NE_AnimInfo *animinfo = NE_ModelPointers[i]->animinfo[j];

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
}

void NE_ModelAnimStart(NE_Model *model, NE_AnimationType type, int32_t speed)
{
    NE_AssertPointer(model, "NULL pointer");
    NE_Assert(model->modeltype == NE_Animated, "Not an animated model");
    model->animinfo[0]->type = type;
    model->animinfo[0]->speed = speed;
    model->animinfo[0]->currframe = 0;
}

void NE_ModelAnimSecondaryStart(NE_Model *model, NE_AnimationType type,
                                int32_t speed)
{
    NE_AssertPointer(model, "NULL pointer");
    NE_Assert(model->modeltype == NE_Animated, "Not an animated model");
    model->animinfo[1]->type = type;
    model->animinfo[1]->speed = speed;
    model->animinfo[1]->currframe = 0;
    model->anim_blend = 0;
}

void NE_ModelAnimSetSpeed(NE_Model *model, int32_t speed)
{
    NE_AssertPointer(model, "NULL pointer");
    NE_Assert(model->modeltype == NE_Animated, "Not an animated model");
    model->animinfo[0]->speed = speed;
}

void NE_ModelAnimSecondarySetSpeed(NE_Model *model, int32_t speed)
{
    NE_AssertPointer(model, "NULL pointer");
    NE_Assert(model->modeltype == NE_Animated, "Not an animated model");
    model->animinfo[1]->speed = speed;
}

int32_t NE_ModelAnimGetFrame(const NE_Model *model)
{
    NE_AssertPointer(model, "NULL pointer");
    NE_Assert(model->modeltype == NE_Animated, "Not an animated model");
    return model->animinfo[0]->currframe;
}

int32_t NE_ModelAnimSecondaryGetFrame(const NE_Model *model)
{
    NE_AssertPointer(model, "NULL pointer");
    NE_Assert(model->modeltype == NE_Animated, "Not an animated model");
    return model->animinfo[1]->currframe;
}

void NE_ModelAnimSetFrame(NE_Model *model, int32_t frame)
{
    NE_AssertPointer(model, "NULL pointer");
    NE_Assert(model->modeltype == NE_Animated, "Not an animated model");
    // TODO: Check if it is off bounds
    model->animinfo[0]->currframe = frame;
}

void NE_ModelAnimSecondarySetFrame(NE_Model *model, int32_t frame)
{
    NE_AssertPointer(model, "NULL pointer");
    NE_Assert(model->modeltype == NE_Animated, "Not an animated model");
    // TODO: Check if it is off bounds
    model->animinfo[1]->currframe = frame;
}

void NE_ModelAnimSecondarySetFactor(NE_Model *model, int32_t factor)
{
    NE_AssertPointer(model, "NULL pointer");
    NE_Assert(model->modeltype == NE_Animated, "Not an animated model");
    if (factor < 0)
        factor = 0;
    if (factor > inttof32(1))
        factor = inttof32(1);
    model->anim_blend = factor;
}

void NE_ModelAnimSecondaryClear(NE_Model *model, bool replace_base_anim)
{
    NE_AssertPointer(model, "NULL pointer");
    NE_Assert(model->modeltype == NE_Animated, "Not an animated model");

    // Return if there is no animation to remove
    if (model->animinfo[1]->animation == NULL)
        return;

    if (replace_base_anim)
        memcpy(model->animinfo[0], model->animinfo[1], sizeof(NE_AnimInfo));

    memset(model->animinfo[1], 0, sizeof(NE_AnimInfo));
}

int NE_ModelLoadDSMFAT(NE_Model *model, const char *path)
{
    if (!ne_model_system_inited)
        return 0;

    NE_AssertPointer(model, "NULL model pointer");
    NE_AssertPointer(path, "NULL path pointer");
    NE_Assert(model->modeltype == NE_Animated, "Not an animated model");

    void *pointer = NE_FATLoadData(path);
    if (pointer == NULL)
        return 1;

    if (model->free_mesh)
        free((void *)model->meshdata);

    model->meshdata = (void *)pointer;
    model->free_mesh = true;

    return 1;
}

int NE_ModelLoadDSM(NE_Model *model, const void *pointer)
{
    if (!ne_model_system_inited)
        return 0;

    NE_AssertPointer(model, "NULL model pointer");
    NE_AssertPointer(pointer, "NULL data pointer");
    NE_Assert(model->modeltype == NE_Animated, "Not an animated model");

    if (model->free_mesh)
        free((void *)model->meshdata);

    model->free_mesh = false;
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
