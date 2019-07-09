#pragma once

class CTimer;
class MainApp
{
public:
	explicit MainApp(void);
	MainApp(const MainApp&) = delete;
	MainApp& operator = (const MainApp&) = delete;
	~MainApp(void);

public:
	bool Ready_MainApp(void);
	int  Update_MainApp(const CTimer& mt);
	void Render_MainApp(const CTimer& mt);

private:
	float m_fTime;
};

typedef std::unique_ptr<MainApp> PMAIN;