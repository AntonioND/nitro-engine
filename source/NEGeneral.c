// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include "NEMain.h"

/*! \file   NEGeneral.c */

const char NE_VersioString[] =
    "Nitro Engine - Version " NITRO_ENGINE_VERSION_STRING " - "
    "(C) 2008-2011, 2019, 2022 Antonio Nino Diaz (AntonioND)";

static bool NE_UsingConsole;
bool NE_TestTouch;
static int NE_screenratio;
static u8 NE_viewport[4];
static u8 NE_Screen;
bool NE_Dual;

NE_Input ne_input;

static bool ne_inited = false;

static SpriteEntry *NE_Sprites;	//For Dual 3D mode

static int ne_znear, ne_zfar;

void NE_End(void)
{
	if (!ne_inited)
		return;

	// Hide BG0
	REG_DISPCNT &= ~(DISPLAY_BG0_ACTIVE | ENABLE_3D);

	vramSetBankA(VRAM_A_LCD);
	vramSetBankB(VRAM_B_LCD);
	if (NE_Dual) {
		vramSetBankC(VRAM_C_LCD);
		vramSetBankD(VRAM_D_LCD);

		free(NE_Sprites);
	} else if (GFX_CONTROL & GL_CLEAR_BMP) {
		NE_ClearBMPEnable(false);
	}

	vramSetBankE(VRAM_E_LCD);
	if (NE_UsingConsole) {
		vramSetBankF(VRAM_F_LCD);
		NE_UsingConsole = false;
	}

	NE_GUISystemEnd();
	NE_SpriteSystemEnd();
	NE_PhysicsSystemEnd();
	NE_ModelSystemEnd();
	NE_AnimationSystemEnd();
	NE_TextResetSystem();
	NE_TextureSystemEnd();
	NE_CameraSystemEnd();
	NE_SpecialEffectSet(0);

	//Power off 3D hardware
	powerOff(POWER_3D_CORE | POWER_MATRIX);

	NE_DebugPrint("Nitro Engine disabled");

	ne_inited = false;
}

void NE_Viewport(int x1, int y1, int x2, int y2)
{
	glViewport(x1, y1, x2, y2);

	NE_viewport[0] = x1;
	NE_viewport[1] = y1;
	NE_viewport[2] = x2;
	NE_viewport[3] = y2;

	NE_screenratio = divf32((x2 - x1 + 1) << 12, (y2 - y1 + 1) << 12);

	MATRIX_CONTROL = GL_PROJECTION;	//New projection matix for this viewport
	MATRIX_IDENTITY = 0;
	gluPerspectivef32(70 * DEGREES_IN_CIRCLE / 360, NE_screenratio,
			  ne_znear, ne_zfar);

	MATRIX_CONTROL = GL_MODELVIEW;
}

static void NE_Init__(void)
{
	// Power all 3D and 2D. Hide 3D screen during init
	powerOn(POWER_ALL);

	videoSetMode(0);

	vramSetBankE(VRAM_E_TEX_PALETTE);

	// While 3D hardware is busy
	while (GFX_BUSY) ;

	// Clear the FIFO
	GFX_STATUS |= (1 << 29);

	glFlush(0);
	glFlush(0);

	lcdMainOnTop();

	glResetMatrixStack();

	GFX_CONTROL = GL_TEXTURE_2D | GL_ANTIALIAS | GL_BLEND;

	glAlphaFunc(BLEND_ALPHA);

	NE_ClearColorSet(NE_Black, 31, 63);
	NE_FogEnableBackground(false);
	glClearDepth(GL_MAX_DEPTH);

	// Default number of objects for everyting - Textures are inited in
	// NE_Init3D and NE_InitDual3D
	NE_CameraSystemReset(0);
	NE_PhysicsSystemReset(0);
	NE_SpriteSystemReset(0);
	NE_GUISystemReset(0);
	NE_ModelSystemReset(0);
	NE_AnimationSystemReset(0);
	NE_TextPriorityReset();

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// The DS uses a table for shinyness..this generates a half-ass one
	glMaterialShinyness();

	// setup default material properties
	NE_MaterialSetDefaultPropierties(RGB15(20, 20, 20), RGB15(16, 16, 16),
					 RGB15(8, 8, 8), RGB15(5, 5, 5),
					 false, true);

	// Turn off some things...
	for (int i = 0; i < 4; i++)
		NE_LightOff(i);

	GFX_COLOR = 0;
	GFX_POLY_FORMAT = 0;

	for (int i = 0; i < 8; i++)
		NE_OutliningSetColor(i, 0);

	ne_znear = floattof32(0.1);
	ne_zfar = floattof32(40.0);
	NE_Viewport(0, 0, 255, 191);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Ready!!

	videoSetMode(MODE_0_3D);
}

