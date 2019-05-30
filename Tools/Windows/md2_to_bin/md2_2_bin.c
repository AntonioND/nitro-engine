// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2008-2011, 2019, Antonio Niño Díaz

// I used the information in this web to make the converter:
// http://tfc.duke.free.fr/coding/md2-specs-en.html

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "nds.h"
#include "dlmaker.h"

#define MAX_PATHLEN	1024

#define absf(x)		(((x) > 0) ? (x) : -(x))

typedef float vec3_t[3];

typedef struct {
	int ident;
	int version;

	int skinwidth;
	int skinheight;
	int framesize;
	int num_skins;
	int num_vertices;
	int num_st;
	int num_tris;
	int num_glcmds;
	int num_frames;
	int offset_skins;
	int offset_st;
	int offset_tris;
	int offset_frames;
	int offset_glcmds;
	int offset_end;
} md2_header_t;

typedef struct {
	short s;
	short t;
} md2_texCoord_t;

typedef struct {
	unsigned short vertex[3];
	unsigned short st[3];
} md2_triangle_t;

typedef struct {
	unsigned char v[3];
	unsigned char normalIndex;
} md2_vertex_t;

typedef struct {
	vec3_t scale;
	vec3_t translate;
	char name[16];
	md2_vertex_t *verts;
} md2_frame_t;

typedef struct {
	float s;
	float t;
	int index;
} md2_glcmd_t;

float anorms[162][3] = {
#include "anorms.h"
};

void PrintUse(void)
{
	printf("\n\n");
	printf("Instructions:\n");
	printf("    md2_2_bin [input.md2] [output.bin] <[float scale]>\n");
	printf("       <[float translate x] [float translate x] [float translate x]>\n\n");
}

int floattovtx10(float n)
{
	int value = ((v10) ((n) * (1 << 6))) & 0x3FF;
	if (n < 0)
		value |= (1 << 10);
	else
		value &= 0x1FF;
	return value;
}

int IsValidSize(int size)
{
	return (size == 8 || size == 16 || size == 32 || size == 64 ||
		size == 128 || size == 256 || size == 512 || size == 1024);
}

