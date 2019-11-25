#include "pch.h"
#include "UtilityMgr.h"

UtilityMgr::UtilityMgr(void)
{
	m_mapStaticObj.clear();
	m_mapDynamicObj.clear();
	m_vecObjMap.clear();

	m_vecObjMap.push_back(m_mapStaticObj);
	m_vecObjMap.push_back(m_mapDynamicObj);
}

UtilityMgr::~UtilityMgr(void)
{
	m_mapStaticObj.clear();
	m_mapDynamicObj.clear();
	m_vecObjMap.clear();
}

void UtilityMgr::Object_Initialize(void)
{
	for (auto& _map : m_vecObjMap)
	{
		for (MAP_OBJ::iterator iter = _map.begin(); iter != _map.end(); ++iter)
		{
			(*iter->second).Initialize();
		}
	}
}

void UtilityMgr::Object_Update(DX::StepTimer const& timer)
{
	for (auto& _map : m_vecObjMap)
	{
		for (MAP_OBJ::iterator iter = _map.begin(); iter != _map.end(); ++iter)
		{
			(*iter->second).Update(timer);
		}
	}
}

void UtilityMgr::Object_Render(void)
{
	for (auto& _map : m_vecObjMap)
	{
		for (MAP_OBJ::iterator iter = _map.begin(); iter != _map.end(); ++iter)
		{
			(*iter->second).Render();
		}
	}
}
