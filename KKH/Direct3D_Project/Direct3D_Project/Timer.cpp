#include "stdafx.h"
#include "Timer.h"

CTimer::CTimer(void)
	: m_dbDeltaTime(-1.0f)
	, m_dbSecondPerCount(0.0f)
	, m_i64BaseTime(0)
	, m_i64CurrTime(0)
	, m_i64PausedTime(0)
	, m_i64StopTime(0)
	, m_i64PrevTime(0)
	, m_bStopped(FALSE)
{
	__int64 CntPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&CntPerSec);
	m_dbSecondPerCount = 1.0f / CntPerSec;
}

CTimer::~CTimer(void)
{
}

float CTimer::TotalTime(void) const
{
	if (m_bStopped)
		return (float)(((m_i64StopTime - m_i64PausedTime) - m_i64BaseTime) * m_dbSecondPerCount);
	else
		return (float)(((m_i64CurrTime - m_i64PausedTime) - m_i64BaseTime) * m_dbSecondPerCount);
}

float CTimer::DeltaTime(void) const
{
	return (float)m_dbDeltaTime;
}

void CTimer::Reset(void)
{
	__int64 currTime = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	m_i64BaseTime = currTime;
	m_i64PrevTime = currTime;
	m_i64StopTime = 0;
	m_bStopped = FALSE;
}

void CTimer::Start(void)
{
	__int64 startTime = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	if (m_bStopped)	//중단 상태에서 타이머 다시 시작할때
	{
		m_i64PausedTime += (startTime - m_i64StopTime);	//일시 정지 시간 누적

		//타이머를 재시작하는 것으므로, 이전 시간의 현재 값은 유효하지 않음(일시정지 도중에 갱신된 값이기 때문)
		//현재 시간으로 다시 세팅
		m_i64PrevTime = startTime;

		m_i64StopTime = 0;
		m_bStopped = FALSE;
	}
}

void CTimer::Stop(void)
{
	//이미 정지라면 무시
	if (!m_bStopped)
	{
		__int64 currTime = 0;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
		
		//일시 정지된 시점의 시간을 저장
		m_i64StopTime = currTime;
		m_bStopped = TRUE;
	}
}

void CTimer::Tick(void)
{
	if (m_bStopped)
	{
		m_dbDeltaTime = 0.0f;
		return;
	}

	//지금 프레임 시간
	__int64 currTime = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	m_i64CurrTime = currTime;

	//이 시간과 이전 프레임 시간의 차이
	m_dbDeltaTime = (m_i64CurrTime - m_i64PrevTime) * m_dbSecondPerCount;

	//다음 프레임 준비
	m_i64PrevTime = m_i64CurrTime;

	//음수가 되지 않도록 함
	//프로세서가 절전모드, 다른 프로세서와 엉키는 예외상황일 경우 Delta값이 음수가 될 수도 있음.
	if (m_dbDeltaTime < 0.0f)
		m_dbDeltaTime = 0.0f;
}
