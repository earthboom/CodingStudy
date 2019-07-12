#pragma once

#include "Timer.h"

#define TIME_MGR	CTimer_Manager::GetInstnace()

class CTimer_Manager : public CSingleton<CTimer_Manager>
{
public:
	enum TimerType
	{
		TIMER_MAIN,
		TIMER_END
	};

public:
	explicit CTimer_Manager(void);
	CTimer_Manager(const CTimer_Manager&) = delete;
	CTimer_Manager & operator = (const CTimer_Manager&) = delete;
	~CTimer_Manager(void);

public:
	HRESULT	Ready_Timer(const TimerType pTag);
	void	Compute_Timer(const TimerType pTag);

	PTIMER	Get_Timer(const TimerType pTag);

public:
	float	Get_TimeDelta(const TimerType pTag);
	float	Get_TotalTime(const TimerType pTag);

	bool	Get_Start(const TimerType pTag);
	bool	Get_Reset(const TimerType pTag);
	bool	Get_Stop(const TimerType pTag);

private:
	std::map<const TimerType, PTIMER> m_mapTimer;
	typedef std::map<const TimerType, PTIMER>	MAP_TIMER;

	PTIMER	Find_Timer(const TimerType pTag);

private:
	void Free(void);
};