void NE_SwapScreens(void)
{
	REG_POWERCNT ^= POWER_SWAP_LCDS;
}

void NE_UpdateInput(void)
{
	ne_input.kdown = keysDown();
	ne_input.kheld = keysHeld();
	ne_input.kup = keysUp();

	if (ne_input.kheld & KEY_TOUCH) {
		touchRead(&ne_input.touch);
	}
}

void NE_Init3D(void)
{
	if (ne_inited)
		NE_End();

	NE_Init__();

	NE_TextureSystemReset(0, 0, NE_VRAM_ABCD);

	NE_UpdateInput();

	ne_inited = true;
	NE_Dual = false;

	NE_DebugPrint("Nitro Engine initialized in normal 3D mode");
}

void NE_InitDual3D(void)
{
	if (ne_inited)
		NE_End();

	NE_Init__();

	NE_TextureSystemReset(0, 0, NE_VRAM_AB);

	NE_UpdateInput();

	videoSetModeSub(0);

	REG_BG2CNT_SUB = BG_BMP16_256x256;
	REG_BG2PA_SUB = 1 << 8;
	REG_BG2PB_SUB = 0;
	REG_BG2PC_SUB = 0;
	REG_BG2PD_SUB = 1 << 8;
	REG_BG2X_SUB = 0;
	REG_BG2Y_SUB = 0;

	vramSetBankC(VRAM_C_SUB_BG);
	vramSetBankD(VRAM_D_SUB_SPRITE);

	NE_Sprites = calloc(128, sizeof(SpriteEntry));

	int y = 0, x = 0;

	// Reset sprites
	for (int i = 0; i < 128; i++)
		NE_Sprites[i].attribute[0] = ATTR0_DISABLED;

	int i = 0;
	for (y = 0; y < 3; y++) {
		for (x = 0; x < 4; x++) {
			NE_Sprites[i].attribute[0] = ATTR0_BMP | ATTR0_SQUARE
						   | (64 * y);
			NE_Sprites[i].attribute[1] = ATTR1_SIZE_64 | (64 * x);
			NE_Sprites[i].attribute[2] = ATTR2_ALPHA(1)
						   | (8 * 32 * y) | (8 * x);
			i++;
		}
	}

	videoSetModeSub(MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_SPR_ACTIVE |
			DISPLAY_SPR_2D_BMP_256);

	ne_inited = true;
	NE_Dual = true;

	NE_DebugPrint("Nitro Engine initialized in dual 3D mode");
}

void NE_InitConsole(void)
{
	if (!ne_inited)
		return;

	NE_UsingConsole = true;

	videoSetMode(MODE_0_3D | DISPLAY_BG1_ACTIVE);

	vramSetBankF(VRAM_F_MAIN_BG);

	BG_PALETTE[255] = 0xFFFF;

	// Use BG 1 for text, set to highest priority
	REG_BG1CNT = BG_MAP_BASE(4) | BG_PRIORITY(0);

	// Set BG 0 (3D background) to be a lower priority than BG 1
	REG_BG0CNT = BG_PRIORITY(1);

	consoleInit(0, 1, BgType_Text4bpp, BgSize_T_256x256, 4, 0, true, true);
}

void NE_SetConsoleColor(u32 color)
{
	BG_PALETTE[255] = color;
}

void NE_Process(NE_Voidfunc drawscene)
{
	NE_UpdateInput();

	glViewport(NE_viewport[0], NE_viewport[1], NE_viewport[2],
		   NE_viewport[3]);

	NE_PolyFormat(31, 0, NE_LIGHT_ALL, NE_CULL_BACK, 0);

	MATRIX_CONTROL = GL_PROJECTION;
	MATRIX_IDENTITY = 0;
	gluPerspectivef32(70 * DEGREES_IN_CIRCLE / 360, NE_screenratio,
			  ne_znear, ne_zfar);

	MATRIX_CONTROL = GL_MODELVIEW;
	MATRIX_IDENTITY = 0;

	NE_AssertPointer(drawscene, "NULL function pointer");
	drawscene();

	GFX_FLUSH = GL_TRANS_MANUALSORT;
}

