#include "pch.h"
#include "UtiltyMgr.h"

UtiltyMgr::UtiltyMgr(void)
{
	m_mapStaticObj.clear();
	m_mapDynamicObj.clear();
	m_vecObjMap.clear();

	m_vecObjMap.push_back(m_mapStaticObj);
	m_vecObjMap.push_back(m_mapDynamicObj);
}

UtiltyMgr::~UtiltyMgr(void)
{
}

void UtiltyMgr::Object_Initialize(void)
{
	for (auto& mapObj : m_vecObjMap)
	{
		for (MAP_OBJECT::iterator iter = mapObj.begin(); iter != mapObj.end(); ++iter)
		{
			(*iter->second).Initialize();
		}
	}
}

void UtiltyMgr::Object_Update(DX::StepTimer const& timer)
{
	for (auto& mapObj : m_vecObjMap)
	{
		for (MAP_OBJECT::iterator iter = mapObj.begin(); iter != mapObj.end(); ++iter)
		{
			(*iter->second).Update(timer);
		}
	}
}

void UtiltyMgr::Object_Render(DX::StepTimer const& timer)
{
	for (auto& mapObj : m_vecObjMap)
	{
		for (MAP_OBJECT::iterator iter = mapObj.begin(); iter != mapObj.end(); ++iter)
		{
			(*iter->second).Render(timer);
		}
	}
}
