#pragma once

using namespace ECS_tool;

#define RES_MGR ResourceMgr::GetInstance()

class ResourceMgr : public Singleton<ResourceMgr>
{
public:
	explicit ResourceMgr(void);
	ResourceMgr(const ResourceMgr&) = delete;
	ResourceMgr operator=(const ResourceMgr&) = delete;
	~ResourceMgr(void);

public:
	void Ready_FbxManager(void);
	
	BOOL Import(std::string _filename);
	BOOL Ready_Mesh(void);

private:
	void LoadNode(FbxNode* _node);
	void ProcessControlPoints(FbxMesh* _mesh);
	XMFLOAT3 ReadNormal(const FbxMesh* _mesh, int _controlPointIndex, int _vertexCounter);
	XMFLOAT3 ReadBinormal(const FbxMesh* _mesh, int _controlPointIndex, int _vertexCounter);
	XMFLOAT3 ReadTangent(const FbxMesh* _mesh, int _controlPointIndex, int _vertexCounter);
	XMFLOAT2 ReadUV(const FbxMesh* _mesh, int _controlPointIndex, int _vertexCounter);

	void IndextVertex(const XMFLOAT3& _pos, const XMFLOAT3& _normal, const XMFLOAT2& _uv, const XMFLOAT3& _binormal, const XMFLOAT3& _tangent);

	   
	int HierarchyLoad(std::vector<Node*>* destHierarchy);

	static void LoadAnimStack(FbxAnimStack* _animStack, FbxNode* _root, std::vector<Node*>* destHierarchy);
	static void RecursiveNode(FbxNode* _root, FbxAnimLayer* _animLayer, const int _index, const int _parentIndex, std::vector<Node*>* destHierarchy, const int _depth);
	static void GetCurveValues(FbxAnimCurve* _animCurveX, FbxAnimCurve* _animCurveY, FbxAnimCurve* _cnimCurveZ, std::map<int, XMFLOAT3>* _dic);

	
	BOOL MeshLoad(std::vector<VTXBONE*>* _vertices, std::vector<Material*>* _materials, std::vector<Node*>* _nodeHierarchy);

	static void RecursiveNode(FbxNode* _node, std::vector<VTXBONE*>* _vertices, std::vector<Material*>* _materials, std::vector<Node*>* _nodeHierarchy);
	static void ExtractVertex(FbxNode* _node, FbxMesh* _mesh, std::vector<VTXBONE*>* _vertices, std::vector<Node*>* _nodeHierarchy);
	static void ExtractControlPoints(FbxMesh* _mesh, std::vector<ControlPoint*>* _cps);
	static void ExtractWeight(FbxMesh* _mesh, FbxNode* _node, std::vector<ControlPoint*>* _cps, std::vector<Node*>* _nodeHierarchy);

	static XMFLOAT3		ExtractNormal(const FbxMesh* _mesh, const int _cpIndex, const int _vertexCounter);
	static XMFLOAT2		ExtractUV(const FbxMesh* _mesh, const int _cpIndex, const int _vertexCounter);
	static FbxAMatrix	ExtractBoneGlobalToLocalMatrix(FbxCluster* _cluster, FbxNode* _node);
	static FbxAMatrix	GetGeometryTransform(FbxNode* _node);
	static void			ExtractMaterial(FbxSurfaceMaterialPtr _surfaceMatPtr, std::string* _nodeName, const int _polygonCount, std::vector<Material*>* _materials);
	static std::string*	ExtractTextureName(FbxProperty _prop);

	static int FindNodeIndexUsingName(std::string* _name, std::vector<Node*>* _nodeHierarchy);

private:
	FbxManager* m_pFbxmgr;
	FbxScene*	m_pFbxScene;
	FbxNode*	m_pRootNode;
	VTXBONE*	m_pVtxbone;

	std::vector<VTXBONE>	m_vecVtxbone;
	std::vector<UINT>		m_vecIndices;

	std::unordered_map<VTXBONE, UINT, VTXBONE> m_mapIndexMapping;

	std::vector<Node*>* m_vecNodeHierarchy;	//뼈대
	std::vector<VTXBONE*>* m_vecVertices;		//정점
	std::vector<Material*>* m_vecMaterials;	//재질

	static int m_iFrameCount;

public:
	FbxManager*& Get_FbxManager(void) { return m_pFbxmgr; }

	std::vector<VTXBONE>&						Get_vecVtxbone(void)		{ return m_vecVtxbone; }
	std::vector<UINT>&							Get_vecIndices(void)		{ return m_vecIndices; }
	std::unordered_map<VTXBONE, UINT, VTXBONE>&	Get_mapIndexMapping(void)	{ return m_mapIndexMapping; }
};

