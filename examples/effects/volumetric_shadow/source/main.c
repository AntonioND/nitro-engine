// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "teapot_bin.h"
#include "teapot.h"

typedef struct {
    NE_Camera *Camera;
    NE_Model *Teapot;
    NE_Material *Material;

    bool draw_edges;
} SceneData;

void DrawFloor(void)
{
    NE_PolyNormal(0, -0.97, 0);

    NE_PolyBegin(GL_QUAD);

        NE_PolyTexCoord(0, 0);
        NE_PolyVertex(-10, 0, -10);

        NE_PolyTexCoord(0, 256);
        NE_PolyVertex(-10, 0, 10);

        NE_PolyTexCoord(256, 256);
        NE_PolyVertex(10, 0, 10);

        NE_PolyTexCoord(256, 0);
        NE_PolyVertex(10, 0, -10);

    NE_PolyEnd();
}

void DrawLid(void)
{
    NE_PolyNormal(0, -0.97, 0);

    NE_PolyBegin(GL_QUAD);

        NE_PolyTexCoord(0, 0);
        NE_PolyVertex(-0.75, 3, -0.75);

        NE_PolyTexCoord(0, 256);
        NE_PolyVertex(-0.75, 3,  0.75);

        NE_PolyTexCoord(256, 256);
        NE_PolyVertex( 0.75, 3,  0.75);

        NE_PolyTexCoord(256, 0);
        NE_PolyVertex( 0.75, 3, -0.75);

    NE_PolyEnd();
}

void DrawShadowVolume(void)
{
    // Lid

    NE_PolyBegin(GL_QUAD);

        NE_PolyVertex(-0.75, 3, -0.75);
        NE_PolyVertex(-0.75, 3,  0.75);
        NE_PolyVertex( 0.75, 3,  0.75);
        NE_PolyVertex( 0.75, 3, -0.75);

    NE_PolyEnd();

    // Walls

    NE_PolyBegin(GL_QUAD_STRIP);

        NE_PolyVertex(-0.75, 3, -0.75);
        NE_PolyVertex(-0.75, 0, -0.75);
        NE_PolyVertex(-0.75, 3,  0.75);
        NE_PolyVertex(-0.75, 0,  0.75);

        NE_PolyVertex( 0.75, 3,  0.75);
        NE_PolyVertex( 0.75, 0,  0.75);

        NE_PolyVertex( 0.75, 3, -0.75);
        NE_PolyVertex( 0.75, 0, -0.75);

        NE_PolyVertex(-0.75, 3, -0.75);
        NE_PolyVertex(-0.75, 0, -0.75);

    NE_PolyEnd();

    // Bottom

    NE_PolyBegin(GL_QUAD);

        NE_PolyVertex(-0.75, 0, -0.75);
        NE_PolyVertex(-0.75, 0,  0.75);
        NE_PolyVertex( 0.75, 0,  0.75);
        NE_PolyVertex( 0.75, 0, -0.75);

    NE_PolyEnd();
}

void Draw3DSceneBright(void *arg)
{
    NE_LightSet(0, NE_White, 0, -0.97, -0.0);

    SceneData *Scene = arg;

    // Set camera
    NE_CameraUse(Scene->Camera);

    // Set polygon format for regular models
    NE_PolyFormat(31, 0, NE_LIGHT_0, NE_CULL_BACK, NE_MODULATION);

    // Draw regular models
    NE_ModelDraw(Scene->Teapot);

    NE_MaterialUse(Scene->Material);
    DrawFloor();
    DrawLid();

    // Draw shadow volume as a black volume (shadow)
    NE_MaterialUse(NULL);
    NE_PolyColor(NE_Black);

    if (Scene->draw_edges)
    {
        // Draw the shadow volume in wireframe mode to see where it is
        NE_PolyFormat(0, 0, 0, NE_CULL_NONE, NE_MODULATION);
        DrawShadowVolume();
    }

    NE_PolyFormat(1, 0, 0, NE_CULL_NONE, NE_SHADOW_POLYGONS);
    DrawShadowVolume();

    NE_PolyFormat(20, 63, 0, NE_CULL_NONE, NE_SHADOW_POLYGONS);
    DrawShadowVolume();
}