void NE_ProcessDual(NE_Voidfunc topscreen, NE_Voidfunc downscreen)
{
	NE_UpdateInput();

	REG_POWERCNT ^= POWER_SWAP_LCDS;
	NE_Screen ^= 1;

	if (NE_Screen == 1) {
		if (NE_UsingConsole) {
			REG_BG1CNT = BG_MAP_BASE(4) | BG_PRIORITY(0);
			REG_BG0CNT = BG_PRIORITY(1);
		}

		vramSetBankC(VRAM_C_SUB_BG);
		vramSetBankD(VRAM_D_LCD);

		REG_DISPCAPCNT = DCAP_SIZE(DCAP_SIZE_256x192)
			       | DCAP_BANK(DCAP_BANK_VRAM_D)
			       | DCAP_MODE(DCAP_MODE_A)
			       | DCAP_SRC_A(DCAP_SRC_A_COMPOSITED)
			       | DCAP_ENABLE;

	} else {
		if (NE_UsingConsole) {
			REG_BG1CNT = BG_PRIORITY(1);
			REG_BG0CNT = BG_PRIORITY(0);
		}

		vramSetBankC(VRAM_C_LCD);
		vramSetBankD(VRAM_D_SUB_SPRITE);

		REG_DISPCAPCNT = DCAP_SIZE(DCAP_SIZE_256x192)
			       | DCAP_BANK(DCAP_BANK_VRAM_C)
			       | DCAP_MODE(DCAP_MODE_A)
			       | DCAP_SRC_A(DCAP_SRC_A_COMPOSITED)
			       | DCAP_ENABLE;
	}

	NE_PolyFormat(31, 0, NE_LIGHT_ALL, NE_CULL_BACK, 0);

	glViewport(0, 0, 255, 191);

	NE_viewport[0] = 0;
	NE_viewport[1] = 0;
	NE_viewport[2] = 255;
	NE_viewport[3] = 191;

	MATRIX_CONTROL = GL_PROJECTION;
	MATRIX_IDENTITY = 0;
	gluPerspectivef32(70 * DEGREES_IN_CIRCLE / 360,
			  floattof32(256.0 / 192.0), ne_znear, ne_zfar);

	MATRIX_CONTROL = GL_MODELVIEW;
	MATRIX_IDENTITY = 0;

	NE_AssertPointer(topscreen, "NULL function pointer (top screen)");
	NE_AssertPointer(downscreen, "NULL function pointer (lower screen)");

	if (NE_Screen == 1) {
		topscreen();
	} else {
		downscreen();
	}

	GFX_FLUSH = GL_TRANS_MANUALSORT;

	dmaCopy(NE_Sprites, OAM_SUB, 128 * sizeof(SpriteEntry));
}

void NE_ClippingPlanesSetI(int znear, int zfar)
{
	NE_Assert(znear < zfar, "znear must be smaller than zfar");
	ne_znear = znear;
	ne_zfar = zfar;
}

void NE_AntialiasEnable(bool value)
{
	if (value)
		GFX_CONTROL |= GL_ANTIALIAS;
	else
		GFX_CONTROL &= ~GL_ANTIALIAS;
}

int NE_GetPolygonCount(void)
{
	while (GFX_BUSY) ;

	return GFX_POLYGON_RAM_USAGE;
}

int NE_GetVertexCount(void)
{
	while (GFX_BUSY) ;

	return GFX_VERTEX_RAM_USAGE;
}

static int NE_Effect = 0;
static int NE_lastvbladd = 0;
static bool NE_effectpause;
static int *ne_noisepause;
static int ne_cpucount;
static int ne_noise_value = 0xF;
static int ne_sine_mult = 10, ne_sine_shift = 9;

void NE_VBLFunc(void)
{
	if (!ne_inited)
		return;

	if (NE_Effect == 1 || NE_Effect == 2) {
		if (!NE_effectpause)
			NE_lastvbladd = (NE_lastvbladd + 1) & LUT_MASK;
	}
}

void NE_SpecialEffectPause(bool pause)
{
	if (NE_Effect == 0)
		return;

	NE_effectpause = pause;
	if (pause) {
		ne_noisepause = malloc(sizeof(int) * 512);

		for (int i = 0; i < 512; i++)
			ne_noisepause[i] = (rand() & ne_noise_value)
					 - (ne_noise_value >> 1);
	} else {
		if (ne_noisepause != NULL) {
			free(ne_noisepause);
			ne_noisepause = NULL;
		}
	}
}

void NE_HBLFunc(void)
{
	s16 angle;
	int val;

	if (!ne_inited)
		return;

	// This counter is used to estimate CPU usage
	ne_cpucount++;

	// Fix a problem with the first line when using effects
	int vcount = REG_VCOUNT;
	if (vcount == 262)
		vcount = 0;

	switch (NE_Effect) {
	case NE_NOISE:
		if (NE_effectpause && ne_noisepause)
			val = ne_noisepause[vcount & LUT_MASK];
		else
			val = (rand() & ne_noise_value) - (ne_noise_value >> 1);
		REG_BG0HOFS = val;
		break;

	case NE_SINE:
		angle = ((vcount + NE_lastvbladd) * ne_sine_mult) & LUT_MASK;
		REG_BG0HOFS = sinLerp(angle << 6) >> ne_sine_shift;
		break;

	default:
		break;
	}
}

