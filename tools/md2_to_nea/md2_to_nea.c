// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2008-2011, 2019, 2022, Antonio Niño Díaz

// I used the information in this web to make the converter:
// http://tfc.duke.free.fr/coding/md2-specs-en.html

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dynamic_list.h"
#include "framemaker.h"

#define MAX_PATHLEN	1024

static inline int32_t floattof32(float n)
{
	return (int32_t) (n * (1 << 12));
}

static inline int16_t floattov16(float n)
{
	return (int16_t) (n * (1 << 12));
}

static inline int16_t floattov10(float n)
{
	if (n > 0.998)
		return 0x7FFF;
	if (n < -0.998)
		return 0xFFFF;
	return (int16_t) (n * (1 << 9));
}

#define absf(x) (((x) > 0) ? (x) : -(x))

//----------------------------------------------------
//                    MD2 structs
//----------------------------------------------------

typedef float vec3_t[3];

typedef struct {
	int32_t ident;
	int32_t version;

	int32_t skinwidth;
	int32_t skinheight;
	int32_t framesize;
	int32_t num_skins;
	int32_t num_vertices;
	int32_t num_st;
	int32_t num_tris;
	int32_t num_glcmds;
	int32_t num_frames;
	int32_t offset_skins;
	int32_t offset_st;
	int32_t offset_tris;
	int32_t offset_frames;
	int32_t offset_glcmds;
	int32_t offset_end;
} md2_header_t;

typedef struct {
	int16_t s;
	int16_t t;
} md2_texCoord_t;

typedef struct {
	uint16_t vertex[3];
	uint16_t st[3];
} md2_triangle_t;

typedef struct {
	uint8_t v[3];
	uint8_t normalIndex;
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
	int32_t index;
} md2_glcmd_t;

float anorms[162][3] = {
#include "anorms.h"
};

//----------------------------------------------------
//                  Converted structs
//----------------------------------------------------

//Frame data -> texture, normal, vertex, texture...

typedef struct {
	int32_t magic;
	int32_t version;

	int32_t num_frames;
	int32_t num_vertices;

	int32_t offset_frames;
	int32_t offset_vtx;
	int32_t offset_norm;
	int32_t offset_st;

} ds_header_t;

typedef int16_t ds_vec3_t[3];

typedef int16_t ds_st_t[2];

//-----------------------------------------------------------

void PrintUsage(void)
{
	printf("Usage:\n");
	printf("    md2_to_nea [input.md2] [output.nea] ([float scale])\n");
	printf("       ([float translate x] [float translate x] [float translate x])\n");
}

int IsValidSize(int size)
{
	return (size == 8 || size == 16 || size == 32 || size == 64 ||
		size == 128 || size == 256 || size == 512 || size == 1024);
}

