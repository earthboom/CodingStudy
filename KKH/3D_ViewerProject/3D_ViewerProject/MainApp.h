#pragma once

#include "Box.h";

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
	PBOX	m_Box;

private:
	float	m_fTime;

private:
	void	Free(void);
};

typedef std::shared_ptr<CMainApp>	PMAINAPP;