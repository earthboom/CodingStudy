#pragma once

#include "Timer.h"
#include "Unique_Singleton.h"

#define TIME_MGR	CTimer_Manager::GetInstnace()

class CTimer_Manager : public CSingleton<CTimer_Manager>
{
public:
	explicit CTimer_Manager(void);
	CTimer_Manager(const CTimer_Manager&) = delete;
	CTimer_Manager & operator = (const CTimer_Manager&) = delete;
	~CTimer_Manager(void);

public:
	HRESULT	Ready_Timer(const TCHAR* pTag);
	void	Compute_Timer(const TCHAR* pTag);

public:
	float	Get_TimeDelta(const TCHAR* pTag);

private:
	std::map<const TCHAR*, PTIMER> m_mapTimer;
	typedef std::map<const TCHAR*, PTIMER>	MAP_TIMER;

	PTIMER	Find_Timer(const TCHAR* pTag);

private:
	void Free(void);
};