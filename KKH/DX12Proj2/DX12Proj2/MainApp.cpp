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

	CreateObject();
	if (!UTIL.Object_Ready()) return FALSE;


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
