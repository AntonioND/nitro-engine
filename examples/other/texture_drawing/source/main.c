// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "texture_tex_bin.h"

NE_Camera *Camera;
NE_Material *Material;

void Draw3DScene(void)
{
    NE_CameraUse(Camera);

    // This set material's color to drawing color (default = white)
    NE_MaterialUse(Material);

    // In general you should avoid using the functions below

    // Begin drawing
    NE_PolyBegin(GL_QUAD);

        NE_PolyColor(NE_Red);    // Set next vertices color
        NE_PolyTexCoord(0, 0);   // Texture coordinates
        NE_PolyVertex(-1, 1, 0); // Send new vertex

        NE_PolyColor(NE_Blue);
        NE_PolyTexCoord(0, 64);
        NE_PolyVertex(-1, -1, 0);

        NE_PolyColor(NE_Green);
        NE_PolyTexCoord(64, 64);
        NE_PolyVertex(1, -1, 0);

        NE_PolyColor(NE_Yellow);
        NE_PolyTexCoord(64, 0);
        NE_PolyVertex(1, 1, 0);

    // Apparently this command is ignored by the GPU
    NE_PolyEnd();
}

int main(void)
{
    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    NE_Init3D();

    Camera = NE_CameraCreate();
    Material = NE_MaterialCreate();

    NE_CameraSet(Camera,
                 0, 0, 2,
                 0, 0, 0,
                 0, 1, 0);

    NE_MaterialTexLoad(Material, GL_RGBA, 128, 128, TEXGEN_TEXCOORD,
                       (u8 *)texture_tex_bin);

    while (1)
    {
        NE_Process(Draw3DScene);
        NE_WaitForVBL(0);
    }

    return 0;
}
