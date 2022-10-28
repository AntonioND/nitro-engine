// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, 2022 Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include "NEMain.h"

/// @file NEText.c

typedef struct {
    int sizex, sizey;
    const NE_Material *material;
} ne_textinfo_t;

static ne_textinfo_t NE_TextInfo[NE_MAX_TEXT_FONTS];

static int NE_TEXT_PRIORITY = 0;
#define TEXT_SIZE_3D 0.1

void NE_TextPrioritySet(int priority)
{
    NE_TEXT_PRIORITY = priority;
}

void NE_TextPriorityReset(void)
{
    NE_TEXT_PRIORITY = 0;
}

void NE_TextInit(int slot, const NE_Material *mat, int sizex, int sizey)
{
    NE_AssertMinMax(0, slot, NE_MAX_TEXT_FONTS, "Invalid slot %d", slot);
    NE_AssertPointer(mat, "NULL pointer");

    NE_TextInfo[slot].sizex = sizex;
    NE_TextInfo[slot].sizey = sizey;
    NE_TextInfo[slot].material = mat;
}

void NE_TextEnd(int slot)
{
    NE_AssertMinMax(0, slot, NE_MAX_TEXT_FONTS, "Invalid slot %d", slot);

    NE_TextInfo[slot].sizex = 0;
    NE_TextInfo[slot].sizey = 0;
    NE_TextInfo[slot].material = NULL;
}

void NE_TextResetSystem(void)
{
    for (int i = 0; i < NE_MAX_TEXT_FONTS; i++)
    {
        NE_TextInfo[i].sizex = 0;
        NE_TextInfo[i].sizey = 0;
        NE_TextInfo[i].material = NULL;
    }
}

static void _ne_texturecuadprint(int xcrd1, int ycrd1, int xcrd2, int ycrd2,
                                 int xtx1, int ytx1, int xtx2, int ytx2)
{
    GFX_TEX_COORD = TEXTURE_PACK(inttot16(xtx1), inttot16(ytx1));
    GFX_VERTEX16 = (ycrd1 << 16) | (xcrd1 & 0xFFFF);
    GFX_VERTEX16 = NE_TEXT_PRIORITY;

    GFX_TEX_COORD = TEXTURE_PACK(inttot16(xtx1), inttot16(ytx2));
    GFX_VERTEX_XY = (ycrd2 << 16) | (xcrd1 & 0xFFFF);

    GFX_TEX_COORD = TEXTURE_PACK(inttot16(xtx2), inttot16(ytx2));
    GFX_VERTEX_XY = (ycrd2 << 16) | (xcrd2 & 0xFFFF);

    GFX_TEX_COORD = TEXTURE_PACK(inttot16(xtx2), inttot16(ytx1));
    GFX_VERTEX_XY = (ycrd1 << 16) | (xcrd2 & 0xFFFF);
}

static void _ne_charprint(const ne_textinfo_t * textinfo, int xcrd1, int ycrd1,
                          char character)
{
    // Texture coords
    int xcoord = ((character & 31) * textinfo->sizex);
    int xcoord2 = (xcoord + textinfo->sizex);

    int ycoord = ((character >> 5) * textinfo->sizey);
    int ycoord2 = ycoord + textinfo->sizey;

    _ne_texturecuadprint(xcrd1, ycrd1,
                         xcrd1 + textinfo->sizex, ycrd1 + textinfo->sizey,
                         xcoord, ycoord, xcoord2, ycoord2);
}

