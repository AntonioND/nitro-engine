// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "nitrocat_bin.h"
#include "texture_bin.h"

NE_Camera *Camera;
NE_Model *Model;
NE_Material *Material;

int shading, alpha, id;

void Draw3DScene(void)
{
    // Set camera
    NE_CameraUse(Camera);

    // Set polygon format
    NE_PolyFormat(alpha, id, NE_LIGHT_0, NE_CULL_BACK, shading);

    // Draw model
    NE_ModelDraw(Model);
}

int main(void)
{
    // This is needed for special screen effects
    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_VBLANK, NE_HBLFunc);

    // Init console and Nitro Engine
    NE_Init3D();
    // Use banks A and B for textures. libnds uses bank C for the demo text
    // console.
    NE_TextureSystemReset(0, 0, NE_VRAM_AB);
    // This is needed to print text
    consoleDemoInit();

    // Allocate the objects we will use
    Model = NE_ModelCreate(NE_Static);
    Camera = NE_CameraCreate();
    Material = NE_MaterialCreate();

    // Set camera coordinates
    NE_CameraSet(Camera,
                 -8, 0, 0,
                  0, 0, 0,
                  0, 1, 0);

    // Load mesh from RAM and assign it to a model
    NE_ModelLoadStaticMesh(Model, (u32 *)nitrocat_bin);
    // Load texture from RAM and assign it to a material
    NE_MaterialTexLoad(Material, GL_RGB, 128, 128, TEXGEN_TEXCOORD,
                       (u8 *)texture_bin);

    // Assign material to the model
    NE_ModelSetMaterial(Model, Material);

    // Set some propierties to the material
    NE_MaterialSetPropierties(Material,
                  RGB15(24, 24, 24), // Diffuse
                  RGB15(8, 8, 8),    // Ambient
                  RGB15(0, 0, 0),    // Specular
                  RGB15(0, 0, 0),    // Emission
                  false, false);     // Vertex color, use shininess table

    // Setup a light and its color
    NE_LightSet(0, NE_White, -0.5, -0.5, -0.5);

    // This enables shading (you can choose normal or toon).
    NE_ShadingEnable(true);
    // This enables outlining in all polygons, so be careful
    NE_OutliningEnable(true);

    // We set the second outlining color to red.
    // This will be used by polygons with ID 8 - 15.
    NE_OutliningSetColor(1, NE_Red);

    while (1)
    {
        // Refresh keys
        scanKeys();
        uint32 keys = keysHeld();

        printf("\x1b[0;0H"
               "Pad: Rotate.\nA: Toon shading.\n"
               "B: Change alpha value.\nY: Wireframe mode (alpha = 0)\n"
               "X: Outlining.");

        // Rotate model using the pad
        if (keys & KEY_UP)
            NE_ModelRotate(Model, 0, 0, 2);
        if (keys & KEY_DOWN)
            NE_ModelRotate(Model, 0, 0, -2);
        if (keys & KEY_RIGHT)
            NE_ModelRotate(Model, 0, 2, 0);
        if (keys & KEY_LEFT)
            NE_ModelRotate(Model, 0, -2, 0);

        // Change shading type
        if (keys & KEY_A)
            shading = NE_TOON_HIGHLIGHT_SHADING;
        else
            shading = NE_MODULATION;

        if (keys & KEY_B)
            alpha = 15; // Transparent
        else if (keys & KEY_Y)
            alpha = 0;  // Wireframe
        else
            alpha = 31; // Opaque

        // Change polygon ID to change outlining color
        if (keys & KEY_X)
            id = 8;
        else
            id = 0;

        // Draw scene
        NE_Process(Draw3DScene);
        NE_WaitForVBL(0);
    }

    return 0;
}
