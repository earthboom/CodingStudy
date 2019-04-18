#include "stdafx.h"
#include "MainApp.h"
#include "Graphic_Manager.h"
#include "Mouse_Manager.h"
#include "Function.h"

CMainApp::CMainApp(void)
	: m_Shape(nullptr)
	//,m_Box(nullptr)
	, m_fTime(0.0f)
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
	ThrowIfFailed(GRAPHIC->Get_CommandList()->Reset(GRAPHIC->Get_CommandAllocator().Get(), nullptr));

	//m_Box = Box::Create();
	//m_Box->OnResize();

	m_Shape = Shape::Create();
	m_Shape->OnResize();

	//MOUSE.Set_Obj(m_Box);
	MOUSE.Set_Obj(m_Shape);

	//Execute the initialization commands.
	ThrowIfFailed(GRAPHIC->Get_CommandList()->Close());

	ID3D12CommandList* cmdLists[] = { GRAPHIC->Get_CommandList().Get() };
	GRAPHIC->Get_CommandQueue()->ExecuteCommandLists(_countof(cmdLists), cmdLists);

	//Wait until initialization is complete.
	GRAPHIC->FlushCommandQueue();

	return TRUE;
}

int CMainApp::Update_MainApp(const float & dt)
{
	//m_Box->Update(dt);
	m_Shape->Update(dt);

	return 0;
}

void CMainApp::Render_MainApp(const float& dt)
{
	//GRAPHIC_MGR.Draw(dt);
	//m_Box->Render(dt);
	m_Shape->Render(dt);
}

void CMainApp::Free(void)
{
}