static void _ne_texturecuadprint3D(float xcrd1, float ycrd1, float textXPos, float textYPos, float textZPos, float sizeX, float sizeY,
                                   int xtx1, int ytx1, int xtx2, int ytx2, float xAngle, float yAngle, float zAngle, int maxCharWidth, int rowCount, float scale)
{
    glPushMatrix();

    // Set postion for rotation
    glTranslatef32(textXPos * 4096, textYPos * 4096, textZPos * 4096);

    // Scale text
    float scaleF = scale * 4096;
    glScalef32(scaleF, scaleF, scaleF);

    // Rotate text
    glRotateX(xAngle);
    glRotateY(yAngle);
    glRotateZ(zAngle);

    // Set position to middle of the text
    glTranslatef32((xcrd1 - textXPos - (sizeX * (maxCharWidth) / 2.0)) * 4096, (ycrd1 - textYPos + (sizeY * (rowCount - 1) / 2.0) - sizeY / 2.0) * 4096, 0);

    // Create quad
    float sizeXF = sizeX * 4096;
    float sizeYF = sizeY * 4096;

    GFX_TEX_COORD = TEXTURE_PACK(inttot16(xtx1), inttot16(ytx2));
    glVertex3v16(0, 0, 0);

    GFX_TEX_COORD = TEXTURE_PACK(inttot16(xtx1), inttot16(ytx1));
    glVertex3v16(0, sizeYF, 0);

    GFX_TEX_COORD = TEXTURE_PACK(inttot16(xtx2), inttot16(ytx1));
    glVertex3v16(sizeXF, sizeYF, 0);

    GFX_TEX_COORD = TEXTURE_PACK(inttot16(xtx2), inttot16(ytx2));
    glVertex3v16(sizeXF, 0, 0);

    glPopMatrix(1);
}

static void _ne_charprint3D(const ne_textinfo_t *textinfo, float xcrd1, float ycrd1, float textXPos, float textYPos, float textZPos, float xAngle, float yAngle, float zAngle,
                            char character, int maxCharWidth, int rowCount, float scale)
{
    // Texture coords
    int xcoord = ((character & 31) * textinfo->sizex);
    int xcoord2 = (xcoord + textinfo->sizex);

    int ycoord = ((character >> 5) * textinfo->sizey);
    int ycoord2 = ycoord + textinfo->sizey;

    _ne_texturecuadprint3D(xcrd1, ycrd1, textXPos, textYPos, textZPos,
                           textinfo->sizex * TEXT_SIZE_3D, textinfo->sizey * TEXT_SIZE_3D,
                           xcoord, ycoord, xcoord2, ycoord2, xAngle, yAngle, zAngle, maxCharWidth, rowCount, scale);
}

int NE_TextPrint3D(int slot, float x, float y, float z, float xAngle, float yAngle, float zAngle, float scale, u32 color, const char *text)
{
    NE_AssertMinMax(0, slot, NE_MAX_TEXT_FONTS, "Invalid slot %d", slot);

    const ne_textinfo_t *textinfo = &NE_TextInfo[slot];

    if (textinfo->material == NULL)
        return -1;

    NE_MaterialUse(textinfo->material);
    GFX_COLOR = color;

    int count = 0;
    float x_ = x, y_ = y;

    GFX_BEGIN = GL_QUADS;
    int lineCharCount = 0;
    int maxCharWidth = 0;
    int lineCount = 1;

    // Get the number of line to show and the number of characters of the longest line
    while (1)
    {
        if (text[count] == '\0') // End of text
        {
            if (lineCharCount > maxCharWidth)
            {
                if (maxCharWidth == 0)
                    maxCharWidth = lineCharCount;
                else
                    maxCharWidth = lineCharCount - 1;
            }
            lineCharCount = 0;
            break;
        }
        else if (text[count] == '\n') // New line
        {
            if (lineCharCount > maxCharWidth)
            {
                maxCharWidth = lineCharCount - 1;
            }
            lineCharCount = 0;
            lineCount++;
        }
        lineCharCount++;
        count++;
    }

    count = 0;
    while (1)
    {
        if (text[count] == '\0') // End of text
        {
            break;
        }
        else if (text[count] == '\n') // New line
        {
            y_ -= textinfo->sizey * TEXT_SIZE_3D;
            x_ = x;
            count++;
        }
        else
        {
            _ne_charprint3D(textinfo, x_, y_, x, y, z, xAngle, yAngle, zAngle, text[count], maxCharWidth, lineCount, scale);
            count++;
            x_ += textinfo->sizex * TEXT_SIZE_3D;
        }
    }

    return count;
}

