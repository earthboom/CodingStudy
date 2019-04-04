#include "stdafx.h"
#include "MainApp.h"
#include "Graphic_Manager.h"

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
	//GRAPHIC_MGR.Init_Graphic(WINSIZE_X, WINSIZE_Y, g_hWnd);
	if (!GRAPHIC_MGR.Init_Graphic())
		return FALSE;

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