void Draw3DSceneDark(void *arg)
{
    NE_LightSet(0, RGB15(8, 8, 8), 0, -0.97, -0.0);

    SceneData *Scene = arg;

    // Set camera
    NE_CameraUse(Scene->Camera);

    // Set polygon format for regular models
    NE_PolyFormat(31, 0, NE_LIGHT_0, NE_CULL_BACK, NE_MODULATION);

    // Draw regular models
    NE_ModelDraw(Scene->Teapot);

    NE_MaterialUse(Scene->Material);
    DrawFloor();
    DrawLid();

    // Draw shadow volume as a yellow volume (light)
    NE_MaterialUse(NULL);
    NE_PolyColor(RGB15(15, 15, 0));

    if (Scene->draw_edges)
    {
        // Draw the shadow volume in wireframe mode to see where it is
        NE_PolyFormat(0, 0, 0, NE_CULL_NONE, NE_MODULATION);
        DrawShadowVolume();
    }

    NE_PolyFormat(1, 0, 0, NE_CULL_NONE, NE_SHADOW_POLYGONS);
    DrawShadowVolume();

    NE_PolyFormat(20, 63, 0, NE_CULL_NONE, NE_SHADOW_POLYGONS);
    DrawShadowVolume();
}

int main(int argc, char *argv[])
{
    SceneData Scene = { 0 };

    // This is needed for special screen effects
    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_VBLANK, NE_HBLFunc);

    // Init console and Nitro Engine
    NE_InitDual3D();
    NE_InitConsole();

    // Setup camera
    Scene.Camera = NE_CameraCreate();
    NE_CameraSet(Scene.Camera,
                 0, 3.25, -3.25,
                 0, 1.25, 0,
                 0, 1, 0);

    // Load teapot and texture
    {
        Scene.Teapot = NE_ModelCreate(NE_Static);
        Scene.Material = NE_MaterialCreate();

        // Load mesh from RAM and assign it to a model
        NE_ModelLoadStaticMesh(Scene.Teapot, teapot_bin);

        // Load teapot texture from RAM and assign it to a material
        NE_MaterialTexLoad(Scene.Material, NE_RGB5, 256, 256,
                           NE_TEXGEN_TEXCOORD | NE_TEXTURE_WRAP_S | NE_TEXTURE_WRAP_T,
                           teapotBitmap);

        // Assign material to the model
        NE_ModelSetMaterial(Scene.Teapot, Scene.Material);

        // Set some properties to the material
        NE_MaterialSetProperties(Scene.Material,
                        RGB15(24, 24, 24), // Diffuse
                        RGB15(8, 8, 8),    // Ambient
                        RGB15(0, 0, 0),    // Specular
                        RGB15(0, 0, 0),    // Emission
                        false, false);     // Vertex color, use shininess table

        // Set initial position of the object
        NE_ModelSetCoordI(Scene.Teapot,
                          floattof32(0), floattof32(1.5), floattof32(0));
    }

    printf("\x1b[0;0H"
           "ABXY:    Rotate\n"
           "Pad:     Move\n"
           "SELECT:  Show edges of shadow\n"
           "START:   Exit to loader\n");

    while (1)
    {
        NE_WaitForVBL(0);

        // Refresh keys
        scanKeys();
        uint32_t keys = keysHeld();

        // Move model using the pad
        if (keys & KEY_UP)
            NE_ModelTranslate(Scene.Teapot, 0, 0, 0.05);
        if (keys & KEY_DOWN)
            NE_ModelTranslate(Scene.Teapot, 0, 0, -0.05);
        if (keys & KEY_RIGHT)
            NE_ModelTranslate(Scene.Teapot, -0.05, 0, 0);
        if (keys & KEY_LEFT)
            NE_ModelTranslate(Scene.Teapot, 0.05, 0, 0);

        // Rotate model using the pad
        if (keys & KEY_Y)
            NE_ModelRotate(Scene.Teapot, 0, 0, 2);
        if (keys & KEY_B)
            NE_ModelRotate(Scene.Teapot, 0, 0, -2);
        if (keys & KEY_X)
            NE_ModelRotate(Scene.Teapot, 0, 2, 0);
        if (keys & KEY_A)
            NE_ModelRotate(Scene.Teapot, 0, -2, 0);

        if (keys & KEY_SELECT)
            Scene.draw_edges = true;
        else
            Scene.draw_edges = false;

        if (keys & KEY_START)
            break;

        // Draw Scene
        NE_ProcessDualArg(Draw3DSceneBright, Draw3DSceneDark, &Scene, &Scene);
    }

    return 0;
}
