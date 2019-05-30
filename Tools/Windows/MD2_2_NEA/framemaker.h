
#ifndef _FRAMEMAKER_H_
#define _FRAMEMAKER_H_

#define DEFAULT_FRAME_SIZE (512*1024)

void NewFrame(void);
void NewFrameData(unsigned short data);

unsigned int GetFrameSize(int num);
unsigned short *GetFramePointer(int num);

void InitDynamicLists(void);
void EndDynamicLists(void);

int AddVertex(unsigned short x, unsigned short y, unsigned short z);
int AddNormal(unsigned short x, unsigned short y, unsigned short z);
int AddTexCoord(unsigned short u, unsigned short v);

int GetVerticesNumber(void);
int GetNormalNumber(void);
int GetTexcoordsNumber(void);

void GetVertex(int index, unsigned short *x, unsigned short *y,
	       unsigned short *z);
void GetNormal(int index, unsigned short *x, unsigned short *y,
	       unsigned short *z);
void GetTexCoord(int index, unsigned short *u, unsigned short *v);

#endif
