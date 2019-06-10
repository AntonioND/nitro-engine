// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include "NEMain.h"

/*! \file   NE2D.c */

static NE_Sprite **NE_spritepointers = NULL;

static int NE_MAX_SPRITES;

static bool ne_sprite_system_inited = false;

NE_Sprite *NE_SpriteCreate(void)
{
	if (!ne_sprite_system_inited) {
		NE_DebugPrint("System not inited");
		return NULL;
	}

	for (int i = 0; i < NE_MAX_SPRITES; i++) {
		if (NE_spritepointers[i] != NULL)
			continue;

		NE_Sprite *sprite = calloc(1, sizeof(NE_Sprite));

		NE_AssertPointer(sprite, "Not enough memory");

		sprite->visible = true;
		sprite->scale = inttof32(1);
		sprite->color = NE_White;
		sprite->mat = NULL;
		sprite->alpha = 31;

		NE_spritepointers[i] = sprite;

		return sprite;
	}

	NE_DebugPrint("No free slots");
	return NULL;
}

void NE_SpriteSetPos(NE_Sprite *sprite, int x, int y)
{
	NE_AssertPointer(sprite, "NULL pointer");
	sprite->x = x;
	sprite->y = y;
}

void NE_SpriteSetSize(NE_Sprite *sprite, int x, int y)
{
	NE_AssertPointer(sprite, "NULL pointer");
	sprite->sx = x;
	sprite->sy = y;
}

void NE_SpriteSetRot(NE_Sprite *sprite, int angle)
{
	NE_AssertPointer(sprite, "NULL pointer");
	sprite->rot_angle = angle;
}

void NE_SpriteSetScaleI(NE_Sprite *sprite, int scale)
{
	NE_AssertPointer(sprite, "NULL pointer");
	sprite->scale = scale;
}

void NE_SpriteSetMaterial(NE_Sprite *sprite, NE_Material *mat)
{
	NE_AssertPointer(sprite, "NULL sprite pointer");
	NE_AssertPointer(mat, "NULL material pointer");
	sprite->mat = mat;
}

void NE_SpriteSetPriority(NE_Sprite *sprite, int priority)
{
	NE_AssertPointer(sprite, "NULL pointer");
	sprite->priority = priority;
}

void NE_SpriteVisible(NE_Sprite *sprite, bool visible)
{
	NE_AssertPointer(sprite, "NULL pointer");
	sprite->visible = visible;
}

void NE_SpriteSetParams(NE_Sprite *sprite, u8 alpha, u8 id, u32 color)
{
	NE_AssertPointer(sprite, "NULL pointer");
	NE_AssertMinMax(0, alpha, 31, "Invalid alpha value %d", alpha);
	NE_AssertMinMax(0, id, 63, "Invalid polygon ID %d", id);

	sprite->alpha = alpha;
	sprite->id = id;
	sprite->color = color;
}

void NE_SpriteDelete(NE_Sprite *sprite)
{
	if (!ne_sprite_system_inited)
		return;

	NE_AssertPointer(sprite, "NULL pointer");

	for (int i = 0; i < NE_MAX_SPRITES; i++) {
		if (NE_spritepointers[i] != sprite)
			continue;

		NE_spritepointers[i] = NULL;
		free((void *)sprite);

		return;
	}

	NE_DebugPrint("Object not found");
	return;
}

void NE_SpriteDeleteAll(void)
{
	if (!ne_sprite_system_inited)
		return;

	int i;
	for (i = 0; i < NE_MAX_SPRITES; i++)
		NE_SpriteDelete(NE_spritepointers[i]);
}

void NE_SpriteSystemReset(int number_of_sprites)
{
	if (ne_sprite_system_inited)
		NE_SpriteSystemEnd();

	if (number_of_sprites < 1)
		NE_MAX_SPRITES = NE_DEFAULT_SPRITES;
	else
		NE_MAX_SPRITES = number_of_sprites;

	NE_spritepointers = calloc(NE_MAX_SPRITES, sizeof(NE_spritepointers));
	NE_AssertPointer(NE_spritepointers, "Not enough memory");

	ne_sprite_system_inited = true;
}

void NE_SpriteSystemEnd(void)
{
	if (!ne_sprite_system_inited)
		return;

	NE_SpriteDeleteAll();

	free(NE_spritepointers);

	ne_sprite_system_inited = false;
}

void NE_SpriteDraw(NE_Sprite *sprite)
{
	if (!ne_sprite_system_inited)
		return;

	NE_AssertPointer(sprite, "NULL pointer");

	if (!sprite->visible)
		return;

	if (sprite->rot_angle) {
		glPushMatrix();

		NE_2DViewRotateScaleByPositionI(sprite->x + (sprite->sx >> 1),
						sprite->y + (sprite->sy >> 1),
						sprite->rot_angle,
						sprite->scale);
	} else {
		NE_2DViewScaleByPositionI(sprite->x + (sprite->sx >> 1),
					  sprite->y + (sprite->sy >> 1),
					  sprite->scale);
	}

	GFX_POLY_FORMAT = POLY_ALPHA(sprite->alpha) | POLY_ID(sprite->id) |
			  NE_CULL_NONE;

	NE_2DDrawTexturedQuadColor(sprite->x, sprite->y, sprite->x + sprite->sx,
				   sprite->y + sprite->sy, sprite->priority,
				   sprite->mat, sprite->color);

	if (sprite->rot_angle)
		glPopMatrix(1);
}

