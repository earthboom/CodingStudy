#include "pch.h"
#include "MainApp.h"
#include "Mouse_Manager.h"
#include "Utility_Manager.h"
#include "Texture_Manger.h"
#include "Camera_Manager.h"
#include "Skull.h"
#include "NormalObject.h"
#include "Sky.h"

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

	UTIL.Get_ShadowMap() = std::make_unique<ShadowMap>(DEVICE.Get(), 2048, 2048);

	LoadTexture();

	UTIL.OnResize();
	UTIL.BuildRootSignature();
	UTIL.BuildDescriptorHeaps();
	
	CreateObject();
	if (!UTIL.Object_Ready()) return FALSE;

	//UTIL.BuildPublicMaterials();

	//MOUSE.SetPieckedRitem();

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
	if (!TEX.onDDSLoad("bricksTex", L"./Resource/Texture/bricks.dds"))				return FALSE;
	if (!TEX.onDDSLoad("brickNormalTex", L"./Resource/Texture/bricks2_nmap.dds"))	return FALSE;
	if (!TEX.onDDSLoad("tileTex", L"./Resource/Texture/tile.dds"))					return FALSE;
	if (!TEX.onDDSLoad("tileNormalTex", L"./Resource/Texture/tile_nmap.dds"))		return FALSE;
	if (!TEX.onDDSLoad("defaultTex", L"./Resource/Texture/white1x1.dds"))			return FALSE;
	if (!TEX.onDDSLoad("defaultNormalTex", L"./Resource/Texture/default_nmap.dds"))	return FALSE;
	if (!TEX.onDDSLoad("skycubeTex", L"./Resource/Texture/desertcube1024.dds"))		return FALSE;		

	return TRUE;
}

bool MainApp::CreateObject(void)
{
	g_SrvHeapCount = 0;
	g_MatCBcount = 0;
	g_ObjCBcount = 0;

	if (!UTIL.Object_Create(std::dynamic_pointer_cast<Object>(Skull::Create(Object::COM_TYPE::CT_STATIC, "skullGeo", "skull", "defaultTex", "skullMat", "defaultNormalTex")))) return FALSE;

	if (!UTIL.Object_Create(std::dynamic_pointer_cast<Object>(NormalObject::Create(Object::COM_TYPE::CT_STATIC, "BoxGeo", "box", "bricksTex", "boxMat", "brickNormalTex", NormalObject::ShapeType::ST_BOX)))) return FALSE;
	//if (!UTIL.Object_Create(std::dynamic_pointer_cast<Object>(NormalObject::Create(Object::COM_TYPE::CT_STATIC, "ShpereGeo", "shperemirror", "defaultTex", "sphereMirrorMat", NormalObject::ShapeType::ST_SHPERE)))) return FALSE;
	//if (!UTIL.Object_Create(std::dynamic_pointer_cast<Object>(NormalObject::Create(Object::COM_TYPE::CT_STATIC, "GridGeo", "grid", "tileTex", "gridMat", NormalObject::ShapeType::ST_GRID)))) return FALSE;
	//if (!UTIL.Object_Create(std::dynamic_pointer_cast<Object>(NormalObject::Create(Object::COM_TYPE::CT_STATIC, "ShpereGeo", "shpere", "defaultTex", "shpereMat", NormalObject::ShapeType::ST_SHPERE)))) return FALSE;
	//if (!UTIL.Object_Create(std::dynamic_pointer_cast<Object>(NormalObject::Create(Object::COM_TYPE::CT_STATIC, "CylinderGeo", "cylinder", "bricksTex", "cylinderMat", NormalObject::ShapeType::ST_CYLINEDER)))) return FALSE;
	if (!UTIL.Object_Create(std::dynamic_pointer_cast<Object>(Sky::Create(Object::COM_TYPE::CT_STATIC, "ShpereGeo", "sky", "skycubeTex", "skyMat")))) return FALSE;

	InstanceData tempInstance;
	OBJECT Object = UTIL.Get_Object("skull", Object::COM_TYPE::CT_STATIC);
	XMStoreFloat4x4(&tempInstance.World, XMMatrixScaling(2.0f, 1.0f, 2.0f) * XMMatrixTranslation(0.0f, 5.0f, 0.0f));
	XMStoreFloat4x4(&tempInstance.TexTransform, XMMatrixScaling(1.0f, 0.5f, 1.0f));
	Object->Get_vectorInstanceData().push_back(std::move(tempInstance));

	Object = UTIL.Get_Object("sky", Object::COM_TYPE::CT_STATIC);
	XMStoreFloat4x4(&tempInstance.World, XMMatrixScaling(5000.0f, 5000.0f, 5000.0f));
	tempInstance.TexTransform = MathHelper::Identity4x4();
	Object->Get_vectorInstanceData().push_back(std::move(tempInstance));

	Object = UTIL.Get_Object("box", Object::COM_TYPE::CT_STATIC);
	XMStoreFloat4x4(&tempInstance.World, XMMatrixScaling(2.0f, 1.0f, 2.0f) * XMMatrixTranslation(0.0f, 5.0f, 0.0f));
	XMStoreFloat4x4(&tempInstance.TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	Object->Get_vectorInstanceData().push_back(std::move(tempInstance));

	/*tempInstance.Init();
	Object = UTIL.Get_Object("shperemirror", Object::COM_TYPE::CT_STATIC);
	XMStoreFloat4x4(&tempInstance.World, XMMatrixScaling(2.0f, 2.0f, 2.0f) * XMMatrixTranslation(0.0f, 2.0f, 0.0f));
	XMStoreFloat4x4(&tempInstance.TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	Object->Get_vectorInstanceData().push_back(std::move(tempInstance));

	tempInstance.Init();
	Object = UTIL.Get_Object("grid", Object::COM_TYPE::CT_STATIC);
	XMStoreFloat4x4(&tempInstance.TexTransform, XMMatrixScaling(8.0f, 8.0f, 1.0f));
	Object->Get_vectorInstanceData().push_back(std::move(tempInstance));

	XMMATRIX briTexTransform = XMMatrixScaling(1.5f, 2.0f, 1.0f);
	for (int i = 0; i < 5; ++i)
	{
		XMMATRIX leftCylWorld = XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i * 5.0f);
		XMMATRIX rightCylWorld = XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i * 5.0f);

		XMMATRIX leftSphereWorld = XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i * 5.0f);
		XMMATRIX rightSphereWorld = XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i * 5.0f);

		tempInstance.Init();
		Object = UTIL.Get_Object("cylinder", Object::COM_TYPE::CT_STATIC);
		XMStoreFloat4x4(&tempInstance.World, leftCylWorld);
		XMStoreFloat4x4(&tempInstance.TexTransform, briTexTransform);
		Object->Get_vectorInstanceData().push_back(std::move(tempInstance));

		tempInstance.Init();
		XMStoreFloat4x4(&tempInstance.World, rightCylWorld);
		XMStoreFloat4x4(&tempInstance.TexTransform, briTexTransform);
		Object->Get_vectorInstanceData().push_back(std::move(tempInstance));

		tempInstance.Init();
		Object = UTIL.Get_Object("shpere", Object::COM_TYPE::CT_STATIC);
		XMStoreFloat4x4(&tempInstance.World, leftSphereWorld);
		Object->Get_vectorInstanceData().push_back(std::move(tempInstance));

		XMStoreFloat4x4(&tempInstance.World, rightSphereWorld);
		Object->Get_vectorInstanceData().push_back(std::move(tempInstance));
	}*/

	return TRUE;
}
