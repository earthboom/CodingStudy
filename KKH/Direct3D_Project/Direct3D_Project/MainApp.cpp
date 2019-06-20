#include "stdafx.h"
#include "MainApp.h"
#include "Graphic_Manager.h"
#include "Mouse_Manager.h"
#include "Utility_Manager.h"
#include "Box.h"
#include "LitColumn.h"
#include "Grid.h"
#include "Wave.h"
#include "Surface.h"
#include "Mirror.h"
#include "Skull.h"
#include "Billboard.h"
#include "Texture_Manger.h"

#define KEY_DOWN(vk_code)	(GetAsyncKeyState(vk_code) & 0x0001)

CMainApp::CMainApp(void)
	: m_fTime(0.0f)
	//m_LAW(nullptr)
	//,m_Shape(nullptr)
	//,m_Box(nullptr)
{
	g_ScreenBlur = TRUE;
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
	
	UTIL.OnResize();

	UTIL.UtilityInitialize();
	UTIL.BuildRootSignature();
	UTIL.BuildPostProcessRootSignature();
	UTIL.BuildDescriptorHeaps();
	UTIL.BuildShadersAndInputLayer();

	CreateObject();
	if (!UTIL.Object_Ready()) return FALSE;

	UTIL.UtilityDecriptor();
	UTIL.BuildFrameResources();
	UTIL.BuildPSOs();
	
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

	//Billboard
	if (!TEX.onDDSLoad("treeArrayTex", L"../Texture/treeArray2.dds"))	return FALSE;

	return TRUE;
}

bool CMainApp::CreateObject(void)
{
	//if (!UTIL.Object_Create(Box::Create("box", Object::COM_TYPE::CT_STATIC, "BoxGeo", 1.5f, 0.5f, 1.5f, 3))) return FALSE;
	//if(!UTIL.Object_Create(LitColumn::Create("litcolumn", Object::COM_TYPE::CT_STATIC))) return FALSE;

	g_MatCBcount = 0;
	g_ObjCBcount = 0;

	if (!UTIL.Object_Create(std::dynamic_pointer_cast<Object>(Grid::Create(Object::COM_TYPE::CT_STATIC, "landGeo", "grid", "grassTex", "grass")))) return FALSE;
	if (!UTIL.Object_Create(std::dynamic_pointer_cast<Object>(Wave::Create(Object::COM_TYPE::CT_STATIC, "waterGeo", "grid", "waterTex", "water")))) return FALSE;
	if (!UTIL.Object_Create(std::dynamic_pointer_cast<Object>(Box::Create(Object::COM_TYPE::CT_STATIC, "boxGeo", "box", "fenceTex", "wirefence")))) return FALSE;

	//Billboard
	if (!UTIL.Object_Create(std::dynamic_pointer_cast<Object>(Billboard::Create(Object::COM_TYPE::CT_STATIC, "TreeGeo", "Tree", "treeArrayTex", "treearray")))) return FALSE;

	//if (!UTIL.Object_Create(std::dynamic_pointer_cast<Object>(Surface::Create(Object::COM_TYPE::CT_STATIC, "floorGeo", "floor", "checkboardTex", "checkertile")))) return FALSE;
	//if (!UTIL.Object_Create(std::dynamic_pointer_cast<Object>(Surface::Create(Object::COM_TYPE::CT_STATIC, "wall_1_Geo", "wall", "bircksTex", "bricks1")))) return FALSE;
	//if (!UTIL.Object_Create(std::dynamic_pointer_cast<Object>(Surface::Create(Object::COM_TYPE::CT_STATIC, "wall_2_Geo", "wall", "bircksTex", "bricks2")))) return FALSE;
	//if (!UTIL.Object_Create(std::dynamic_pointer_cast<Object>(Surface::Create(Object::COM_TYPE::CT_STATIC, "wall_3_Geo", "wall", "bircksTex", "bricks3")))) return FALSE;
	//if (!UTIL.Object_Create(std::dynamic_pointer_cast<Object>(Mirror::Create(Object::COM_TYPE::CT_STATIC, "mirrorGeo", "mirror", "iceTex", "icemirror")))) return FALSE;
	//if (!UTIL.Object_Create(std::dynamic_pointer_cast<Object>(Skull::Create(Object::COM_TYPE::CT_STATIC, "skullGeo", "skull", "white1x1Tex", "whiteskull")))) return FALSE;
	//SettingRoom();

	return TRUE;
}

