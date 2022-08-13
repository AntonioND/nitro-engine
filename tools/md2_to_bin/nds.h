// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2008-2011, 2019, Antonio Niño Díaz

#ifndef NDS_H__
#define NDS_H__

#include <stdint.h>

//From libnds/gbatek

static inline int32_t floattof32(float n)
{
    return (int32_t)(n * (1 << 12));
}

static inline int16_t floattov16(float n)
{
    return (int16_t)(n * (1 << 12));
}

static inline int16_t floattov10(float n)
{
    if (n > 0.998)
        return 0x1FF;
    if (n < -0.998)
        return 0x3FF;
    return (int16_t)(n * (1 << 9));
}

#define VERTEX_10_PACK(x,y,z) \
    (((x) & 0x3FF) | (((y) & 0x3FF) << 10) | (((z) & 0x3FF) << 20))

#define NORMAL_PACK(x,y,z) \
    (((x) & 0x3FF) | (((y) & 0x3FF) << 10) | ((z) << 20))

#define TEXTURE_PACK(u,v) \
    (((u) & 0xFFFF) | ((v) << 16))

#define COMMAND_PACK(c1, c2, c3, c4) \
    (((c4) << 24) | ((c3) << 16) | ((c2) << 8) | (c1))

#define ID_NOP          0x00
#define ID_VERTEX16     0x23
#define ID_VERTEX10     0x24
#define ID_TEX_COORD    0x22
#define ID_NORMAL       0x21
#define ID_BEGIN        0x40

#endif // NDS_H__
