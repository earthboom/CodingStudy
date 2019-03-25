#pragma once

class CTimer
{
public:
	explicit CTimer(void);
	~CTimer(void);

public:
	float TotalTime(void) const;
	float DeltaTime(void) const;

	void Reset(void);
	void Start(void);
	void Stop(void);
	void Tick(void);

private:
	double	m_dbSecondPerCount;
	double	m_dbDeltaTime;

	__int64	m_i64BaseTime;
	__int64	m_i64PausedTime;
	__int64	m_i64StopTime;
	__int64	m_i64PrevTime;
	__int64	m_i64CurrTime;

	bool m_bStopped;
};

typedef std::shared_ptr<CTimer>	PTIMER;