int main(int argc, char *argv[])
{
	printf("\n\n");
	printf("       ########################################\n");
	printf("       #                                      #\n");
	printf("       #   --    --    MD2 2 BIN    --   --   #\n");
	printf("       #                               v2.0   #\n");
	printf("       ########################################\n");
	printf("\n");
	printf("Copyright (c) 2008-2011, 2019 Antonio Nino Diaz\n");
	printf("All rights reserved.\n\n");

	// Default values
	float global_scale = 1;
	float global_translation[3] = { 0, 0, 0 };

	switch (argc) {
	case 0:
	case 1:
	case 2:
		// Not enough arguments
		PrintUse();
		return -1;
	case 3:
		// Use default modifications
		break;
	case 4:
		// Use default translation and custom scale
		global_scale = atof(argv[3]);
		break;
	case 5:
	case 6:
		// Custom translation, not enough
		printf("You must set 3 coordinates for translation, not less.");
		PrintUse();
		return -1;
	case 7:
		// Custom translation + scale
		global_scale = atof(argv[3]);
		global_translation[0] = atof(argv[4]);
		global_translation[1] = atof(argv[5]);
		global_translation[2] = atof(argv[6]);
		break;
	default:
		// Too many arguments
		PrintUse();
		return -1;
	}

	char inputfilepath[MAX_PATHLEN];
	char outputfilepath[MAX_PATHLEN];

	strcpy(inputfilepath, argv[1]);
	strcpy(outputfilepath, argv[2]);

	if (global_scale == 0) {
		printf("\nScale can't be 0!!");
		PrintUse();
		return -1;
	}

	char *md2data;

	printf("\n");
	printf("Scale:     %f\n", global_scale);
	printf("Translate: %f, %f, %f\n", global_translation[0],
	       global_translation[1], global_translation[2]);

	printf("\n");
	printf("Loading MD2 model...\n");

	FILE *datafile = fopen(inputfilepath, "r");
	if (datafile != NULL) {
		fseek(datafile, 0, SEEK_END);
		long int size_ = ftell(datafile);
		rewind(datafile);
		md2data = (char *)malloc(sizeof(char) * size_);
		fread(md2data, 1, size_, datafile);
		fclose(datafile);
	} else {
		fclose(datafile);
		printf("\n\nCouldn't open %s!!\n\n", inputfilepath);
		PrintUse();
		return -1;
	}

	md2_header_t *header = (md2_header_t *) md2data;

	if ((header->ident != 844121161) || (header->version != 8)) {
		printf("\n\nWrong file type or version!!\n\n");
		return -1;
	}

	int t_w = header->skinwidth, t_h = header->skinheight;

	if (t_w > 1024 || t_h > 1024) {
		printf("\n\nTexture too big!!\n\n");
		return -1;
	}

	if (!IsValidSize(t_w) || !IsValidSize(t_h)) {
		printf("\nWrong texture size. It must be a power of 2.\n");
		printf("\nAlthough the model uses an invalid texture size, it will be converted.\n");
		printf("\nResize the texture to nearest valid size.\n\n");
	}

	while (!IsValidSize(t_w)) {
		t_w++;
	}
	while (!IsValidSize(t_h)) {
		t_h++;
	}

	int num_tris = header->num_tris;

	md2_frame_t *frame = NULL;
	md2_texCoord_t *texcoord =
	    (md2_texCoord_t *) ((uintptr_t) header->offset_st +
				(uintptr_t) header);
	md2_triangle_t *triangle =
	    (md2_triangle_t *) ((uintptr_t) header->offset_tris +
				(uintptr_t) header);

	// Current vertex
	md2_vertex_t *vtx;

	printf("\nMD2 texture size: %dx%d", t_w, t_h);

	printf("\nCreating display list...\n");

	float bigvalue = 0;

	// Everything ready, let's "draw" the display list
	frame =
	    (md2_frame_t *) ((uintptr_t) header->offset_frames +
			     (uintptr_t) header);

	NewDL();

	// Send GL_TRIANGLES command
	NewParamDL(0);
	NewCommandDL(ID_BEGIN);

	// Keep track of the last command to avoid useless repetition
	int olds_ = -1, oldt_ = -1;
	int oldnormindex = -1;

	int vtxcount = 0;

	int t = 0, v = 0;
	for (t = 0; t < num_tris; t++)
		for (v = 0; v < 3; v++) {
			vtx = (md2_vertex_t *) ((uintptr_t) (&(frame->verts)));
			vtx = &vtx[triangle[t].vertex[v]];

			// Texture coordinates
			short s_ = texcoord[triangle[t].st[v]].s;
			short t_ = texcoord[triangle[t].st[v]].t;
			if (olds_ != s_ || oldt_ != t_) {
				olds_ = s_;
				oldt_ = t_;

				// This is used to scale UVs if using a texture
				// size unsupported by DS
				s_ = (int)((float)(s_ * t_w) /
					   (float)header->skinwidth);
				t_ = (int)((float)(t_ * t_h) /
					   (float)header->skinheight);
				NewParamDL(TEXTURE_PACK(s_ << 4, t_ << 4)); // (t_h-t_)<<4));
				NewCommandDL(ID_TEX_COORD);
			}
			// Normal
			float norm[3];
			if (oldnormindex != vtx->normalIndex) {
				oldnormindex = vtx->normalIndex;
				int b;
				for (b = 0; b < 3; b++)
					norm[b] = anorms[vtx->normalIndex][b];
				NewParamDL(NORMAL_PACK
					   (floattov10(norm[0]),
					    floattov10(norm[1]),
					    floattov10(norm[2])));
				NewCommandDL(ID_NORMAL);
			}
			// Vertex
			float _v[3];
			int a = 0;
			int vtx10 = false;
			for (a = 0; a < 3; a++) {
				vtxcount++;
				_v[a] =
				    ((float)frame->scale[a] *
				     (float)(vtx->v[a])) +
				    (float)frame->translate[a];
				_v[a] += global_translation[a];
				_v[a] *= global_scale;
				// 7.9997 for VTX_16, 7.98 for VTX_10
				if ((absf(_v[a]) > (float)7.9997)
				    && (absf(bigvalue) < absf(_v[a])))
					bigvalue = _v[a];
				// Test to see if it is more accurate to use
				// VTX_10 or VTX_16
				if (absf(_v[a]) < (float)7.98) {
					float aux = (float)1 / (float)64;
					float tempvtx = _v[a] / aux;
					tempvtx =
					    absf(tempvtx - (float)(int)tempvtx);
					// Arbitrary threshold
					aux = (float)0.2;
					if (tempvtx < aux
					    || tempvtx > ((float)1 - aux))
						vtx10 = true;
				}
			}
			if (vtx10) {
				NewParamDL(VERTEX_10_PACK
					   (floattovtx10(_v[0]),
					    floattovtx10(_v[2]),
					    floattovtx10(_v[1])));
				NewCommandDL(ID_VERTEX10);
			} else {
				NewParamDL((floattov16(_v[2]) << 16) |
					   (floattov16(_v[0]) & 0xFFFF));
				NewParamDL((floattov16(_v[1]) & 0xFFFF));
				NewCommandDL(ID_VERTEX16);
			}
		}
	FinishDL();

	if (absf(bigvalue) > 0) {
		printf("\nModel too big for DS! Scale it down.\n");
		printf
		    ("\nDS max. allowed value: +/-7,9997\nModel max. detected value: %f\n\n",
		     bigvalue);
		return -1;
	}

	if (vtxcount > 6144) {
		printf("\nModel has too many vertices!\n");
		printf
		    ("\nDS can only render 6144 vertices per frame.\nYour model has %d vertices.\n",
		     vtxcount);
	}

	printf("\nCreating BIN file...\n");

	// Now, let's save them into a BIN file.
	FILE *file = fopen(outputfilepath, "wb+");
	if (file == NULL) {
		printf("\nCouldn't create %s file!", outputfilepath);
		return -1;
	}
	fwrite((int *)GetDLPointer(0), GetDLSize(0) * sizeof(unsigned int), 1,
	       file);
	fclose(file);

	FILE *test = fopen(outputfilepath, "rb");
	fseek(test, 0, SEEK_END);
	long int size = ftell(test);
	fclose(test);
	printf("\nBIN file size: %zd bytes\n", size);

	printf("\nReady!\n\n");

	return 0;
}