void NE_SpecialEffectNoiseConfig(int value)
{
	ne_noise_value = value;
}

void NE_SpecialEffectSineConfig(int mult, int shift)
{
	ne_sine_mult = mult;
	ne_sine_shift = shift;
}

void NE_SpecialEffectSet(NE_SpecialEffects effect)
{
	switch (effect) {
	case 1:
		NE_Effect = 1;
		break;
	case 2:
		NE_Effect = 2;
		break;
	case 0:
	default:
		NE_Effect = 0;
		REG_BG0HOFS = 0;
		break;
	}
}

static int NE_CPUPercent;

void NE_WaitForVBL(NE_UpdateFlags flags)
{
	if (flags & NE_UPDATE_GUI)
		NE_GUIUpdate();
	if (flags & NE_UPDATE_ANIMATIONS)
		NE_ModelAnimateAll();
	if (flags & NE_UPDATE_PHYSICS)
		NE_PhysicsUpdateAll();

	NE_CPUPercent = div32(ne_cpucount * 100, 263);
	if (flags & NE_CAN_SKIP_VBL) {
		if (NE_CPUPercent > 100) {
			ne_cpucount = 0;
			return;

			//REG_DISPSTAT & DISP_IN_VBLANK
		}
	}

	swiWaitForVBlank();
	ne_cpucount = 0;
}

int NE_GetCPUPercent(void)
{
	return NE_CPUPercent;
}

bool NE_GPUIsRendering(void)
{
	if (REG_VCOUNT > 190 && REG_VCOUNT < 214)
		return false;

	return true;
}

// -------------------------------------------------------------------------
//                               DEBUG
#ifdef NE_DEBUG
static void (*ne_userdebugfn)(const char *) = NULL;

void __ne_debugoutputtoconsole(const char *text)
{
	printf(text);
}

void __NE_debugprint(const char *text)
{
	if (!ne_inited)
		return;
	if (ne_userdebugfn)
		ne_userdebugfn(text);
}

void NE_DebugSetHandler(void (*fn)(const char *))
{
	ne_userdebugfn = fn;
}

void NE_DebugSetHandlerConsole(void)
{
	NE_InitConsole();
	ne_userdebugfn = __ne_debugoutputtoconsole;
}
#endif
//-------------------------------------------------------------------------

#include <nds/arm9/postest.h>

static int ne_vertexcount;

void NE_TouchTestStart(void)
{
	//Hide what we are going to draw
	glViewport(255, 255, 255, 255);

	//Save current state
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	// Setup temporary render environment
	glLoadIdentity();

	int temp[4] = {
		NE_viewport[0], NE_viewport[1], NE_viewport[2], NE_viewport[3]
	};

	// Render only what is below the cursor
	gluPickMatrix(ne_input.touch.px, 191 - ne_input.touch.py, 3, 3, temp);
	gluPerspectivef32(70 * DEGREES_IN_CIRCLE / 360, NE_screenratio,
			  ne_znear, ne_zfar);

	glMatrixMode(GL_MODELVIEW);

	NE_Assert(!NE_TestTouch, "Test already active");

	NE_TestTouch = true;
}

void NE_TouchTestObject(void)
{
	NE_Assert(NE_TestTouch, "No active test");

	// Wait for the position test to finish
	while (PosTestBusy()) ;
	// Wait for all the polygons from the last object to be drawn
	while (GFX_BUSY) ;

	// Save the vertex ram count
	ne_vertexcount = NE_GetVertexCount();
}

int NE_TouchTestResult(void)
{
	NE_Assert(NE_TestTouch, "No active test");

	// Wait for all the polygons to get drawn
	while (GFX_BUSY) ;
	// Wait for the position test to finish
	while (PosTestBusy()) ;

	// If a polygon was drawn
	if (NE_GetVertexCount() > ne_vertexcount) {
		return PosTestWresult();
	}

	return -1;
}

void NE_TouchTestEnd(void)
{
	NE_Assert(NE_TestTouch, "No active test");

	NE_TestTouch = false;

	// Reset the viewport
	glViewport(NE_viewport[0], NE_viewport[1],
		   NE_viewport[2], NE_viewport[3]);

	// Restore previous state
	glMatrixMode(GL_PROJECTION);
	glPopMatrix(1);
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix(1);
}
