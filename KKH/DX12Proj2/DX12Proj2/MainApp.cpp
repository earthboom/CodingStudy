#include "pch.h"
#include "MainApp.h"

MainApp::MainApp(void)
	: m_fTime(0.0f)
{
}

MainApp::~MainApp(void)
{
}

bool MainApp::Ready_MainApp(void)
{
	return TRUE;
}

int MainApp::Update_MainApp(const CTimer& mt)
{
	return 0;
}

void MainApp::Render_MainApp(const CTimer& mt)
{
}
