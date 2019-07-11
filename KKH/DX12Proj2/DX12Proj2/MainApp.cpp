#include "pch.h"
#include "MainApp.h"
#include "Mouse_Manager.h"
#include "Utility_Manager.h"
#include "Texture_Manger.h"
#include "Camera_Manager.h"

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

	return TRUE;
}

int MainApp::Update_MainApp(const CTimer& mt)
{
	return 0;
}

void MainApp::Render_MainApp(const CTimer& mt)
{
}

bool MainApp::LoadTexture(void)
{
	if (!TEX.onDDSLoad("grassTex", L"./Resource/Texture/grass.dds"))			return FALSE;
	if (!TEX.onDDSLoad("waterTex", L"./Resource/Texture/water1.dds"))			return FALSE;
	if (!TEX.onDDSLoad("fenceTex", L"./Resource/Texture/WireFence.dds"))		return FALSE;

	//Stnecil
	if (!TEX.onDDSLoad("bircksTex", L"./Resource/Texture/bricks3.dds"))			return FALSE;
	if (!TEX.onDDSLoad("checkboardTex", L"./Resource/Texture/checkboard.dds"))	return FALSE;
	if (!TEX.onDDSLoad("iceTex", L"./Resource/Texture/ice.dds"))				return FALSE;
	if (!TEX.onDDSLoad("white1x1Tex", L"./Resource/Texture/white1x1.dds"))		return FALSE;

	//Billboard
	if (!TEX.onDDSLoad("treeArrayTex", L"./Resource/Texture/treeArray2.dds"))	return FALSE;

	return TRUE;
}

bool MainApp::CreateObject(void)
{
	return TRUE;
}
