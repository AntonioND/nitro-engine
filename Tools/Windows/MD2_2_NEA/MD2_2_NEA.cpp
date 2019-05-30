// MD2_2_NEA.cpp : Defines the entry point for the console application.
//

//I used the information in this web to make the converter:
//http://tfc.duke.free.fr/coding/md2-specs-en.html
//Thanks.

#include "stdafx.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>

#include "framemaker.h"
#include "dynamic_list.h"

#define MAX_PATHLEN    1024

typedef     signed int  s32;
typedef	  unsigned int  u32 ;
typedef   signed short  s16 ;
typedef unsigned short  u16 ;
typedef    signed char  s8  ;
typedef  unsigned char  u8  ;

inline s32 floattof32(float n)
{
	return (s32)(n * (1 << 12));
}

inline s16 floattov16(float n)
{
	return (s16)(n * (1 << 12));
}

inline s16 floattov10(float n)
{
	if(n > 0.998) return 0x7FFF;
	if(n < -0.998) return 0xFFFF;
	return (s16)(n*(1<<9));
}



#define absf(x) (((x) > 0) ? (x) : -(x))

//----------------------------------------------------
//                    MD2 structs
//----------------------------------------------------

typedef float vec3_t[3];

typedef struct 
{
  int ident; int version;

  int skinwidth; int skinheight;
  int framesize;
  int num_skins; int num_vertices; int num_st; int num_tris; int num_glcmds; int num_frames;
  int offset_skins; int offset_st; int offset_tris; int offset_frames; int offset_glcmds;
  int offset_end;
} md2_header_t;

typedef struct { short s; short t; } md2_texCoord_t;

typedef struct { unsigned short vertex[3]; unsigned short st[3]; } md2_triangle_t;

typedef struct { unsigned char v[3]; unsigned char normalIndex; } md2_vertex_t;

typedef struct {
  vec3_t scale; vec3_t translate;
  char name[16];
  md2_vertex_t *verts;
} md2_frame_t;

typedef struct { float s; float t; int index; } md2_glcmd_t;

float anorms[162][3] = { 
#include "anorms.h"
};


//----------------------------------------------------
//                  Converted structs
//----------------------------------------------------

//Frame data -> texture, normal, vertex, texture...

typedef struct 
{
  s32 magic; s32 version;

  s32 num_frames; s32 num_vertices;

  s32 offset_frames;
  s32 offset_vtx;
  s32 offset_norm;
  s32 offset_st;
  
} ds_header_t;

typedef s16 ds_vec3_t[3];

typedef s16 ds_st_t[2];

//-----------------------------------------------------------


void PrintUse(void)
{
printf("\n\nInstructions:\n\n");
printf("    MD2_2_NEA [input.md2] [output.nea] ([float scale])\n");
printf("       ([float translate x] [float translate x] [float translate x])\n\n");
}

