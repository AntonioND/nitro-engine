// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2008-2011, 2019, Antonio Niño Díaz

#ifndef _NDS_H_
#define _NDS_H_

//From libnds/gbatek

typedef short int v16;
typedef short int v10;
typedef short int vtx10;

#define floattov16(n)        ((v16)((n) * (1 << 12)))

static inline short int floattov10(float n)
{
	if(n > 0.998) return 0x1FF;
	if(n < -0.998) return 0x3FF;
	return (short int)(n*(1<<9));
}

#define floattof32(n)		 ((int)((n) * (1 << 12)))

#define VERTEX_10_PACK(x,y,z) (((x) & 0x3FF) | (((y) & 0x3FF) << 10) | (((z) & 0x3FF) << 20)) 
#define NORMAL_PACK(x,y,z)   (((x) & 0x3FF) | (((y) & 0x3FF) << 10) | ((z) << 20))
#define TEXTURE_PACK(u,v)    (((u) & 0xFFFF) | ((v) << 16))

#define COMMAND_PACK(c1,c2,c3,c4) (((c4) << 24) | ((c3) << 16) | ((c2) << 8) | (c1))

#define ID_NOP				0x00
#define ID_VERTEX16			0x23
#define ID_VERTEX10			0x24
#define ID_TEX_COORD		0x22
#define ID_NORMAL			0x21
#define ID_BEGIN			0x40			

#endif
