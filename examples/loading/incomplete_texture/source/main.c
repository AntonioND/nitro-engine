// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2024
//
// This file is part of Nitro Engine

// This is an example to show how Nitro Engine can load textures of any height.
// Internally, the NDS thinks that the texture is bigger, but Nitro Engine only
// uses the parts that the user has loaded.
//
// The width needs to be a power of two because:
//
// - Supporting them complicates the loading code a lot.
//
// - Compressed textures can't really be expanded because they are composed
//   by many 4x4 subimages.
//
// - They don't save space in VRAM.
//
// - They save space in the final ROM, but you can achieve the same effect
//   compressing them with LZSS compression, for example.

#include <NEMain.h>

#include "a3pal32.h"
#include "pal4.h"

typedef struct {
    NE_Material *Material, *Material2;
} SceneData;

void Draw3DScene(void *arg)
{
    SceneData *Scene = arg;

    NE_2DViewInit();

    NE_2DDrawTexturedQuad(40, 10,
                          40 + 32, 10 + 100,
                          0, Scene->Material);

    NE_2DDrawTexturedQuad(128, 10,
                          128 + 64, 10 + 100,
                          0, Scene->Material2);
}

int main(int argc, char *argv[])
{
    SceneData Scene = { 0 };

    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    // Init 3D mode
    NE_Init3D();

    // Allocate objects for a material
    Scene.Material = NE_MaterialCreate();
    NE_Palette *Palette = NE_PaletteCreate();

    NE_MaterialTexLoad(Scene.Material,
                       NE_A3PAL32, // Texture type
                       64, 200,    // Width, height (in pixels)
                       NE_TEXGEN_TEXCOORD, a3pal32Bitmap);
    NE_PaletteLoad(Palette, a3pal32Pal, 32, NE_A3PAL32);
    NE_MaterialSetPalette(Scene.Material, Palette);

    // Allocate objects for another material
    Scene.Material2 = NE_MaterialCreate();
    NE_Palette *Palette2 = NE_PaletteCreate();

    NE_MaterialTexLoad(Scene.Material2, NE_PAL4, 64, 100, NE_TEXGEN_TEXCOORD,
                       pal4Bitmap);
    NE_PaletteLoad(Palette2, pal4Pal, 4, NE_PAL4);
    NE_MaterialSetPalette(Scene.Material2, Palette2);

    while (1)
    {
        NE_WaitForVBL(0);

        // Draw 3D scene
        NE_ProcessArg(Draw3DScene, &Scene);
    }

    return 0;
}
