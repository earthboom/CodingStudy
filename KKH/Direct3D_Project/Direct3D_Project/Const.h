#pragma once

#include <DirectXMath.h>

extern HWND	 g_hWnd;

extern DirectX::XMFLOAT3 g_EyePos;


extern bool mIsWireframe;

extern DirectX::XMFLOAT4X4 g_Proj;

const WORD WINSIZE_X = 1400;
const WORD WINSIZE_Y = 1050;

const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

const float PI = 3.1415926535897932f;

const int NumFrameResources = 3;



