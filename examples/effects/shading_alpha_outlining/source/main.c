// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2024
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "teapot_bin.h"
#include "teapot.h"

typedef struct {
    NE_Camera *Camera;
    NE_Model *Model;

    int shading, alpha, id;
} SceneData;

void Draw3DScene(void *arg)
{
    SceneData *Scene = arg;

    // Set camera
    NE_CameraUse(Scene->Camera);

    // Set polygon format
    NE_PolyFormat(Scene->alpha, Scene->id, NE_LIGHT_0, NE_CULL_BACK,
                  Scene->shading);

    // Draw model
    NE_ModelDraw(Scene->Model);
}

int main(int argc, char *argv[])
{
    SceneData Scene = { 0 };

    // This is needed for special screen effects
    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_VBLANK, NE_HBLFunc);

    // Init console and Nitro Engine
    NE_Init3D();
    // Use banks A and B for teapots. libnds uses bank C for the demo text
    // console.
    NE_TextureSystemReset(0, 0, NE_VRAM_AB);
    // This is needed to print text
    consoleDemoInit();

    // Allocate the objects we will use
    Scene.Model = NE_ModelCreate(NE_Static);
    Scene.Camera = NE_CameraCreate();
    NE_Material *Material = NE_MaterialCreate();

    // Set camera coordinates
    NE_CameraSet(Scene.Camera,
                 0, 0, -3,
                 0, 0, 0,
                 0, 1, 0);

    // Load mesh from RAM and assign it to a model
    NE_ModelLoadStaticMesh(Scene.Model, teapot_bin);
    // Load teapot texture from RAM and assign it to a material
    NE_MaterialTexLoad(Material, NE_RGB5, 256, 256,
                       NE_TEXGEN_TEXCOORD | NE_TEXTURE_WRAP_S | NE_TEXTURE_WRAP_T,
                       teapotBitmap);

    // Assign material to the model
    NE_ModelSetMaterial(Scene.Model, Material);

    // Set some properties to the material
    NE_MaterialSetProperties(Material,
                  RGB15(24, 24, 24), // Diffuse
                  RGB15(8, 8, 8),    // Ambient
                  RGB15(0, 0, 0),    // Specular
                  RGB15(0, 0, 0),    // Emission
                  false, false);     // Vertex color, use shininess table

    // Setup a light and its color
    NE_LightSet(0, NE_White, -0.5, -0.5, -0.5);

    // This enables shading (you can choose normal or toon).
    NE_SetupToonShadingTables(true);
    // This enables outlining in all polygons, so be careful
    NE_OutliningEnable(true);

    // We set the second outlining color to red.
    // This will be used by polygons with ID 8 - 15.
    NE_OutliningSetColor(1, NE_Red);

    while (1)
    {
        NE_WaitForVBL(0);

        // Refresh keys
        scanKeys();
        uint32_t keys = keysHeld();

        printf("\x1b[0;0H"
               "Pad: Rotate.\nA: Toon shading.\n"
               "B: Change alpha value.\nY: Wireframe mode (alpha = 0)\n"
               "X: Outlining.");

        // Rotate model using the pad
        if (keys & KEY_UP)
            NE_ModelRotate(Scene.Model, 0, 0, 2);
        if (keys & KEY_DOWN)
            NE_ModelRotate(Scene.Model, 0, 0, -2);
        if (keys & KEY_RIGHT)
            NE_ModelRotate(Scene.Model, 0, 2, 0);
        if (keys & KEY_LEFT)
            NE_ModelRotate(Scene.Model, 0, -2, 0);

        // Change shading type
        if (keys & KEY_A)
            Scene.shading = NE_TOON_HIGHLIGHT_SHADING;
        else
            Scene.shading = NE_MODULATION;

        if (keys & KEY_B)
            Scene.alpha = 15; // Transparent
        else if (keys & KEY_Y)
            Scene.alpha = 0;  // Wireframe
        else
            Scene.alpha = 31; // Opaque

        // Change polygon ID to change outlining color
        if (keys & KEY_X)
            Scene.id = 8;
        else
            Scene.id = 0;

        // Draw scene
        NE_ProcessArg(Draw3DScene, &Scene);
    }

    return 0;
}
