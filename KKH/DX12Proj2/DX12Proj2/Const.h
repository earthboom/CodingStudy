#pragma once

#define MaxLights 16

extern HWND g_hWnd;

extern WORD WINSIZE_X;
extern WORD WINSIZE_Y;

extern XMFLOAT3 g_EyePos;
extern XMFLOAT4X4 g_Proj;

extern const float PI;
extern const float Infinity;

extern const int NumFrameResources;

extern short g_SrvHeapCount;
extern short g_MatCBcount;
extern short g_ObjCBcount;

extern UINT g_totalVertexOffset;
extern UINT g_totalIndexOffset;

extern bool g_ScreenBlur;
extern bool g_FrustumCullingEnabled;

enum class DrawLayer : int
{
	DL_OPAUQE = 0,	
	DL_DEUBG,
	DL_SKY,
	//DL_MIRROR,
	//DL_REFLECTED,
	//DL_TRANSPARENT,
	//DL_HIGHLIGHT,
	//DL_ALPHATESTED,
	//DL_ALPHATESTED_TREESPRITE,
	//DL_SHADOW,
	DL_END
};