void NE_SpriteDrawAll(void)
{
	if (!ne_sprite_system_inited)
		return;

	int i;
	for (i = 0; i < NE_MAX_SPRITES; i++) {
		if (NE_spritepointers[i] == NULL)
			continue;

		NE_Sprite *sprite = NE_spritepointers[i];

		if (!sprite->visible)
			continue;

		if (sprite->rot_angle) {
			glPushMatrix();

			NE_2DViewRotateScaleByPositionI(sprite->x + (sprite->sx >> 1),
							sprite->y + (sprite->sy >> 1),
							sprite->rot_angle,
							sprite->scale);
		} else {
			NE_2DViewScaleByPositionI(sprite->x + (sprite->sx >> 1),
						  sprite->y + (sprite->sy >> 1),
						  sprite->scale);
		}

		GFX_POLY_FORMAT = POLY_ALPHA(sprite->alpha) |
				  POLY_ID(sprite->id) | NE_CULL_NONE;

		NE_2DDrawTexturedQuadColor(sprite->x, sprite->y,
					   sprite->x + sprite->sx,
					   sprite->y + sprite->sy,
					   sprite->priority,
					   sprite->mat, sprite->color);

		if (sprite->rot_angle)
			glPopMatrix(1);
	}
}

//----------------------------------------------------------

//              Functions to draw freely in 2D.

//----------------------------------------------------------

// Internal use. See NETexture.c

int __NE_TextureGetRawX(NE_Material *tex);
int __NE_TextureGetRawY(NE_Material *tex);

//--------------------------------------------

void NE_2DViewInit(void)
{
	glViewport(0, 0, 255, 191);

	MATRIX_CONTROL = GL_PROJECTION;
	MATRIX_IDENTITY = 0;
	glOrthof32(0, 256 << 12, 192 << 12, 0, inttof32(1), inttof32(-1));

	MATRIX_CONTROL = GL_MODELVIEW;
	MATRIX_IDENTITY = 0;

	// Why the "<< 12"? Because in this way you can use coordinates 0 - 255
	// and 0 - 191 when drawing but, when rotating, the DS can be more
	// accurate.
	MATRIX_SCALE = inttof32(1 << 12);
	MATRIX_SCALE = inttof32(1 << 12);
	MATRIX_SCALE = inttof32(1);

	NE_PolyFormat(31, 0, 0, NE_CULL_NONE, 0);
}

void NE_2DViewRotateScaleByPositionI(int x, int y, int rotz, int scale)
{
	NE_ViewMoveI(x, y, 0);

	MATRIX_SCALE = scale;
	MATRIX_SCALE = scale;
	MATRIX_SCALE = inttof32(1);

	glRotateZi(rotz << 6);

	NE_ViewMoveI(-x, -y, 0);
}

void NE_2DViewRotateByPosition(int x, int y, int rotz)
{
	NE_ViewMoveI(x, y, 0);

	glRotateZi(rotz << 6);

	NE_ViewMoveI(-x, -y, 0);
}

void NE_2DViewScaleByPositionI(int x, int y, int scale)
{
	NE_ViewMoveI(x, y, 0);

	MATRIX_SCALE = scale;
	MATRIX_SCALE = scale;
	MATRIX_SCALE = inttof32(1);

	NE_ViewMoveI(-x, -y, 0);
}

void NE_2DDrawQuad(s16 x1, s16 y1, s16 x2, s16 y2, s16 z, u32 color)
{
	GFX_BEGIN = GL_QUADS;

	GFX_TEX_FORMAT = 0;

	GFX_COLOR = color;

	GFX_VERTEX16 = (y1 << 16) | (x1 & 0xFFFF);	// Up-left
	GFX_VERTEX16 = z;

	GFX_VERTEX_XY = (y2 << 16) | (x1 & 0xFFFF);	// Down-left

	GFX_VERTEX_XY = (y2 << 16) | (x2 & 0xFFFF);	// Down-right

	GFX_VERTEX_XY = (y1 << 16) | (x2 & 0xFFFF);	// Up-right
}

void NE_2DDrawQuadGradient(s16 x1, s16 y1, s16 x2, s16 y2, s16 z, u32 color1,
			   u32 color2, u32 color3, u32 color4)
{
	GFX_BEGIN = GL_QUADS;

	GFX_TEX_FORMAT = 0;

	GFX_COLOR = color1;
	GFX_VERTEX16 = (y1 << 16) | (x1 & 0xFFFF);	// Up-left
	GFX_VERTEX16 = z;

	GFX_COLOR = color4;
	GFX_VERTEX_XY = (y2 << 16) | (x1 & 0xFFFF);	// Down-left

	GFX_COLOR = color3;
	GFX_VERTEX_XY = (y2 << 16) | (x2 & 0xFFFF);	// Down-right

	GFX_COLOR = color2;
	GFX_VERTEX_XY = (y1 << 16) | (x2 & 0xFFFF);	// Up-right
}

