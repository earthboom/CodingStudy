#pragma once

#include "Unique_Singleton.h"
#include "GraphicDev.h"

#define GRAPHIC_MGR CGraphic_Manager::GetInstnace()

class CGraphic_Manager : public CSingleton<CGraphic_Manager>
{
public:
	explicit CGraphic_Manager(void);
	CGraphic_Manager(const CGraphic_Manager&) = delete;
	CGraphic_Manager& operator=(const CGraphic_Manager&) = delete;
	~CGraphic_Manager(void);

public:
	//bool Init_Graphic(const WORD _width, const WORD _height, HWND _hwnd);
	//void ShutDown(void);

	bool Init_Graphic(void);

private:
	PGRAPHIC	m_pGraphic;

public:
	std::function< PGRAPHIC&()>	Get_Graphic = [&]()->PGRAPHIC& { return m_pGraphic; };
};