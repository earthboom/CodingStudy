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

HRESULT CMainApp::Ready_MainApp(void)
{
	//GRAPHIC_MGR.Init_Graphic(WINSIZE_X, WINSIZE_Y, g_hWnd);

	return S_OK;
}

int CMainApp::Update_MainApp(const float & fDeltaTime)
{
	return 0;
}

void CMainApp::Render_MainApp(void)
{
	GRAPHIC_MGR.Get_Graphic()->BeginScene(0.6f, 0.6f, 0.6f, 1.0f);

	GRAPHIC_MGR.Get_Graphic()->EndScene();
}

void CMainApp::Free(void)
{
}
