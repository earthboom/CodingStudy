#include "stdafx.h"
#include "MainApp.h"
#include "Graphic_Manager.h"
#include "Function.h"

CMainApp::CMainApp(void)
	: m_fTime(0.0f)
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


	return TRUE;
}

int CMainApp::Update_MainApp(const float & dt)
{
	return 0;
}

void CMainApp::Render_MainApp(const float& dt)
{
	GRAPHIC_MGR.Draw(dt);
}

void CMainApp::Free(void)
{
}
