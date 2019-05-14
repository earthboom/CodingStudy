#pragma once

#include "Unique_Singleton.h"
#include "GraphicDev.h"

#define GRAPHIC_MGR CGraphic_Manager::GetInstnace()
#define GRAPHIC CGraphic_Manager::GetInstnace().Get_Graphic()
#define GRAPHIC_DEV	CGraphic_Manager::GetInstnace().Get_Graphic()->Get_Device()
#define COM_LIST GRAPHIC->Get_CommandList()

class CGraphic_Manager : public CSingleton<CGraphic_Manager>
{
public:
	explicit CGraphic_Manager(void);
	CGraphic_Manager(const CGraphic_Manager&) = delete;
	CGraphic_Manager& operator=(const CGraphic_Manager&) = delete;
	~CGraphic_Manager(void);

public:
	bool Init_Graphic(void);
	void Draw(const float& dt);

private:
	PGRAPHIC	m_pGraphic;

public:
	std::function< PGRAPHIC&()>	Get_Graphic = [&]()->PGRAPHIC& { return m_pGraphic; };
};