#pragma once

#include <DirectXMath.h>

#define MaxLights 16

extern HWND	 g_hWnd;

extern DirectX::XMFLOAT3 g_EyePos;

extern bool g_bDebugConsole;
extern bool mIsWireframe;

extern DirectX::XMFLOAT4X4 g_Proj;

extern short g_MatCBcount;
extern short g_ObjCBcount;

const WORD WINSIZE_X = 1400;
const WORD WINSIZE_Y = 1050;

const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

const float PI = 3.1415926535897932f;

const int NumFrameResources = 3;

enum class DrawLayer : int
{
	DL_OPAUQE = 0,
	DL_MIRROR,
	DL_REFLECTED,
	DL_TRANSPARENT,
	DL_ALAPHTESTED,
	DL_SHADOW,
	DL_END
};