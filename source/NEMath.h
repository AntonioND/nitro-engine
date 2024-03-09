// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2022 Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include <nds.h>

// Internal math functions

static inline
void ne_div_start(int32_t num, int32_t den)
{
    REG_DIVCNT = DIV_32_32;

    while (REG_DIVCNT & DIV_BUSY);

    REG_DIV_NUMER_L = num;
    REG_DIV_DENOM_L = den;
}

static inline
int32_t ne_div_result(void)
{
    while (REG_DIVCNT & DIV_BUSY);

    return REG_DIV_RESULT_L;
}
