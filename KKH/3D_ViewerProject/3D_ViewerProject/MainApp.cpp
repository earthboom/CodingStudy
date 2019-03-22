#include "stdafx.h"
#include "MainApp.h"

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
	return S_OK;
}

int CMainApp::Update_MainApp(const float & fDeltaTime)
{
	return 0;
}

void CMainApp::Render_MainApp(void)
{
}

void CMainApp::Free(void)
{
}