void CMainApp::SettingRoom(void)
{
	std::array<VERTEX, 4> _vertex;
	//floor
	auto Objtemp = std::dynamic_pointer_cast<Surface>(UTIL.Get_Object("floorGeo", Object::COM_TYPE::CT_STATIC));
	_vertex =
	{
		VERTEX(-3.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 4.0f),
		VERTEX(-3.5f, 0.0f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f),
		VERTEX(7.5f,  0.0f,  0.0f,  0.0f, 1.0f, 0.0f, 4.0f, 0.0f),
		VERTEX(7.5f,  0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 4.0f, 4.0f)
	};
	Objtemp->Set_Vertex(_vertex);
	Objtemp->Get_Material().DiffuseAlbedo = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Objtemp->Get_Material().FresnelR0 = DirectX::XMFLOAT3(0.07f, 0.07f, 0.07f);
	Objtemp->Get_Material().Roughness = 0.3f;

	//Wall_1
	Objtemp = std::dynamic_pointer_cast<Surface>(UTIL.Get_Object("wall_1_Geo", Object::COM_TYPE::CT_STATIC));
	_vertex =
	{
		VERTEX(-3.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f),
		VERTEX(-3.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f),
		VERTEX(-2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.5f, 0.0f),
		VERTEX(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.5f, 2.0f)
	};
	Objtemp->Set_Vertex(_vertex);
	Objtemp->Get_Material().DiffuseAlbedo = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Objtemp->Get_Material().FresnelR0 = DirectX::XMFLOAT3(0.05f, 0.05f, 0.05f);
	Objtemp->Get_Material().Roughness = 0.25f;

	//Wall_2
	Objtemp = std::dynamic_pointer_cast<Surface>(UTIL.Get_Object("wall_2_Geo", Object::COM_TYPE::CT_STATIC));
	_vertex =
	{
		VERTEX(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f),
		VERTEX(2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f),
		VERTEX(7.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 2.0f, 0.0f),
		VERTEX(7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 2.0f, 2.0f)
	};
	Objtemp->Set_Vertex(_vertex);
	Objtemp->Get_Material().DiffuseAlbedo = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Objtemp->Get_Material().FresnelR0 = DirectX::XMFLOAT3(0.05f, 0.05f, 0.05f);
	Objtemp->Get_Material().Roughness = 0.25f;

	//Wall_3
	Objtemp = std::dynamic_pointer_cast<Surface>(UTIL.Get_Object("wall_3_Geo", Object::COM_TYPE::CT_STATIC));
	_vertex =
	{
		VERTEX(-3.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f),
		VERTEX(-3.5f, 6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f),
		VERTEX(7.5f,  6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 6.0f, 0.0f),
		VERTEX(7.5f,  4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 6.0f, 1.0f)
	};
	Objtemp->Set_Vertex(_vertex);
	Objtemp->Get_Material().DiffuseAlbedo = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Objtemp->Get_Material().FresnelR0 = DirectX::XMFLOAT3(0.05f, 0.05f, 0.05f);
	Objtemp->Get_Material().Roughness = 0.25f;

	//Mirror
	auto Objtemp2 = std::dynamic_pointer_cast<Mirror>(UTIL.Get_Object("mirrorGeo", Object::COM_TYPE::CT_STATIC));
	_vertex =
	{
		VERTEX(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f),
		VERTEX(-2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f),
		VERTEX(2.5f,  4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f),
		VERTEX(2.5f,  0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f)
	};
	Objtemp2->Set_Vertex(_vertex);
	Objtemp2->Get_Material().DiffuseAlbedo = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 0.3f);
	Objtemp2->Get_Material().FresnelR0 = DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f);
	Objtemp2->Get_Material().Roughness = 0.5f;
}


void CMainApp::Free(void)
{
}
