#include "stdafx.h"
#include "MainApp.h"
#include "Graphic_Manager.h"
#include "Mouse_Manager.h"
#include "Function.h"
#include "Utility_Manager.h"
#include "Box.h"
#include "LitColumn.h"
#include "Grid.h"
#include "Texture_Manger.h"

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

	if (!TEX.onDDSLoad("grassTex", L"../Texture/grass.dds")) return FALSE;

	UTIL.BuildRootSignature();
	UTIL.BuildDescriptorHeaps();
	UTIL.BuildShadersAndInputLayer();

	CreateObject();
	if (!UTIL.Object_Cycle(0.0f, Utility_Manager::OS_READY)) return FALSE;

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

int CMainApp::Update_MainApp(const float & dt)
{
	//m_Box->Update(dt);
	//m_Shape->Update(dt);
	//m_LAW->Update(dt);

	if (UTIL.Object_Cycle(dt, Utility_Manager::OS_UPDATE)) return 0;

	return 1;
}

void CMainApp::Render_MainApp(const float& dt)
{
	//GRAPHIC_MGR.Draw(dt);
	//m_Box->Render(dt);
	//m_Shape->Render(dt);
	//m_LAW->Render(dt);

	if (UTIL.Object_Cycle(dt, Utility_Manager::OS_RENDER)) return;
}

bool CMainApp::CreateObject(void)
{
	//if (!UTIL.Object_Create(Box::Create("box", Object::COM_TYPE::CT_STATIC, "BoxGeo", 1.5f, 0.5f, 1.5f, 3))) return FALSE;
	//if(!UTIL.Object_Create(LitColumn::Create("litcolumn", Object::COM_TYPE::CT_STATIC))) return FALSE;

	if (!UTIL.Object_Create(Grid::Create(Object::COM_TYPE::CT_STATIC, "landGeo", "grid", "grassTex", "grass"))) return FALSE;

	return TRUE;
}

void CMainApp::Free(void)
{
}
