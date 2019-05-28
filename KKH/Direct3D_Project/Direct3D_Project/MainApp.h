#pragma once

//#include "BoxScene.h"
//#include "ShapeScene.h"
//#include "LandAndWave.h"

#include "Timer.h"

class CMainApp
{
public:
	explicit CMainApp(void);
	~CMainApp(void);

public:
	bool	Ready_MainApp(void);
	int		Update_MainApp(const CTimer& mt);
	void	Render_MainApp(const CTimer& mt);

private:
	bool	LoadTexture(void);
	bool	CreateObject(void);
	
	void	SettingRoom(void);

private:
	//PBOX	m_Box;
	//SHAPE m_Shape;
	//LAW m_LAW;

private:
	float	m_fTime;

private:
	void	Free(void);
};

typedef std::shared_ptr<CMainApp>	PMAINAPP;