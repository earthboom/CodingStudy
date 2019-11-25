#pragma once

typedef std::map<WORD, POBJECT> MAP_OBJ;
class UtilityMgr : public Singleton<UtilityMgr>
{
public:
	explicit UtilityMgr(void);
	UtilityMgr(const UtilityMgr&) = delete;
	UtilityMgr& operator=(const UtilityMgr&) = delete;
	~UtilityMgr(void);

public:
	void Object_Initialize(void);
	void Object_Update(DX::StepTimer const& timer);
	void Object_Render(void);

private:
	MAP_OBJ	m_mapStaticObj;
	MAP_OBJ m_mapDynamicObj;

	std::vector<MAP_OBJ> m_vecObjMap;
};