#include "stdafx.h"
#include "MainApp.h"
#include "Graphic_Manager.h"
#include "Mouse_Manager.h"
#include "Function.h"
#include "Utility_Manager.h"
#include "Box.h"
#include "LitColumn.h"
#include "Grid.h"
#include "Wave.h"
#include "Texture_Manger.h"

#define KEY_DOWN(vk_code)	(GetAsyncKeyState(vk_code) & 0x0001)

CMainApp::CMainApp(void)
	: m_fTime(0.0f)
	//m_LAW(nullptr)
	//,m_Shape(nullptr)
	//,m_Box(nullptr)
{
}

CMainApp::~CMainApp(void)
{
	Free();
}

bool CMainApp::Ready_MainApp(void)
{
	if (!GRAPHIC_MGR.Init_Graphic())
		return FALSE;

	//Reset the command list to prep for initialization commands.
	ThrowIfFailed(COM_LIST->Reset(GRAPHIC->Get_CommandAllocator().Get(), nullptr));

	UTIL.Get_CbvSrvDescriptorSize() = GRAPHIC_DEV->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	LoadTexture();	

	UTIL.BuildRootSignature();
	UTIL.BuildDescriptorHeaps();
	UTIL.BuildShadersAndInputLayer();

	CreateObject();
	if (!UTIL.Object_Ready()) return FALSE;

	UTIL.BuildFrameResources();
	UTIL.BuildPSOs();

	UTIL.OnResize();

	//m_LAW = LandAndWave::Create();
	//m_LAW->OnResize();

	//MOUSE.Set_Obj(m_Box);
	//MOUSE.Set_Obj(m_Shape);
	//MOUSE.Set_Obj(m_LAW);

	//Execute the initialization commands.
	ThrowIfFailed(COM_LIST->Close());

	ID3D12CommandList* cmdLists[] = { COM_LIST.Get() };
	GRAPHIC->Get_CommandQueue()->ExecuteCommandLists(_countof(cmdLists), cmdLists);

	//Wait until initialization is complete.
	GRAPHIC->FlushCommandQueue();

	return TRUE;
}

int CMainApp::Update_MainApp(const CTimer& mt)
{
	if (!UTIL.Object_Cycle(mt, Utility_Manager::OS_UPDATE)) return 0;

	return 1;
}

void CMainApp::Render_MainApp(const CTimer& mt)
{
	if (!UTIL.Object_Cycle(mt, Utility_Manager::OS_RENDER)) return;
}

bool CMainApp::LoadTexture(void)
{
	if (!TEX.onDDSLoad("grassTex", L"../Texture/grass.dds"))			return FALSE;
	if (!TEX.onDDSLoad("waterTex", L"../Texture/water1.dds"))			return FALSE;
	if (!TEX.onDDSLoad("fenceTex", L"../Texture/WireFence.dds"))		return FALSE;

	//Stnecil
	if (!TEX.onDDSLoad("bircksTex", L"../Texture/bricks3.dds"))			return FALSE;
	if (!TEX.onDDSLoad("checkboardTex", L"../Texture/checkboard.dds"))	return FALSE;
	if (!TEX.onDDSLoad("iceTex", L"../Texture/ice.dds"))				return FALSE;
	if (!TEX.onDDSLoad("white1x1Tex", L"../Texture/white1x1.dds"))		return FALSE;

	return TRUE;
}

bool CMainApp::CreateObject(void)
{
	//if (!UTIL.Object_Create(Box::Create("box", Object::COM_TYPE::CT_STATIC, "BoxGeo", 1.5f, 0.5f, 1.5f, 3))) return FALSE;
	//if(!UTIL.Object_Create(LitColumn::Create("litcolumn", Object::COM_TYPE::CT_STATIC))) return FALSE;

	g_MatCBcount = 0;
	g_ObjCBcount = 0;

	if (!UTIL.Object_Create(Grid::Create(Object::COM_TYPE::CT_STATIC, "landGeo", "grid", "grassTex", "grass"))) return FALSE;
	if (!UTIL.Object_Create(Wave::Create(Object::COM_TYPE::CT_STATIC, "waterGeo", "grid", "waterTex", "water"))) return FALSE;
	if (!UTIL.Object_Create(Box::Create(Object::COM_TYPE::CT_STATIC, "boxGeo", "box", "fenceTex", "wirefence"))) return FALSE;

	return TRUE;
}


void CMainApp::Free(void)
{
}
