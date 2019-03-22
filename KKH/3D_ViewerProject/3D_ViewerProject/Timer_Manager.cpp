#include "stdafx.h"
#include "Timer_Manager.h"

CTimer_Manager::CTimer_Manager(void)
{
}

CTimer_Manager::~CTimer_Manager(void)
{
	Free();
}

HRESULT CTimer_Manager::Ready_Timer(const TCHAR * pTag)
{
	
	return S_OK;
}

void CTimer_Manager::Compute_Timer(const TCHAR * pTag)
{
}

float CTimer_Manager::Get_TimeDelta(const TCHAR * pTag)
{
	return 0.0f;
}

PTIMER CTimer_Manager::Find_Timer(const TCHAR * pTag)
{
	auto pFindTimer = [pTag](std::map<const TCHAR*, PTIMER>::value_type const& itr)->auto { if (!strcmp(itr.first, pTag)) return itr; };

	auto pTimer = find(begin(m_mapTimer), end(m_mapTimer), pFindTimer);
	return PTIMER();
}

void CTimer_Manager::Free(void)
{
	for_each(begin(m_mapTimer), end(m_mapTimer), [](auto& mapTimer) { mapTimer.second.reset(); });
}
