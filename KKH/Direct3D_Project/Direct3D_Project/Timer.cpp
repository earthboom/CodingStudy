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

	if (m_bStopped)	//�ߴ� ���¿��� Ÿ�̸� �ٽ� �����Ҷ�
	{
		m_i64PausedTime += (startTime - m_i64StopTime);	//�Ͻ� ���� �ð� ����

		//Ÿ�̸Ӹ� ������ϴ� �����Ƿ�, ���� �ð��� ���� ���� ��ȿ���� ����(�Ͻ����� ���߿� ���ŵ� ���̱� ����)
		//���� �ð����� �ٽ� ����
		m_i64PrevTime = startTime;

		m_i64StopTime = 0;
		m_bStopped = FALSE;
	}
}

void CTimer::Stop(void)
{
	//�̹� ������� ����
	if (!m_bStopped)
	{
		__int64 currTime = 0;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
		
		//�Ͻ� ������ ������ �ð��� ����
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

	//���� ������ �ð�
	__int64 currTime = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	m_i64CurrTime = currTime;

	//�� �ð��� ���� ������ �ð��� ����
	m_dbDeltaTime = (m_i64CurrTime - m_i64PrevTime) * m_dbSecondPerCount;

	//���� ������ �غ�
	m_i64PrevTime = m_i64CurrTime;

	//������ ���� �ʵ��� ��
	//���μ����� �������, �ٸ� ���μ����� ��Ű�� ���ܻ�Ȳ�� ��� Delta���� ������ �� ���� ����.
	if (m_dbDeltaTime < 0.0f)
		m_dbDeltaTime = 0.0f;
}
