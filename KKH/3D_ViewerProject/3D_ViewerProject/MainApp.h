#pragma once

class CMainApp
{
public:
	explicit CMainApp(void);
	~CMainApp(void);

public:
	HRESULT	Ready_MainApp(void);
	int		Update_MainApp(const float& fDeltaTime);
	void	Render_MainApp(void);

private:
	float	m_fTime;

private:
	void	Free(void);
};

typedef std::shared_ptr<CMainApp>	PMAINAPP;