// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include <nds/arm9/postest.h>

#include "NEMain.h"

/*! \file   NEModel.c */

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
	while (1) {
		if (NE_ModelPointers[i] == NULL) {
			NE_ModelPointers[i] = model;
			break;
		}
		i++;
		if (i == NE_MAX_MODELS) {
			NE_DebugPrint("No free slots");
			free(model);
			return NULL;
		}
	}

	model->sx = model->sy = model->sz = inttof32(1);

	if (type == NE_Animated) {
		model->meshdata = calloc(1, sizeof(NE_AnimData));
		NE_AssertPointer(model->meshdata,
				 "Couldn't allocate animation data.");
		NE_AnimData *anim = (NE_AnimData *) model->meshdata;

		anim->direction = 1;
		model->anim_interpolate = true;
	} else { /*if (type == NE_Static) */
		model->meshdata = NULL;
	}

	model->modeltype = type;

	return model;
}

void NE_ModelDelete(NE_Model *model)
{
	if (!ne_model_system_inited)
		return;

	NE_AssertPointer(model, "NULL pointer");

	int i = 0;
	while (1) {
		if (i == NE_MAX_MODELS) {
			NE_DebugPrint("Model not found");
			return;
		}
		if (NE_ModelPointers[i] == model) {
			NE_ModelPointers[i] = NULL;
			break;
		}
		i++;
	}

	if (!model->iscloned && model->meshfromfat) {
		if (model->modeltype == NE_Animated)
			free(((NE_AnimData *) model->meshdata)->fileptrtr);
		else /* if (model->modeltype == NE_Static) */
			free(model->meshdata);
	}

	if (model->modeltype == NE_Animated)
		free(model->meshdata);	//Free animation data

	free(model);
}

int NE_ModelLoadStaticMeshFAT(NE_Model *model, char *path)
{
	if (!ne_model_system_inited)
		return 0;

	NE_AssertPointer(model, "NULL model pointer");
	NE_AssertPointer(path, "NULL path pointer");
	NE_Assert(model->modeltype == NE_Static, "Not a static model");
	NE_Assert(!model->iscloned, "Can't load a mesh to a cloned model");

	// Free previous data...
	if (model->meshfromfat && model->meshdata != NULL) {
		free(model->meshdata);
		model->meshdata = NULL;
	}

	model->meshdata = (u32 *) NE_FATLoadData(path);
	NE_AssertPointer(model->meshdata, "Couldn't load file from FAT");

	if (model->meshdata == NULL)
		return 0;

	model->meshfromfat = true;

	return 1;
}