int NE_TextPrint(int slot, int x, int y, u32 color, const char *text)
{
    NE_AssertMinMax(0, slot, NE_MAX_TEXT_FONTS, "Invalid slot %d", slot);

    const ne_textinfo_t *textinfo = &NE_TextInfo[slot];

    if (textinfo->material == NULL)
        return -1;

    NE_MaterialUse(textinfo->material);
    GFX_COLOR = color;

    int count = 0;
    int x_ = x * textinfo->sizex, y_ = y * textinfo->sizey;

    GFX_BEGIN = GL_QUADS;

    while (1)
    {
        if (text[count] == '\0')
        {
            break;
        }
        else if (text[count] == '\n')
        {
            y_ += textinfo->sizey;
            x_ = 0;
            count++;
        }
        else
        {
            if (x_ > 255)
            {
                y_ += textinfo->sizey;
                x_ = 0;
            }
            if (y_ > 191)
                break;

            _ne_charprint(textinfo, x_, y_, text[count]);

            count++;
            x_ += textinfo->sizex;
        }
    }

    return count;
}

int NE_TextPrintBox(int slot, int x, int y, int endx, int endy, u32 color,
                    int charnum, const char *text)
{
    NE_AssertMinMax(0, slot, NE_MAX_TEXT_FONTS, "Invalid slot %d", slot);

    const ne_textinfo_t *textinfo = &NE_TextInfo[slot];

    if (textinfo->material == NULL)
        return -1;

    NE_MaterialUse(textinfo->material);
    GFX_COLOR = color;

    int count = 0;
    int x_ = x * textinfo->sizex, y_ = y * textinfo->sizey;
    int xlimit = endx * textinfo->sizex, ylimit = endy * textinfo->sizey;
    ylimit = (ylimit > 191) ? 191 : ylimit;

    if (charnum < 0)
        charnum = 0x0FFFFFFF;

    GFX_BEGIN = GL_QUADS;

    while (1)
    {
        if (charnum <= count)
        {
            break;
        }
        else if (text[count] == '\0')
        {
            break;
        }
        else if (text[count] == '\n')
        {
            y_ += textinfo->sizey;
            x_ = x * textinfo->sizex;
            count++;
        }
        else
        {
            if (x_ > xlimit)
            {
                y_ += textinfo->sizey;
                x_ = x * textinfo->sizex;
            }
            if (y_ > ylimit)
                break;

            _ne_charprint(textinfo, x_, y_, text[count]);

            count++;
            x_ += textinfo->sizex;
        }
    }

    return count;
}

int NE_TextPrintFree(int slot, int x, int y, u32 color, const char *text)
{
    NE_AssertMinMax(0, slot, NE_MAX_TEXT_FONTS, "Invalid slot %d", slot);

    const ne_textinfo_t *textinfo = &NE_TextInfo[slot];

    if (textinfo->material == NULL)
        return -1;

    NE_MaterialUse(textinfo->material);
    GFX_COLOR = color;

    int count = 0;
    int x_ = x, y_ = y;

    GFX_BEGIN = GL_QUADS;

    while (1)
    {
        if (text[count] == '\0')
        {
            break;
        }
        else
        {
            if (x_ > 255)
                break;

            _ne_charprint(textinfo, x_, y_, text[count]);

            count++;
            x_ += textinfo->sizex;
        }
    }

    return count;
}

int NE_TextPrintBoxFree(int slot, int x, int y, int endx, int endy, u32 color,
                        int charnum, const char *text)
{
    NE_AssertMinMax(0, slot, NE_MAX_TEXT_FONTS, "Invalid slot %d", slot);

    ne_textinfo_t *textinfo = &NE_TextInfo[slot];

    if (textinfo->material == NULL)
        return -1;

    NE_MaterialUse(textinfo->material);
    GFX_COLOR = color;

    int count = 0;
    int x_ = x, y_ = y;
    int xlimit = endx;
    int ylimit = (endy > 191) ? 191 : endy;

    if (charnum < 0)
        charnum = 0x0FFFFFFF;

    GFX_BEGIN = GL_QUADS;

    while (1)
    {
        if (charnum <= count)
        {
            break;
        }
        else if (text[count] == '\0')
        {
            break;
        }
        else if (text[count] == '\n')
        {
            y_ += textinfo->sizey;
            x_ = x;
            count++;
        }
        else
        {
            if (x_ > xlimit)
            {
                y_ += textinfo->sizey;
                x_ = x;
            }
            if (y_ > ylimit)
                break;

            _ne_charprint(textinfo, x_, y_, text[count]);

            count++;
            x_ += textinfo->sizex;
        }
    }

    return count;
}
