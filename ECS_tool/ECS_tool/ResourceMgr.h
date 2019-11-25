#pragma once

class ResourceMgr : public Singleton<ResourceMgr>
{
public:
	explicit ResourceMgr(void);
	ResourceMgr(const ResourceMgr&) = delete;
	ResourceMgr operator=(const ResourceMgr&) = delete;
	~ResourceMgr(void);

public:
	void Ready_FbxManager(void);
	BOOL Ready_Mesh(void);

private:
	void LoadNode(FbxNode* _node);

private:
	FbxManager* m_pFbxmgr;

public:
	FbxManager*& Get_FbxManager(void) { return m_pFbxmgr; }
};