int NE_ModelLoadStaticMesh(NE_Model *model, void *pointer)
{
	if (!ne_model_system_inited)
		return 0;

	NE_AssertPointer(model, "NULL model pointer");
	NE_AssertPointer(pointer, "NULL data pointer");
	NE_Assert(model->modeltype == NE_Static, "Not a static model");
	NE_Assert(!model->iscloned, "Can't load a mesh to a cloned model");

	// Free previous data...
	if (model->meshfromfat && model->meshdata != NULL) {
		free(model->meshdata);
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

//------------------------------------------------------------------------------

static void __ne_drawanimatedmodel_interpolate(NE_AnimData *anim)
{
	int frame_one = anim->currframe;
	int frame_two = anim->currframe + anim->direction;
	u32 time = anim->nextframetime;

	if (anim->direction < 0 && frame_two < anim->startframe) {
		if (anim->animtype == NE_ANIM_LOOP)
			frame_two = anim->endframe;
		else if (anim->animtype == NE_ANIM_UPDOWN)
			frame_two = anim->currframe + 1;
		else if (anim->animtype == NE_ANIM_ONESHOT)
			frame_two = frame_one;
	} else if (anim->direction > 0 && frame_two > anim->endframe) {
		if (anim->animtype == NE_ANIM_LOOP)
			frame_two = anim->startframe;
		else if (anim->animtype == NE_ANIM_UPDOWN)
			frame_two = anim->currframe - 1;
		else if (anim->animtype == NE_ANIM_ONESHOT)
			frame_two = frame_one;
	}

	u32 *fileptr = anim->fileptrtr + 2;
	NE_Assert(frame_one < *fileptr && frame_two < *fileptr,
		  "Drawing nonexistent frame.");
	fileptr++;
	u32 vtxcount = *fileptr++;
	u16 *framearrayptr = (u16 *) ((int)anim->fileptrtr + (int)*fileptr++);
	s16 *vtxarrayptr = (s16 *) ((int)anim->fileptrtr + (int)*fileptr++);
	s16 *normarrayptr = (s16 *) ((int)anim->fileptrtr + (int)*fileptr++);
	s16 *texcoordsarrayptr = (s16 *) ((int)anim->fileptrtr + (int)*fileptr);

	u16 *frame_one_ptr = (u16 *) ((int)framearrayptr + (vtxcount * 6 * frame_one));
	u16 *frame_two_ptr = (u16 *) ((int)framearrayptr + (vtxcount * 6 * frame_two));

	GFX_BEGIN = GL_TRIANGLES;

	for (u32 i = 0; i < vtxcount; i++) {
		s32 vector[3];
		s16 *frame_one_anim, *frame_two_anim;

		// Texture coordinates
		// -------------------

		frame_one_anim = &texcoordsarrayptr[*frame_one_ptr * 2];
		GFX_TEX_COORD = ((s32) * frame_one_anim) |
				(((s32) * (frame_one_anim + 1)) << 16);
		frame_one_ptr++;
		frame_two_ptr++;

		// Normal
		// ------

		frame_one_anim = &normarrayptr[*frame_one_ptr * 3];
		frame_two_anim = &normarrayptr[*frame_two_ptr * 3];
		for (int j = 0; j < 3; j++) {
			vector[j] = (s32) *frame_one_anim;
			vector[j] += (((s32) *frame_two_anim -
				       (s32) *frame_one_anim) * time) >> 6;
			vector[j] &= 0x3FF;
			frame_one_anim++;
			frame_two_anim++;
		}
		GFX_NORMAL = (vector[0] << 20) | (vector[1] << 10) | vector[2];
		frame_one_ptr++;
		frame_two_ptr++;

		// Vertex
		// ------

		frame_one_anim = &vtxarrayptr[*frame_one_ptr * 3];
		frame_two_anim = &vtxarrayptr[*frame_two_ptr * 3];
		for (int j = 0; j < 3; j++) {
			vector[j] = (s32) *frame_one_anim;
			vector[j] += (((s32) *frame_two_anim -
				       (s32) *frame_one_anim) * time) >> 6;
			vector[j] &= 0xFFFF;
			frame_one_anim++;
			frame_two_anim++;
		}
		GFX_VERTEX16 = vector[0] | (vector[1] << 16);
		GFX_VERTEX16 = vector[2];
		frame_one_ptr++;
		frame_two_ptr++;
	}

	// GFX_END = 0;
}

static void __ne_drawanimatedmodel_nointerpolate(NE_AnimData *anim)
{
	int frame = anim->currframe;

	u32 *fileptr = anim->fileptrtr + 2;
	NE_Assert(frame < *fileptr, "Drawing nonexistent frame");
	fileptr++;

	u32 vtxcount = *fileptr++;
	u16 *framearrayptr = (u16 *) ((int)anim->fileptrtr + (int)*fileptr++);
	s16 *vtxarrayptr = (s16 *) ((int)anim->fileptrtr + (int)*fileptr++);
	s16 *normarrayptr = (s16 *) ((int)anim->fileptrtr + (int)*fileptr++);
	s16 *texcoordsarrayptr = (s16 *) ((int)anim->fileptrtr + (int)*fileptr);

	u16 *frame_ptr = (u16 *) ((int)framearrayptr + (vtxcount * 6 * frame));

	GFX_BEGIN = GL_TRIANGLES;

	for (u32 i = 0; i < vtxcount; i++) {
		s32 vector[3];
		s16 *frame_anim;

		// Texture coordinates
		// -------------------

		frame_anim = &texcoordsarrayptr[*frame_ptr * 2];
		GFX_TEX_COORD = ((s32) *frame_anim) |
				(((s32) *(frame_anim + 1)) << 16);
		frame_ptr++;

		// Normal
		// ------

		frame_anim = &normarrayptr[*frame_ptr * 3];
		for (int j = 0; j < 3; j++) {
			vector[j] = ((s32) *frame_anim) & 0x3FF;
			frame_anim++;
		}
		GFX_NORMAL = (vector[0] << 20) | (vector[1] << 10) | vector[2];
		frame_ptr++;

		// Vertex
		// ------

		frame_anim = &vtxarrayptr[*frame_ptr * 3];
		for (int j = 0; j < 3; j++) {
			vector[j] = ((s32) *frame_anim) & 0xFFFF;
			frame_anim++;
		}
		GFX_VERTEX16 = vector[0] | (vector[1] << 16);
		GFX_VERTEX16 = vector[2];
		frame_ptr++;
	}

	// GFX_END = 0;
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
		if (((NE_AnimData *) model->meshdata)->fileptrtr == NULL)
			return;

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

	if (NE_TestTouch) {
		PosTest_Asynch(0, 0, 0);
	} else {
		// If the texture pointer is NULL, this will set GFX_TEX_FORMAT
		// to 0 and GFX_COLOR to white
		NE_MaterialUse(model->texture);
	}

	if (model->modeltype == NE_Static) {
		glCallList(model->meshdata);
	} else { // if(model->modeltype == NE_Animated)
		NE_AnimData *anim = (void *) model->meshdata;

		if (model->anim_interpolate)
			__ne_drawanimatedmodel_interpolate(anim);
		else
			__ne_drawanimatedmodel_nointerpolate(anim);
	}

	MATRIX_POP = 1;
}

void NE_ModelClone(NE_Model *dest, NE_Model *source)
{
	NE_AssertPointer(dest, "NULL dest pointer");
	NE_AssertPointer(source, "NULL source pointer");
	NE_Assert(dest->modeltype == source->modeltype,
		  "Different model types");

	if (dest->modeltype == NE_Animated) {
		swiCopy(source->meshdata, dest->meshdata,
			(sizeof(NE_AnimData) >> 2) | COPY_MODE_WORD);
		dest->iscloned = true;
		dest->texture = source->texture;
	} else {
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

	for (int i = 0; i < NE_MAX_MODELS; i++) {
		if (NE_ModelPointers[i] == NULL)
			continue;

		if (NE_ModelPointers[i]->modeltype != NE_Animated)
			continue;

		NE_AnimData *anim =
		    (NE_AnimData *) (NE_ModelPointers[i]->meshdata);

		anim->nextframetime += anim->speed[anim->currframe];

		if (abs(anim->nextframetime) <= 64)
			continue;

		anim->nextframetime = 0;

		switch (anim->animtype) {
		case NE_ANIM_LOOP:
			if (anim->currframe == anim->startframe
				&& anim->direction < 0) {
				anim->currframe = anim->endframe;
			} else if (anim->currframe == anim->endframe
					&& anim->direction > 0) {
				anim->currframe = anim->startframe;
			} else {
				if (anim->direction > 0)
					anim->currframe++;
				else
					anim->currframe--;
			}
			break;

		case NE_ANIM_ONESHOT:
			if (anim->currframe == anim->startframe
				&& anim->direction < 0) {
				NE_ModelAnimSetSpeed(NE_ModelPointers[i], 0);
			} else if (anim->currframe == anim->endframe
					&& anim->direction > 0) {
				NE_ModelAnimSetSpeed(NE_ModelPointers[i], 0);
			} else {
				if (anim->direction > 0)
					anim->currframe++;
				else
					anim->currframe--;
			}
			break;

		case NE_ANIM_UPDOWN:
			if (anim->currframe == anim->startframe
				&& anim->direction < 0) {
				anim->direction *= -1;
				anim->currframe++;
			} else if (anim->currframe == anim->endframe
					&& anim->direction > 0) {
				anim->direction *= -1;
				anim->currframe--;
			} else {
				if (anim->direction > 0)
					anim->currframe++;
				else
					anim->currframe--;
			}
			break;
		}
	}
}

void NE_ModelAnimStart(NE_Model *model, int min, int start, int max,
		       NE_AnimationTypes type, int speed)
{
	NE_AssertPointer(model, "NULL pointer");
	NE_Assert(model->modeltype == NE_Animated, "Not an animated model");

	NE_AnimData *anim = (void *)model->meshdata;

	anim->animtype = type;
	anim->currframe = start;
	anim->startframe = min;
	anim->endframe = max;
	anim->nextframetime = 0;
	anim->direction = ((speed >= 0) ? 1 : -1);

	for (int i = 0; i < NE_MAX_FRAMES; i++)
		anim->speed[i] = abs(speed);
}

void NE_ModelAnimSetSpeed(NE_Model *model, int speed)
{
	NE_AssertPointer(model, "NULL pointer");
	NE_Assert(model->modeltype == NE_Animated, "Not an animated model");

	NE_AnimData *anim = (void *)model->meshdata;

	for (int i = 0; i < NE_MAX_FRAMES; i++)
		anim->speed[i] = abs(speed);

	anim->direction = ((speed >= 0) ? 1 : -1);
}

void NE_ModelAnimSetFrameSpeed(NE_Model *model, int frame, int speed)
{
	NE_AssertPointer(model, "NULL pointer");
	NE_Assert(model->modeltype == NE_Animated, "Not an animated model");

	NE_AnimData *anim = (void *)model->meshdata;
	anim->speed[frame] = speed;
}

int NE_ModelAnimGetFrame(NE_Model *model)
{
	NE_AssertPointer(model, "NULL pointer");
	NE_Assert(model->modeltype == NE_Animated, "Not an animated model");
	NE_AnimData *anim = (void *)model->meshdata;
	return anim->currframe;
}

void NE_ModelAnimSetFrame(NE_Model *model, int frame)
{
	NE_AssertPointer(model, "NULL pointer");
	NE_Assert(model->modeltype == NE_Animated, "Not an animated model");

	NE_AnimData *anim = (void *)model->meshdata;
	anim->currframe = frame;
	anim->nextframetime = 0;
}

void NE_ModelAnimInterpolate(NE_Model *model, bool interpolate)
{
	NE_AssertPointer(model, "NULL pointer");
	NE_Assert(model->modeltype == NE_Animated, "Not an animated model");
	model->anim_interpolate = interpolate;
}

int NE_ModelLoadNEAFAT(NE_Model *model, char *path)
{
	if (!ne_model_system_inited)
		return 0;

	NE_AssertPointer(model, "NULL model pointer");
	NE_AssertPointer(path, "NULL path pointer");
	NE_Assert(model->modeltype == NE_Animated, "Not an animated model");

	if (model->meshfromfat)
		free(((NE_AnimData *) model->meshdata)->fileptrtr);

	model->iscloned = 0;
	model->meshfromfat = true;

	u32 *pointer = (u32 *) NE_FATLoadData(path);
	NE_AssertPointer(pointer, "Couldn't load file from FAT");

	// Check file type ('NEAM') - NEA file
	if (*pointer != 1296123214) {
		NE_DebugPrint("Not a NEA file");
		free(pointer);
		return 0;
	}

	// Check version
	if (pointer[1] != 2) {
		NE_DebugPrint("NEA file version is %ld, should be 2",
			      pointer[1]);
		free(pointer);
		return 0;
	}

	((NE_AnimData *) model->meshdata)->fileptrtr = (void *)pointer;

	return 1;
}

int NE_ModelLoadNEA(NE_Model *model, void *pointer)
{
	if (!ne_model_system_inited)
		return 0;

	NE_AssertPointer(model, "NULL model pointer");
	NE_AssertPointer(pointer, "NULL data pointer");
	NE_Assert(model->modeltype == NE_Animated, "Not an animated model");

	if (model->meshfromfat)
		free(((NE_AnimData *) model->meshdata)->fileptrtr);

	model->iscloned = 0;
	model->meshfromfat = false;

	u32 *ptr = pointer;

	// Check file type ('NEAM') - NEA file
	if (*ptr != 1296123214) {
		NE_DebugPrint("Not a NEA file");
		return 0;
	}

	ptr++;

	// Check version
	if (*ptr != 2) {
		NE_DebugPrint("NEA file version is %ld, should be 2", *ptr);
		return 0;
	}
	((NE_AnimData *) model->meshdata)->fileptrtr = pointer;

	return 1;
}

void NE_ModelDeleteAll(void)
{
	if (!ne_model_system_inited)
		return;

	for (int i = 0; i < NE_MAX_MODELS; i++)
		if (NE_ModelPointers[i] != NULL)
			NE_ModelDelete(NE_ModelPointers[i]);
}

void NE_ModelSystemReset(int number_of_models)
{
	if (ne_model_system_inited)
		NE_ModelSystemEnd();

	if (number_of_models < 1)
		NE_MAX_MODELS = NE_DEFAULT_MODELS;
	else
		NE_MAX_MODELS = number_of_models;

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
