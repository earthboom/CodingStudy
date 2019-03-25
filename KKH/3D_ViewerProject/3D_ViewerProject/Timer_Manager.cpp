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
	auto pTimer = Find_Timer(pTag);
	if (pTimer)	return E_FAIL;

	pTimer = std::make_shared<CTimer>();
	if (!pTimer) return E_FAIL;

	m_mapTimer.insert(MAP_TIMER::value_type(pTag, pTimer));
	return S_OK;
}

void CTimer_Manager::Compute_Timer(const TCHAR * pTag)
{
	auto pTimer = Find_Timer(pTag);
	if (pTimer == nullptr)
		return;

	pTimer->Tick();
}

float CTimer_Manager::Get_TimeDelta(const TCHAR * pTag)
{
	auto pTimer = Find_Timer(pTag);
	if (pTimer == nullptr)
		return 0.0f;

	return pTimer->DeltaTime();
}

PTIMER CTimer_Manager::Find_Timer(const TCHAR * pTag)
{
	//auto pFindTimer = [pTag](auto& itr)->bool { 	if (!_tcscmp(itr.first, pTag))	return TRUE;	else return FALSE;};

	//auto pTimer = find(begin(m_mapTimer), end(m_mapTimer), pFindTimer);

	auto pTimer = find_if(begin(m_mapTimer), end(m_mapTimer), [pTag](const MAP_TIMER::value_type& _timer) { if (!_tcscmp(_timer.first, pTag)) return TRUE;	else return FALSE; });
	if (pTimer == m_mapTimer.end())
		return nullptr;

	return pTimer->second;
}

void CTimer_Manager::Free(void)
{
	for_each(begin(m_mapTimer), end(m_mapTimer), [](auto& mapTimer) { mapTimer.second.reset(); });
}
