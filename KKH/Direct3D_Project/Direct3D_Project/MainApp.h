#pragma once

//#include "BoxScene.h"
//#include "ShapeScene.h"
//#include "LandAndWave.h"

class CMainApp
{
public:
	explicit CMainApp(void);
	~CMainApp(void);

public:
	bool	Ready_MainApp(void);
	int		Update_MainApp(const float& dt);
	void	Render_MainApp(const float& dt);

private:
	bool	LoadTexture(void);
	bool	CreateObject(void);

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