#include "pch.h"
#include "MainApp.h"
#include "Mouse_Manager.h"
#include "Utility_Manager.h"
#include "Texture_Manger.h"
#include "Camera_Manager.h"
#include "Skull.h"

MainApp::MainApp(void)
	: m_fTime(0.0f)
{
}

MainApp::~MainApp(void)
{
}

bool MainApp::Ready_MainApp(void)
{
	if (!g_Graphics->Initialize()) return FALSE;

	ThrowIfFailed(COM_LIST->Reset(g_Graphics->Get_CommandAllocator().Get(), nullptr));

	UTIL.Get_CbvSrvDescriptorSize() = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//Camera
	CAM_MGR.CreateCamera();
	CURR_CAM->SetPosition(0.0f, 2.0f, -15.0f);

	LoadTexture();

	UTIL.OnResize();
	UTIL.BuildRootSignature();
	UTIL.BuildDescriptorHeaps();
	
	CreateObject();
	if (!UTIL.Object_Ready()) return FALSE;

	MOUSE.SetPieckedRitem();

	UTIL.BuildShadersAndInputLayer();
	UTIL.BuildFrameResources();
	UTIL.BuildPSOs();

	//Execute the initialization commands.
	ThrowIfFailed(COM_LIST->Close());

	ID3D12CommandList* cmdLists[] = { COM_LIST.Get() };
	g_Graphics->Get_CommandQueue()->ExecuteCommandLists(_countof(cmdLists), cmdLists);

	//Wait until initialization is complete.
	g_Graphics->FlushCommandQueue();

	return TRUE;
}

int MainApp::Update_MainApp(const CTimer& mt)
{
	float dt = mt.DeltaTime();

	if (GetAsyncKeyState('W') & 0x8000)
		CURR_CAM->Walk(25.0f * dt);

	if (GetAsyncKeyState('S') & 0x8000)
		CURR_CAM->Walk(-25.0f * dt);

	if (GetAsyncKeyState('A') & 0x8000)
		CURR_CAM->Strafe(-25.0f * dt);

	if (GetAsyncKeyState('D') & 0x8000)
		CURR_CAM->Strafe(25.0f * dt);

	if (GetAsyncKeyState('1') & 0x8000)
		g_FrustumCullingEnabled = TRUE;

	if (GetAsyncKeyState('2') & 0x8000)
		g_FrustumCullingEnabled = FALSE;

	CURR_CAM->UpdateViewMatrix();

	if (!UTIL.Object_Cycle(mt, Utility_Manager::OS_UPDATE)) return 0;

	return 1;
}

void MainApp::Render_MainApp(const CTimer& mt)
{
	if (!UTIL.Object_Cycle(mt, Utility_Manager::OS_RENDER)) return;
}

bool MainApp::LoadTexture(void)
{
	if (!TEX.onDDSLoad("bricksTex", L"./Resource/Texture/bricks.dds"))			return FALSE;
	if (!TEX.onDDSLoad("stoneTex", L"./Resource/Texture/stone.dds"))			return FALSE;
	if (!TEX.onDDSLoad("tileTex", L"./Resource/Texture/tile.dds"))				return FALSE;
	if (!TEX.onDDSLoad("crateTex", L"./Resource/Texture/WoodCrate01.dds"))		return FALSE;
	if (!TEX.onDDSLoad("iceTex", L"./Resource/Texture/ice.dds"))				return FALSE;
	if (!TEX.onDDSLoad("grassTex", L"./Resource/Texture/grass.dds"))			return FALSE;	
	if (!TEX.onDDSLoad("defaultTex", L"./Resource/Texture/white1x1.dds"))		return FALSE;

	return TRUE;
}

bool MainApp::CreateObject(void)
{
	g_MatCBcount = 0;
	g_ObjCBcount = 0;

	if (!UTIL.Object_Create(std::dynamic_pointer_cast<Object>(Skull::Create(Object::COM_TYPE::CT_STATIC, "skullGeo", "skull", "white1x1Tex", "whiteskull")))) return FALSE;

	return TRUE;
}
