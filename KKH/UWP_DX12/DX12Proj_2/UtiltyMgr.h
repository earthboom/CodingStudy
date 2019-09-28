#pragma once

#include "Object.h"

#define UTIL UtiltyMgr::GetInstance()

class UtiltyMgr : public Singleton<UtiltyMgr>
{
public:
	explicit UtiltyMgr(void);
	UtiltyMgr(const UtiltyMgr&) = delete;
	UtiltyMgr& operator=(const UtiltyMgr&) = delete;
	~UtiltyMgr(void);
	
public:
	void Object_Initialize(void);
	void Object_Update(DX::StepTimer const& timer);
	void Object_Render(DX::StepTimer const& timer);

private:
	typedef std::map<WORD, POBJECT> MAP_OBJECT;
	MAP_OBJECT m_mapStaticObj;
	MAP_OBJECT m_mapDynamicObj;

	std::vector<MAP_OBJECT> m_vecObjMap;
};