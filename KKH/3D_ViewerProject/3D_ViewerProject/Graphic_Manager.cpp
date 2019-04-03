#include "stdafx.h"
#include "Graphic_Manager.h"

CGraphic_Manager::CGraphic_Manager(void)
	: m_pGraphic(nullptr)
{
}

CGraphic_Manager::~CGraphic_Manager(void)
{
}

bool CGraphic_Manager::Init_Graphic(void)
{
	m_pGraphic = std::make_shared<CGraphicDev>();
	if (!m_pGraphic)	return FALSE;

	if (!m_pGraphic->Init_Graphic())
		return FALSE;

	m_pGraphic->OnResize();

	return TRUE;
}

//bool CGraphic_Manager::Init_Graphic(const WORD _width, const WORD _height, HWND _hwnd)
//{
//	m_pGraphic = std::make_shared<CGraphicDev>();
//	if (!m_pGraphic)	return FALSE;
//
//	if (!m_pGraphic->Init_Graphic(CGraphicDev::WINDOW_MODE::MODE_WIN, _hwnd, _width, _height, TRUE))
//		return FALSE;	
//
//	return TRUE;
//}
//
//void CGraphic_Manager::ShutDown(void)
//{
//	if (m_pGraphic)
//	{
//		m_pGraphic->ShutDown();
//		m_pGraphic.reset();
//		m_pGraphic = nullptr;
//	}
//
//	return;
//}