int main(int argc, char* argv[])
{
	printf("\n\n       ########################################\n");
	printf("       #/                                    \\#\n");
	printf("       #   --    --    MD2 2 NEA    --   --   #\n");
	printf("       #\\                              V2.0  /#\n");
	printf("       ########################################\n");

	printf("\nCopyright (C) 2008 Antonio Ni%co D%caz\nAll rights reserved.\n\n", 164, 161);

	printf("   --   --   --   --   --   --   --   --   --   --\n");

	float general_scale = 1; //DEFAULT VALUES
	float general_trans[3] = {0, 0, 0};

	switch(argc)
		{
		case 0:
		case 1:
		case 2:  //Not enough
			PrintUse();
			return -1;
		case 3:  //Use default modifications
			break; 
		case 4:  //Use default translation and custom scale
			general_scale = atof(argv[3]); 
			break;
		case 5:
		case 6:  //Custom translation, not enough
			printf("You must set 3 coordinates for translation, not less.");
			PrintUse();
			return -1;
		case 7:  //Custom translation + scale
			general_scale = atof(argv[3]); 
			general_trans[0] = atof(argv[4]);
			general_trans[1] = atof(argv[5]);
			general_trans[2] = atof(argv[6]);
			break;
		default: //The rest...
			PrintUse();
			return -1;
		}
	
	char inputfilepath[MAX_PATHLEN];
	char outputfilepath[MAX_PATHLEN];
	
	strcpy(inputfilepath,argv[1]);
	strcpy(outputfilepath,argv[2]);
	
	if(general_scale == 0) 
		{ 
		printf("\nScale can't be 0!!");
		PrintUse(); 
		return -1; 
		}
	
	printf("\nScale:     %f\n",general_scale);
	printf("Translate: %f, %f, %f\n",general_trans[0],general_trans[1],general_trans[2]);
	
	char * md2data;

	printf("\nLoading MD2 model...\n");

	FILE * datafile = fopen (inputfilepath, "r");
	if(datafile != NULL)
		{
		fseek (datafile , 0 , SEEK_END);
		long int tamano = ftell (datafile);
		rewind (datafile);
		md2data = (char*) malloc (sizeof(char)*tamano);
		fread (md2data,1,tamano,datafile); 
		fclose (datafile);		
		}
	else 
		{ fclose (datafile); printf("\n\nCouldn't open %s!!\n\n", inputfilepath); 
		PrintUse(); return -1; }
	
	md2_header_t * header = (md2_header_t*)md2data;
	
	if ((header->ident != 844121161) || (header->version != 8))
		{ printf("\n\nWrong file type or version!!\n\n"); return -1; }
	
	int t_w = header->skinwidth, t_h = header->skinheight;

	if(t_w > 1024 || t_h > 1024) { printf("\n\nTexture too big!!\n\n"); return -1; }

	if(!(t_w==8||t_w==16||t_w==32||t_w==64||t_w==128||t_w==256||t_w==512||t_w==1024) ||
		!(t_h==8||t_h==16||t_h==32||t_h==64||t_h==128||t_h==256||t_h==512||t_h==1024))
			{ 
			printf("\nWrong texture size. Must be power of 2.\n"); 
			printf("\nAlthough the model uses an invalid texture size, it will be converted.\n");
			printf("\nResize the texture to nearest valid size.\n\n"); 
			//return -1; 
			}
	
	while(1)
		{ if(t_w==8||t_w==16||t_w==32||t_w==64||t_w==128||t_w==256||t_w==512||t_w==1024) break;
		t_w ++; }
	while(1)
		{ if(t_h==8||t_h==16||t_h==32||t_h==64||t_h==128||t_h==256||t_h==512||t_h==1024) break;
		t_h ++; }

	int num_tris = header->num_tris;

	md2_frame_t * frame = NULL;
	md2_texCoord_t * texcoord = (md2_texCoord_t*)((int)header->offset_st + (int)header);
	md2_triangle_t * triangle = (md2_triangle_t*)((int)header->offset_tris + (int)header);

	md2_vertex_t * vtx; //Current vertex

	printf("\nMD2 Information:\n\n  Number of frames: %d\n  Texture size: %dx%d", 
		header->num_frames, t_w,t_h);

	printf("\nCreating lists of commands...\n"); 

	float bigvalue = 0;
	int maxvtxnum = 0;

	InitDynamicLists();

	//Everything ready, let's "draw" all frames
	int n;
	for(n = 0; n < header->num_frames; n++)
		{
		frame = (md2_frame_t*)((int)header->offset_frames + (int)header + 
			(header->framesize * n));

		NewFrame();
		
		int vtxcount = 0;

		int t = 0, v = 0;
		for(t = 0; t < num_tris; t ++)  for(v = 0; v < 3; v++)
			{
			vtx = (md2_vertex_t*)  (  (int) (&(frame->verts))  );
			vtx = &vtx[triangle[t].vertex[v]];
			
			//Texture
			short s_ = texcoord[triangle[t].st[v]].s;
			short t_ = texcoord[triangle[t].st[v]].t;
		
			//This is used to change UVs if using a texture size unsupported by DS
			s_ = (int)((float)(s_ * t_w) / (float)header->skinwidth);
			t_ = (int)((float)(t_ * t_h) / (float)header->skinheight);
			NewFrameData(AddTexCoord(s_<<4,t_<<4));   //(t_h-t_)<<4));
			
			//Normal
			unsigned short norm[3];
			int b;
			for(b = 0; b < 3; b++) norm[b] = floattov10(anorms[vtx->normalIndex][b]);
			NewFrameData(AddNormal(norm[0],norm[1],norm[2]));

			//Vertex
			vtxcount ++;
			float _v[3];
			int a = 0;
			for(a = 0; a < 3; a++) 
				{ 
				_v[a] = ((float)frame->scale[a] *  (float)(vtx->v[a])) + (float)frame->translate[a];
				_v[a] += general_trans[a];
				_v[a] *= general_scale;
				
				if((absf(_v[a]) > (float)7.9997) && ( absf(bigvalue) < absf(_v[a]))) 
					bigvalue = _v[a];
				}
			
			NewFrameData(AddVertex(floattov16(_v[0]),floattov16(_v[2]),floattov16(_v[1])));
			}

		if(maxvtxnum < vtxcount) maxvtxnum = vtxcount;
		}

	if(absf(bigvalue) > 0)
		{
		printf("\nModel too big for DS! Scale it down.\n");
		printf("\nDS max. allowed value: +/-7,9997\nModel max. detected value: %f\n",bigvalue);
		}
	
	if(maxvtxnum > 6144)
		{
		printf("\nModel has too many vertices!\n");
		printf("\nDS can only render 6144 vertices per frame.\nYour model has %d vertices.\n",maxvtxnum);
		}

	printf("\nCreating NEA file...\n");

	//Now, let's save them into a NEA file.
	FILE * file = fopen(outputfilepath,"wb+");
	if(file == NULL)
		{
		printf("\nCouldn't create %s file!",outputfilepath);
		EndDynamicLists();
		return -1;
		}
	
	ds_header_t temp_header;
	temp_header.magic = 1296123214; // 'NEAM'
	temp_header.version = 2;
	temp_header.num_frames = header->num_frames;
	temp_header.num_vertices = num_tris * 3;
	printf("\nNumber of vertices: %d - Each frame: %d\n", GetVerticesNumber(), temp_header.num_vertices);
	printf("Number of normals: %d\n",GetNormalNumber());
	printf("Number of texture coordinates: %d\n",GetTexcoordsNumber());
	printf("Number of frames: %d\n",temp_header.num_frames);
	temp_header.offset_norm = sizeof(ds_header_t);
	temp_header.offset_st = temp_header.offset_norm + (sizeof(ds_vec3_t) * GetNormalNumber());
	temp_header.offset_vtx = temp_header.offset_st + (sizeof(ds_st_t) * GetTexcoordsNumber());
	temp_header.offset_frames = temp_header.offset_vtx + (sizeof(ds_vec3_t) * GetVerticesNumber());
	fwrite(&temp_header,sizeof(ds_header_t),1,file); 
	 
	//Normals...
	ds_vec3_t temp_vector;
	int number = GetNormalNumber();
	int	i_;
	for(i_ = 0; i_ < number; i_++)
	{
		GetNormal(i_,(u16*)&temp_vector[0],(u16*)&temp_vector[1],(u16*)&temp_vector[2]);
		fwrite(&temp_vector,sizeof(ds_vec3_t),1,file); 
	}

	//Texcoords
	ds_st_t temp_texcoord;
	number = GetTexcoordsNumber();
	for(i_ = 0; i_ < number; i_++)
	{
		GetTexCoord(i_,(u16*)&temp_texcoord[0],(u16*)&temp_texcoord[1]);
		fwrite(&temp_texcoord,sizeof(ds_st_t),1,file); 
	}

	//Vertices
	number = GetVerticesNumber();
	for(i_ = 0; i_ < number; i_++)
	{
		GetVertex(i_,(u16*)&temp_vector[0],(u16*)&temp_vector[1],(u16*)&temp_vector[2]);
		fwrite(&temp_vector,sizeof(ds_vec3_t),1,file); 
	}

	printf("\nSize of a frame: %d\n",GetFrameSize(0)*sizeof(unsigned short));

	//Frames
	for(i_ = 0; i_ < header->num_frames; i_++)
	{
		fwrite((int*)GetFramePointer(i_),1,GetFrameSize(i_)*sizeof(unsigned short),file);
	}
	
	fclose(file);
	
	FILE * test = fopen(outputfilepath,"rb");
	fseek (test , 0 , SEEK_END);
	long int size = ftell (test);
	fclose(test);

	printf("\nNEA file size: %d bytes", size);

	if(size > 1 * 1024 * 1024) printf("  --  Quite big, isn't it?");

	printf("\n\nReady!\n\n");

	EndDynamicLists();

	return 0;
}