void NE_2DDrawTexturedQuad(s16 x1, s16 y1, s16 x2, s16 y2, s16 z,
			   NE_Material *mat)
{
	NE_AssertPointer(mat, "NULL pointer");
	NE_Assert(mat->texindex != NE_NO_TEXTURE, "No texture");

	int rx = __NE_TextureGetRawX(mat), ry = __NE_TextureGetRawY(mat);
	int x = NE_TextureGetSizeX(mat), y = NE_TextureGetSizeY(mat);

	NE_MaterialUse(mat);

	GFX_BEGIN = GL_QUADS;

	GFX_TEX_COORD = TEXTURE_PACK(0, 0);
	GFX_VERTEX16 = (y1 << 16) | (x1 & 0xFFFF);	// Up-left
	GFX_VERTEX16 = z;

	GFX_TEX_COORD = TEXTURE_PACK(0, (inttot16(y) + ry));
	GFX_VERTEX_XY = (y2 << 16) | (x1 & 0xFFFF);	// Down-left

	GFX_TEX_COORD = TEXTURE_PACK((inttot16(x) + rx), (inttot16(y) + ry));
	GFX_VERTEX_XY = (y2 << 16) | (x2 & 0xFFFF);	// Down-right

	GFX_TEX_COORD = TEXTURE_PACK((inttot16(x) + rx), 0);
	GFX_VERTEX_XY = (y1 << 16) | (x2 & 0xFFFF);	// Up-right
}

void NE_2DDrawTexturedQuadColor(s16 x1, s16 y1, s16 x2, s16 y2, s16 z,
				NE_Material *mat, u32 color)
{
	NE_AssertPointer(mat, "NULL pointer");
	NE_Assert(mat->texindex != NE_NO_TEXTURE, "No texture");

	int rx = __NE_TextureGetRawX(mat), ry = __NE_TextureGetRawY(mat);
	int x = NE_TextureGetSizeX(mat), y = NE_TextureGetSizeY(mat);

	NE_MaterialUse(mat);

	GFX_COLOR = color;

	GFX_BEGIN = GL_QUADS;

	GFX_TEX_COORD = TEXTURE_PACK(0, 0);
	GFX_VERTEX16 = (y1 << 16) | (x1 & 0xFFFF);	// Up-left
	GFX_VERTEX16 = z;

	GFX_TEX_COORD = TEXTURE_PACK(0, (inttot16(y) + ry));
	GFX_VERTEX_XY = (y2 << 16) | (x1 & 0xFFFF);	// Down-left

	GFX_TEX_COORD = TEXTURE_PACK((inttot16(x) + rx), (inttot16(y) + ry));
	GFX_VERTEX_XY = (y2 << 16) | (x2 & 0xFFFF);	// Down-right

	GFX_TEX_COORD = TEXTURE_PACK((inttot16(x) + rx), 0);
	GFX_VERTEX_XY = (y1 << 16) | (x2 & 0xFFFF);	// Up-right
}

void NE_2DDrawTexturedQuadGradient(s16 x1, s16 y1, s16 x2, s16 y2, s16 z,
				   NE_Material *mat, u32 color1, u32 color2,
				   u32 color3, u32 color4)
{
	NE_AssertPointer(mat, "NULL pointer");
	NE_Assert(mat->texindex != NE_NO_TEXTURE, "No texture");

	int rx = __NE_TextureGetRawX(mat), ry = __NE_TextureGetRawY(mat);
	int x = NE_TextureGetSizeX(mat), y = NE_TextureGetSizeY(mat);

	NE_MaterialUse(mat);

	GFX_BEGIN = GL_QUADS;

	GFX_COLOR = color1;
	GFX_TEX_COORD = TEXTURE_PACK(0, 0);
	GFX_VERTEX16 = (y1 << 16) | (x1 & 0xFFFF);	// Up-left
	GFX_VERTEX16 = z;

	GFX_COLOR = color4;
	GFX_TEX_COORD = TEXTURE_PACK(0, (inttot16(y) + ry));
	GFX_VERTEX_XY = (y2 << 16) | (x1 & 0xFFFF);	// Down-left

	GFX_COLOR = color3;
	GFX_TEX_COORD = TEXTURE_PACK((inttot16(x) + rx), (inttot16(y) + ry));
	GFX_VERTEX_XY = (y2 << 16) | (x2 & 0xFFFF);	// Down-right

	GFX_COLOR = color2;
	GFX_TEX_COORD = TEXTURE_PACK((inttot16(x) + rx), 0);
	GFX_VERTEX_XY = (y1 << 16) | (x2 & 0xFFFF);	// Up-right
}
