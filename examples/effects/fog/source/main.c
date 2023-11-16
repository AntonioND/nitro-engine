// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "texture_tex_bin.h"
#include "sphere_bin.h"

NE_Camera *Camera;
NE_Model *Model, *Model2, *Model3;
NE_Material *Material;

void Draw3DScene(void)
{
    // Set camera
    NE_CameraUse(Camera);

    // This has to be used to use fog
    NE_PolyFormat(31, 0, NE_LIGHT_ALL, NE_CULL_BACK, NE_FOG_ENABLE);

    // Draw models
    NE_ModelDraw(Model);
    NE_ModelDraw(Model2);
    NE_ModelDraw(Model3);
}

int main(void)
{
    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_VBLANK, NE_HBLFunc);

    // Init console and Nitro Engine
    NE_Init3D();
    // Use banks A and B for textures. libnds uses bank C for the demo text
    // console.
    NE_TextureSystemReset(0, 0, NE_VRAM_AB);
    consoleDemoInit();

    // Allocate objects
    Model = NE_ModelCreate(NE_Static);
    Model2 = NE_ModelCreate(NE_Static);
    Model3 = NE_ModelCreate(NE_Static);
    Camera = NE_CameraCreate();
    Material = NE_MaterialCreate();

    // Set camera coordinates
    NE_CameraSet(Camera,
                 -1, 2, -1,
                  1, 1, 1,
                  0, 1, 0);

    // Load models
    NE_ModelLoadStaticMesh(Model, (u32 *)sphere_bin);
    NE_ModelLoadStaticMesh(Model2, (u32 *)sphere_bin);
    NE_ModelLoadStaticMesh(Model3, (u32 *)sphere_bin);

    // Load texture
    NE_MaterialTexLoad(Material, NE_RGB5, 256, 256, NE_TEXGEN_TEXCOORD,
                       (u8 *)texture_tex_bin);

    // Assign the same material to every model object.
    NE_ModelSetMaterial(Model, Material);
    NE_ModelSetMaterial(Model2, Material);
    NE_ModelSetMaterial(Model3, Material);

    // Set light and vector of light 0
    NE_LightSet(0, NE_White, 0, -1, -1);

    // Set position of every object
    NE_ModelSetCoord(Model, 1, 0, 1);
    NE_ModelSetCoord(Model2, 3, 1, 3);
    NE_ModelSetCoord(Model3, 7, 2, 7);

    // Set initial fog color to black
    u32 color = NE_Black;

    // Some parameters
    u16 depth = 0x7C00;
    u8 shift = 5;
    u8 mass = 2;

    while (1)
    {
        NE_WaitForVBL(0);

        // Refresh keys
        scanKeys();
        uint32_t keys = keysDown();

        // Modify parameters
        if (keys & KEY_UP)
            shift ++;
        if (keys & KEY_DOWN)
            shift --;
        if (keys & KEY_X)
            mass ++;
        if(keys & KEY_B)
            mass --;
        if (keysHeld() & KEY_R)
            depth += 0x20;
        if(keysHeld() & KEY_L)
            depth -= 0x20;

        // Wrap values of parameters
        shift &= 0xF;
        mass &= 7;
        depth = (depth & 0x0FFF) + 0x7000;

        // Set fog color
        if (keys & KEY_START)
            color = NE_Black;
        if (keys & KEY_SELECT)
            color = NE_White;

        // Enable/update fog
        NE_FogEnable(shift, color, 31, mass, depth);

        printf("\x1b[0;0H"
               "Up/Down - Shift: %d \nX/B - Mass: %d  \n"
               "L/R - Depth: 0x%x   \nSelect/Start - Change color.",
               shift, mass, depth);

        // Draw scene
        NE_Process(Draw3DScene);
    }

    return 0;
}