int main(int argc, char *argv[])
{
	printf("md2_to_nea v2.1\n");
	printf("\n");
	printf("Copyright (c) 2008-2011, 2019 Antonio Nino Diaz\n");
	printf("\n");

	// DEFAULT VALUES
	float general_scale = 1;
	float general_trans[3] = { 0, 0, 0 };

	switch (argc) {
	case 0:
	case 1:
	case 2:
		// Not enough
		PrintUsage();
		return -1;
	case 3:
		// Use default modifications
		break;
	case 4:
		// Use default translation and custom scale
		general_scale = atof(argv[3]);
		break;
	case 5:
	case 6:
		// Custom translation, not enough
		printf("You must set 3 coordinates for translation, not less.");
		PrintUsage();
		return -1;
	case 7:
		// Custom translation + scale
		general_scale = atof(argv[3]);
		general_trans[0] = atof(argv[4]);
		general_trans[1] = atof(argv[5]);
		general_trans[2] = atof(argv[6]);
		break;
	default:
		// The rest...
		PrintUsage();
		return -1;
	}

	char inputfilepath[MAX_PATHLEN];
	char outputfilepath[MAX_PATHLEN];

	strcpy(inputfilepath, argv[1]);
	strcpy(outputfilepath, argv[2]);

	if (general_scale == 0) {
		printf("\nScale can't be 0!!");
		PrintUsage();
		return -1;
	}

	printf("\nScale:     %f\n", general_scale);
	printf("Translate: %f, %f, %f\n", general_trans[0], general_trans[1],
	       general_trans[2]);

	char *md2data;

	printf("\nLoading MD2 model...\n");

	FILE *datafile = fopen(inputfilepath, "r");
	if (datafile != NULL) {
		fseek(datafile, 0, SEEK_END);
		long int size = ftell(datafile);
		rewind(datafile);
		md2data = (char *)malloc(sizeof(char) * size);
		fread(md2data, 1, size, datafile);
		fclose(datafile);
	} else {
		fclose(datafile);
		printf("\n\nCouldn't open %s!!\n\n", inputfilepath);
		PrintUsage();
		return -1;
	}

	md2_header_t *header = (md2_header_t *) md2data;

	if ((header->ident != 844121161) || (header->version != 8)) {
		printf("\n\nWrong file type or version!!\n\n");
		return -1;
	}

	int32_t t_w = header->skinwidth;
    int32_t t_h = header->skinheight;

	if (t_w > 1024 || t_h > 1024) {
		printf("\n\nTexture too big!!\n\n");
		return -1;
	}

	if (!IsValidSize(t_w) || !IsValidSize(t_h)) {
		printf("\nWrong texture size. Must be power of 2.\n");
		printf("\nAlthough the model uses an invalid texture size, it will be converted.\n");
		printf("\nResize the texture to nearest valid size.\n\n");
	}

	while (1) {
		if (IsValidSize(t_w))
			break;
		t_w++;
	}
	while (1) {
		if (IsValidSize(t_h))
			break;
		t_h++;
	}

	int num_tris = header->num_tris;

	md2_frame_t *frame = NULL;
	md2_texCoord_t *texcoord =
	    (md2_texCoord_t *) ((uintptr_t)header->offset_st + (uintptr_t)header);
	md2_triangle_t *triangle =
	    (md2_triangle_t *) ((uintptr_t)header->offset_tris + (uintptr_t)header);

	// Current vertex
	md2_vertex_t *vtx;

	printf("\nMD2 Information:\n\n  Number of frames: %d\n  Texture size: %dx%d",
	    header->num_frames, t_w, t_h);

	printf("\nCreating lists of commands...\n");

	float bigvalue = 0;
	int32_t maxvtxnum = 0;

	InitDynamicLists();

	// Everything ready, let's "draw" all frames
	int n;
	for (n = 0; n < header->num_frames; n++) {
		frame = (md2_frame_t *) ((uintptr_t)header->offset_frames +
                                 (uintptr_t)header + (header->framesize * n));

		NewFrame();

		int32_t vtxcount = 0;

		for (int32_t t = 0; t < num_tris; t++) {
			for (int32_t v = 0; v < 3; v++) {
				vtx = (md2_vertex_t *) ((uintptr_t)(&(frame->verts)));
				vtx = &vtx[triangle[t].vertex[v]];

				// Texture
				int16_t s_ = texcoord[triangle[t].st[v]].s;
				int16_t t_ = texcoord[triangle[t].st[v]].t;

				// This is used to change UVs if using a texture size unsupported by DS
				s_ = (int32_t)((float)(s_ * t_w) / (float)header->skinwidth);
				t_ = (int32_t)((float)(t_ * t_h) / (float)header->skinheight);
				NewFrameData(AddTexCoord(s_ << 4, t_ << 4));	// (t_h-t_)<<4));

				// Normal
				uint16_t norm[3];
				for (int32_t b = 0; b < 3; b++)
					norm[b] = floattov10(anorms[vtx->normalIndex][b]);
				NewFrameData(AddNormal(norm[0], norm[1], norm[2]));

				// Vertex
				vtxcount++;
				float _v[3];
				for (int32_t a = 0; a < 3; a++) {
					_v[a] = ((float)frame->scale[a] * (float)(vtx->v[a])) +
					        (float)frame->translate[a];
					_v[a] += general_trans[a];
					_v[a] *= general_scale;

					if ((absf(_v[a]) > (float)7.9997)
					    && (absf(bigvalue) < absf(_v[a])))
						bigvalue = _v[a];
				}

				NewFrameData(AddVertex(floattov16(_v[0]), floattov16(_v[2]),
				                       floattov16(_v[1])));
			}
		}

		if (maxvtxnum < vtxcount)
			maxvtxnum = vtxcount;
	}

	if (absf(bigvalue) > 0) {
		printf("\nModel too big for DS! Scale it down.\n");
		printf("\nDS max. allowed value: +/-7,9997\nModel max. detected value: %f\n",
		     bigvalue);
	}

	if (maxvtxnum > 6144) {
		printf("\nModel has too many vertices!\n");
		printf("\nDS can only render 6144 vertices per frame.\nYour model has %d vertices.\n",
		     maxvtxnum);
	}

	printf("\nCreating NEA file...\n");

	// Now, let's save them into a NEA file.
	FILE *file = fopen(outputfilepath, "wb+");
	if (file == NULL) {
		printf("\nCouldn't create %s file!", outputfilepath);
		EndDynamicLists();
		return -1;
	}

	ds_header_t temp_header;
	temp_header.magic = 1296123214; // 'NEAM'
	temp_header.version = 2;
	temp_header.num_frames = header->num_frames;
	temp_header.num_vertices = num_tris * 3;
	printf("\nNumber of vertices: %d - Each frame: %d\n",
	       GetVerticesNumber(), temp_header.num_vertices);
	printf("Number of normals: %d\n", GetNormalNumber());
	printf("Number of texture coordinates: %d\n", GetTexcoordsNumber());
	printf("Number of frames: %d\n", temp_header.num_frames);
	temp_header.offset_norm = sizeof(ds_header_t);
	temp_header.offset_st =
	    temp_header.offset_norm + (sizeof(ds_vec3_t) * GetNormalNumber());
	temp_header.offset_vtx =
	    temp_header.offset_st + (sizeof(ds_st_t) * GetTexcoordsNumber());
	temp_header.offset_frames =
	    temp_header.offset_vtx + (sizeof(ds_vec3_t) * GetVerticesNumber());
	fwrite(&temp_header, sizeof(ds_header_t), 1, file);

	// Normals...
	ds_vec3_t temp_vector;
	int32_t number = GetNormalNumber();
	for (int32_t i = 0; i < number; i++) {
		GetNormal(i, (uint16_t *)&temp_vector[0], (uint16_t *)&temp_vector[1],
                     (uint16_t *)&temp_vector[2]);
		fwrite(&temp_vector, sizeof(ds_vec3_t), 1, file);
	}

	// Texcoords
	ds_st_t temp_texcoord;
	number = GetTexcoordsNumber();
	for (int32_t i = 0; i < number; i++) {
		GetTexCoord(i, (uint16_t *)&temp_texcoord[0],
                       (uint16_t *)&temp_texcoord[1]);
		fwrite(&temp_texcoord, sizeof(ds_st_t), 1, file);
	}

	// Vertices
	number = GetVerticesNumber();
	for (int32_t i = 0; i < number; i++) {
		GetVertex(i, (uint16_t *)&temp_vector[0], (uint16_t *)&temp_vector[1],
                     (uint16_t *)&temp_vector[2]);
		fwrite(&temp_vector, sizeof(ds_vec3_t), 1, file);
	}

	printf("\nSize of a frame: %ld\n",
	       (long int)(GetFrameSize(0) * sizeof(uint16_t)));

	// Frames
	for (int32_t i_ = 0; i_ < header->num_frames; i_++) {
		fwrite((int *)GetFramePointer(i_), 1,
		       GetFrameSize(i_) * sizeof(uint16_t), file);
	}

	fclose(file);

	FILE *test = fopen(outputfilepath, "rb");
	fseek(test, 0, SEEK_END);
	long int size = ftell(test);
	fclose(test);

	printf("\nNEA file size: %ld bytes", size);
	printf("\n\nReady!\n\n");

	EndDynamicLists();

	return 0;
}
