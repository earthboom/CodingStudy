#include "pch.h"
#include "ResourceMgr.h"

using namespace std;
using namespace DirectX;

int ResourceMgr::m_iFrameCount = 0;

ResourceMgr::ResourceMgr(void)
	: m_pFbxmgr(nullptr)
	, m_pVtxbone(nullptr)
{
	Ready_FbxManager();

	m_vecNodeHierarchy = new std::vector<Node*>();
	m_vecVertices = new std::vector<VTXBONE*>();
	m_vecMaterials = new std::vector<Material*>();
}

ResourceMgr::~ResourceMgr(void)
{
	if(m_pFbxmgr)
		m_pFbxmgr->Destroy();

	m_vecNodeHierarchy->clear();
	m_vecVertices->clear();
	m_vecMaterials->clear();
}

void ResourceMgr::Ready_FbxManager(void)
{
	if (m_pFbxmgr == nullptr)
	{
		m_pFbxmgr = FbxManager::Create();

		//FbxIOSettings Ŭ������ ���� ��Ҹ� ���Ͽ��� �������ų� ���Ϸ� �������� ���θ� ����.
		FbxIOSettings* ios = FbxIOSettings::Create(m_pFbxmgr, IOSROOT);
		m_pFbxmgr->SetIOSettings(ios);
	}
}

BOOL ResourceMgr::Import(std::string _filename)
{
	FbxString fs_filename = FbxString(_filename.c_str());

	int _fileformat = EOF;
	m_pFbxmgr->GetIOPluginRegistry()->DetectReaderFileFormat(fs_filename, _fileformat);	//������ �ִ��� Ȯ��

	FbxImporter* importer = FbxImporter::Create(m_pFbxmgr, "");
	if (!importer->Initialize(fs_filename, _fileformat, m_pFbxmgr->GetIOSettings()))
	{
		std::cout << "Call to importer->Initialize Failed!!" << std::endl;
		std::cout << "Error : " << importer->GetStatus().GetErrorString() << std::endl;
		exit(-1);
	}

	m_pFbxScene = FbxScene::Create(m_pFbxmgr, "");
	if (!importer->Import(m_pFbxScene))
	{
		std::cout << "Call to importer->Import Failed!!" << std::endl;
		std::cout << "Error : " << importer->GetStatus().GetErrorString() << std::endl;
		exit(-1);
	}

	importer->Destroy();

	FbxNode* node = FbxNode::Create(m_pFbxScene, "");

	//Scene�� ��Ʈ ��带 �����´�.
	m_pRootNode = m_pFbxScene->GetRootNode();

	FbxGeometryConverter geometryConverter(m_pFbxmgr);

	//Scene�� �� �ý���, �ֺ� ���� �� �ð� ������ FbxGlobalSettings ��ü�� ����. 
	//�� ��ü�� FbxScene::GlobalSettings()�� ���� ����
	FbxAxisSystem sceneAxisSystem = m_pFbxScene->GetGlobalSettings().GetAxisSystem();

	//Scene ���� ��ǥ���� �ٲ�.
	FbxAxisSystem::MayaYUp.ConvertScene(m_pFbxScene);

	//Scene ������ �ﰢ��ȭ �� �� �ִ� ��� ��带 �ﰢ��ȭ ��Ų��.	
	geometryConverter.Triangulate(m_pFbxScene, TRUE);

	//VBO ����ϱ� ���ϰ� Material ���� Mesh�� �ɰ���.
	geometryConverter.SplitMeshesPerMaterial(m_pFbxScene, TRUE);

	// Node�� ���� ������ ����
	m_iFrameCount = HierarchyLoad(m_vecNodeHierarchy);

	// Mesh ����(Vertex, Material) ����
	MeshLoad(m_vecVertices, m_vecMaterials, m_vecNodeHierarchy);

	//Ready_Mesh();

	return TRUE;
}


BOOL ResourceMgr::Ready_Mesh(void)
{	
	LoadNode(m_pRootNode);

	FbxMesh* mesh = FbxMesh::Create(m_pFbxScene, "");

	FbxCameraManipulator* cameraManipulator = FbxCameraManipulator::Create(m_pFbxmgr, "");
	

	return TRUE;
}

void ResourceMgr::LoadNode(FbxNode* _node)
{
	//Scene�� ���(mesh, light, camera ��)�� FbxNode�� FbxNodeAttribute�� ���� Ŭ������ �����Ͽ� ����.
	//FbxNodeAttribute�� Scene������ ��ġ�� �����ϱ� ���� FbxNode�� ���ε� ��.
	//�� ���ε� �Ӽ���FbxNode::GetNodeAttribute() ��� �Լ��� ���� ���� ����
	FbxNodeAttribute* _nodeAtt = _node->GetNodeAttribute();

	if (_nodeAtt)
	{
		//Node�� �Ӽ��� Mesh�� ��
		if (_nodeAtt->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			//FbxMesh�� ĳ���õ� Node �Ӽ��� �����͸� ���� ��.
			FbxMesh* _mesh = _node->GetMesh();
			ProcessControlPoints(_mesh);	//���������κ��� ��ġ ����Ʈ�� ä���.

			UINT _triCnt = _mesh->GetPolygonCount();	//mesh�� �ﰢ�� ����
			UINT _vertexCnt = 0;	//���� ����

			for (UINT i = 0; i < _triCnt; ++i)	// �ﰢ���� ����
			{
				for (UINT j = 0; j < 3; ++j)	//�ﰢ���� �� ���� �������� �����Ǿ� �ֱ� ����
				{
					int _controlPointIndex = _mesh->GetPolygonVertex(i, j);	//������ �ε����� ������
					
					XMFLOAT3 _pos = m_pVtxbone[_controlPointIndex].vPos;	//���� ������ ��ġ
					XMFLOAT3 _normal = ReadNormal(_mesh, _controlPointIndex, _vertexCnt);	//Noraml vector
					XMFLOAT3 _binormal = ReadBinormal(_mesh, _controlPointIndex, _vertexCnt);	//Binoraml
					XMFLOAT3 _tangent = ReadTangent(_mesh, _controlPointIndex, _vertexCnt);	//Tangent
					XMFLOAT2 _uv = ReadUV(_mesh, _controlPointIndex, _mesh->GetTextureUVIndex(i, j));	//UV

					IndextVertex(_pos, _normal, _uv, _binormal, _tangent);

					++_vertexCnt;
				}
			}
		}
	}

	const UINT _cnt = _node->GetChildCount();
	for (UINT i = 0; i < _cnt; ++i)
		LoadNode(_node->GetChild(i));
}

void ResourceMgr::ProcessControlPoints(FbxMesh* _mesh)
{
	size_t _cnt = _mesh->GetControlPointsCount();
	m_pVtxbone = new VTXBONE[_cnt];

	for (UINT i = 0; i < _cnt; ++i)
	{
		m_pVtxbone[i].vPos.x = static_cast<float>(_mesh->GetControlPointAt(i).mData[0]);	//x ��ǥ
		m_pVtxbone[i].vPos.y = static_cast<float>(_mesh->GetControlPointAt(i).mData[1]);	//y��ǥ
		m_pVtxbone[i].vPos.z = static_cast<float>(_mesh->GetControlPointAt(i).mData[2]);	//z ��ǥ
	}
}

XMFLOAT3 ResourceMgr::ReadNormal(const FbxMesh* _mesh, int _controlPointIndex, int _vertexCounter)
{
	if (_mesh->GetElementNormalCount() < 1)	//Noraml ������ 0���� ��
		std::cout << "Invalid normal !!" << std::endl;

	const FbxGeometryElementNormal* _vertexNormal = _mesh->GetElementNormal(0);	//�븻 ȹ��
	XMFLOAT3 _result;	//noraml vector�� ������ ����
	int _index = 0;

	switch (_vertexNormal->GetMappingMode())	//Mapping mode
	{
		//control point mapping
		//�� ������(����)���� �ϳ��� ���� ��ǥ�� ������ �ǹ�.
		//�븻 ������ �迭, �������� �迭�� �־����� �븻 ������ �迭�� �������� �迭�� �����Ǵ� ����� �� �� ����.
		//GetReferenceMode() ��� �Լ��� �����Ǵ� ����� �� �� ����.
	case FbxGeometryElement::eByControlPoint:
		switch (_vertexNormal->GetReferenceMode())
		{
			//n��° ��ҿ� ���� ���� ������ FbxLayerElementTemplate::mDirectArray���� n��° ��ġ���� �߰ߵǾ����� ��Ÿ��.
		case FbxGeometryElement::eDirect:
			_result.x = static_cast<float>(_vertexNormal->GetDirectArray().GetAt(_controlPointIndex).mData[0]);
			_result.y = static_cast<float>(_vertexNormal->GetDirectArray().GetAt(_controlPointIndex).mData[1]);
			_result.z = static_cast<float>(_vertexNormal->GetDirectArray().GetAt(_controlPointIndex).mData[2]);
			break;
			
			//FbxLayerElementTemplate::mIndexArray�� �� ��ҿ� FbxLayerElementTemplate::mDirectArray�� ��Ҹ� ��Ÿ���� �ε����� ������ ��Ÿ��.
		case FbxGeometryElement::eIndexToDirect:
			_index = _vertexNormal->GetIndexArray().GetAt(_controlPointIndex);	//index�� ����

			_result.x = static_cast<float>(_vertexNormal->GetDirectArray().GetAt(_index).mData[0]);
			_result.y = static_cast<float>(_vertexNormal->GetDirectArray().GetAt(_index).mData[1]);
			_result.z = static_cast<float>(_vertexNormal->GetDirectArray().GetAt(_index).mData[2]);
			break;
		default:
			std::cout << "Error : Invalid vertex reference mode !" << std::endl;
		}
		break;
	
		//�� ������(����)���� �ϳ��� ���� ��ǥ�� ������ �ǹ�.
	case FbxGeometryElement::eByPolygonVertex:	//polygon vertex mapping
		switch (_vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			_result.x = static_cast<float>(_vertexNormal->GetDirectArray().GetAt(_vertexCounter).mData[0]);
			_result.y = static_cast<float>(_vertexNormal->GetDirectArray().GetAt(_vertexCounter).mData[1]);
			_result.z = static_cast<float>(_vertexNormal->GetDirectArray().GetAt(_vertexCounter).mData[2]);
			break;

		case FbxGeometryElement::eIndexToDirect:
			_index = _vertexNormal->GetIndexArray().GetAt(_vertexCounter);	//index�� ����

			_result.x = static_cast<float>(_vertexNormal->GetDirectArray().GetAt(_index).mData[0]);
			_result.y = static_cast<float>(_vertexNormal->GetDirectArray().GetAt(_index).mData[1]);
			_result.z = static_cast<float>(_vertexNormal->GetDirectArray().GetAt(_index).mData[2]);
			break;
		default:
			std::cout << "Error : Invalid vertex reference mode !" << std::endl;
		}
		break;
	}

	return _result;
}

XMFLOAT3 ResourceMgr::ReadBinormal(const FbxMesh* _mesh, int _controlPointIndex, int _vertexCounter)
{
	if (_mesh->GetElementBinormalCount() < 1)	//Binormal ������ 0���� ��
		std::cout << "Invalid Binormal !!" << std::endl;

	const FbxGeometryElementBinormal* _BiNormal = _mesh->GetElementBinormal(0);	//Binormal ȹ��
	XMFLOAT3 _result;	
	int _index = 0;

	switch (_BiNormal->GetMappingMode())	//Mapping mode
	{
	case FbxGeometryElement::eByControlPoint:
		switch (_BiNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			_result.x = static_cast<float>(_BiNormal->GetDirectArray().GetAt(_controlPointIndex).mData[0]);
			_result.y = static_cast<float>(_BiNormal->GetDirectArray().GetAt(_controlPointIndex).mData[1]);
			_result.z = static_cast<float>(_BiNormal->GetDirectArray().GetAt(_controlPointIndex).mData[2]);
			break;

		case FbxGeometryElement::eIndexToDirect:
			_index = _BiNormal->GetIndexArray().GetAt(_controlPointIndex);	//index�� ����

			_result.x = static_cast<float>(_BiNormal->GetDirectArray().GetAt(_index).mData[0]);
			_result.y = static_cast<float>(_BiNormal->GetDirectArray().GetAt(_index).mData[1]);
			_result.z = static_cast<float>(_BiNormal->GetDirectArray().GetAt(_index).mData[2]);
			break;
		default:
			std::cout << "Error : Invalid vertex reference mode !" << std::endl;
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch (_BiNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			_result.x = static_cast<float>(_BiNormal->GetDirectArray().GetAt(_vertexCounter).mData[0]);
			_result.y = static_cast<float>(_BiNormal->GetDirectArray().GetAt(_vertexCounter).mData[1]);
			_result.z = static_cast<float>(_BiNormal->GetDirectArray().GetAt(_vertexCounter).mData[2]);
			break;

		case FbxGeometryElement::eIndexToDirect:
			_index = _BiNormal->GetIndexArray().GetAt(_vertexCounter);	//index�� ����

			_result.x = static_cast<float>(_BiNormal->GetDirectArray().GetAt(_index).mData[0]);
			_result.y = static_cast<float>(_BiNormal->GetDirectArray().GetAt(_index).mData[1]);
			_result.z = static_cast<float>(_BiNormal->GetDirectArray().GetAt(_index).mData[2]);
			break;
		default:
			std::cout << "Error : Invalid vertex reference mode !" << std::endl;
		}
		break;
	}

	return _result;
}

XMFLOAT3 ResourceMgr::ReadTangent(const FbxMesh* _mesh, int _controlPointIndex, int _vertexCounter)
{
	if (_mesh->GetElementTangentCount() < 1)	//Tangent ������ 0���� ��
		std::cout << "Invalid Tangent !!" << std::endl;

	const FbxGeometryElementTangent* _tangent = _mesh->GetElementTangent(0);	//Tangent ȹ��
	XMFLOAT3 _result;
	int _index = 0;

	switch (_tangent->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (_tangent->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			_result.x = static_cast<float>(_tangent->GetDirectArray().GetAt(_controlPointIndex).mData[0]);
			_result.y = static_cast<float>(_tangent->GetDirectArray().GetAt(_controlPointIndex).mData[1]);
			_result.z = static_cast<float>(_tangent->GetDirectArray().GetAt(_controlPointIndex).mData[2]);
			break;

		case FbxGeometryElement::eIndexToDirect:
			_index = _tangent->GetIndexArray().GetAt(_controlPointIndex);	//index�� ����

			_result.x = static_cast<float>(_tangent->GetDirectArray().GetAt(_index).mData[0]);
			_result.y = static_cast<float>(_tangent->GetDirectArray().GetAt(_index).mData[1]);
			_result.z = static_cast<float>(_tangent->GetDirectArray().GetAt(_index).mData[2]);
			break;
		default:
			std::cout << "Error : Invalid vertex reference mode !" << std::endl;
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch (_tangent->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			_result.x = static_cast<float>(_tangent->GetDirectArray().GetAt(_vertexCounter).mData[0]);
			_result.y = static_cast<float>(_tangent->GetDirectArray().GetAt(_vertexCounter).mData[1]);
			_result.z = static_cast<float>(_tangent->GetDirectArray().GetAt(_vertexCounter).mData[2]);
			break;

		case FbxGeometryElement::eIndexToDirect:
			_index = _tangent->GetIndexArray().GetAt(_vertexCounter);	//index�� ����

			_result.x = static_cast<float>(_tangent->GetDirectArray().GetAt(_index).mData[0]);
			_result.y = static_cast<float>(_tangent->GetDirectArray().GetAt(_index).mData[1]);
			_result.z = static_cast<float>(_tangent->GetDirectArray().GetAt(_index).mData[2]);
			break;
		default:
			std::cout << "Error : Invalid vertex reference mode !" << std::endl;
		}
		break;
	}

	return _result;
}

XMFLOAT2 ResourceMgr::ReadUV(const FbxMesh* _mesh, int _controlPointIndex, int _vertexCounter)
{
	if (_mesh->GetElementUVCount() < 1)	//Tangent ������ 0���� ��
		std::cout << "Invalid UV !!" << std::endl;

	const FbxGeometryElementUV* _uv = _mesh->GetElementUV(0);	//Tangent ȹ��
	XMFLOAT2 _result;
	int _index = 0;

	switch (_uv->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (_uv->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			_result.x = static_cast<float>(_uv->GetDirectArray().GetAt(_controlPointIndex).mData[0]);
			_result.y = static_cast<float>(_uv->GetDirectArray().GetAt(_controlPointIndex).mData[1]);
			break;

		case FbxGeometryElement::eIndexToDirect:
			_index = _uv->GetIndexArray().GetAt(_controlPointIndex);	//index�� ����

			_result.x = static_cast<float>(_uv->GetDirectArray().GetAt(_index).mData[0]);
			_result.y = static_cast<float>(_uv->GetDirectArray().GetAt(_index).mData[1]);
			break;
		default:
			std::cout << "Error : Invalid vertex reference mode !" << std::endl;
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch (_uv->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			_result.x = static_cast<float>(_uv->GetDirectArray().GetAt(_vertexCounter).mData[0]);
			_result.y = static_cast<float>(_uv->GetDirectArray().GetAt(_vertexCounter).mData[1]);
			break;

		case FbxGeometryElement::eIndexToDirect:
			_index = _uv->GetIndexArray().GetAt(_vertexCounter);	//index�� ����

			_result.x = static_cast<float>(_uv->GetDirectArray().GetAt(_index).mData[0]);
			_result.y = static_cast<float>(_uv->GetDirectArray().GetAt(_index).mData[1]);
			break;
		default:
			std::cout << "Error : Invalid vertex reference mode !" << std::endl;
		}
		break;
	}

	return _result;
}

void ResourceMgr::IndextVertex(const XMFLOAT3& _pos, const XMFLOAT3& _normal, const XMFLOAT2& _uv, const XMFLOAT3& _binormal, const XMFLOAT3& _tangent)
{
	VTXBONE _vb ( _pos, _normal, _uv, _binormal, _tangent );
	auto lookup = m_mapIndexMapping.find(_vb);

	if (lookup != m_mapIndexMapping.end())
		m_vecIndices.push_back(lookup->second);
	else
	{
		UINT _index = m_vecVtxbone.size();
		m_mapIndexMapping[_vb] = _index;
		m_vecIndices.push_back(_index);
		m_vecVtxbone.push_back(_vb);
	}
}

int ResourceMgr::HierarchyLoad(std::vector<Node*>* destHierarchy)
{
	const int anumStackCnt = m_pFbxScene->GetSrcObjectCount<FbxAnimStack>();
	m_iFrameCount = 0;

	if (anumStackCnt <= 0)
		return 0;

	//Animation stack �ϳ��� ���
	FbxAnimStack* animStack = m_pFbxScene->GetSrcObject<FbxAnimStack>(0);
	LoadAnimStack(animStack, m_pFbxScene->GetRootNode(), destHierarchy);

	return m_iFrameCount;
}

void ResourceMgr::LoadAnimStack(FbxAnimStack* _animStack, FbxNode* _root, std::vector<Node*>* destHierarchy)
{
	// Layer (Layer�� �ϳ��θ� ���)
	FbxAnimLayer* animLayer = _animStack->GetMember<FbxAnimLayer>(0);

	RecursiveNode(_root, animLayer, 0, -1, destHierarchy, 0);
}

void ResourceMgr::RecursiveNode(FbxNode* _root, FbxAnimLayer* _animLayer, const int _index, const int _parentIndex, std::vector<Node*>* destHierarchy, const int _depth)
{
	const FbxNodeAttribute* attr = _root->GetNodeAttribute();

	//Node�� �߰����� ������ �ε��� ī��Ʈ�� ���� �ʰ�, �θ� �ε����� �״�� �ѱ�
	int nodeIndex = _parentIndex;

	if (attr != nullptr)
	{
		const FbxNodeAttribute::EType attrType = attr->GetAttributeType();

		if (attrType == FbxNodeAttribute::eSkeleton)
		{
			std::shared_ptr<Node> destNode = std::make_shared<Node>();
			destNode->Index = nodeIndex = _index;
			destNode->Name = new std::string(_root->GetName());
			destNode->ParentIndex = _parentIndex;

			destNode->Type = NodeType::eSkeletonNode;

			destNode->LclTranslate	= XMFLOAT3(_root->LclTranslation.Get().mData[0], _root->LclTranslation.Get().mData[1], _root->LclTranslation.Get().mData[2]);
			destNode->LclRotation	= XMFLOAT3(_root->LclRotation.Get().mData[0], _root->LclRotation.Get().mData[1], _root->LclRotation.Get().mData[2]);
			destNode->LclScaling	= XMFLOAT3(_root->LclScaling.Get().mData[0], _root->LclScaling.Get().mData[1], _root->LclScaling.Get().mData[2]);

			//Curve
			FbxAnimCurve* tempCurveX = nullptr;
			FbxAnimCurve* tempCurveY = nullptr;
			FbxAnimCurve* tempCurveZ = nullptr;

			//LclTranslate
			tempCurveX = _root->LclTranslation.GetCurve(_animLayer, FBXSDK_CURVENODE_COMPONENT_X);
			tempCurveY = _root->LclTranslation.GetCurve(_animLayer, FBXSDK_CURVENODE_COMPONENT_Y);
			tempCurveZ = _root->LclTranslation.GetCurve(_animLayer, FBXSDK_CURVENODE_COMPONENT_Z);

			GetCurveValues(tempCurveX, tempCurveY, tempCurveZ, destNode->LclTransFrames);

			//LclRotation
			tempCurveX = _root->LclRotation.GetCurve(_animLayer, FBXSDK_CURVENODE_COMPONENT_X);
			tempCurveY = _root->LclRotation.GetCurve(_animLayer, FBXSDK_CURVENODE_COMPONENT_Y);
			tempCurveZ = _root->LclRotation.GetCurve(_animLayer, FBXSDK_CURVENODE_COMPONENT_Z);

			GetCurveValues(tempCurveX, tempCurveY, tempCurveZ, destNode->LclRotFrames);

			//LclScaling
			tempCurveX = _root->LclScaling.GetCurve(_animLayer, FBXSDK_CURVENODE_COMPONENT_X);
			tempCurveY = _root->LclScaling.GetCurve(_animLayer, FBXSDK_CURVENODE_COMPONENT_Y);
			tempCurveZ = _root->LclScaling.GetCurve(_animLayer, FBXSDK_CURVENODE_COMPONENT_Z);

			GetCurveValues(tempCurveX, tempCurveY, tempCurveZ, destNode->LclScalingFrames);

			destHierarchy->push_back(destNode.get());
		}
	}

	//Recursive
	const int childCount = _root->GetChildCount();
	for (int _index = 0; _index < childCount; ++_index)
	{
		RecursiveNode(_root->GetChild(_index), _animLayer, destHierarchy->size(), nodeIndex, destHierarchy, _depth + 1);
	}
}

void ResourceMgr::GetCurveValues(FbxAnimCurve* _animCurveX, FbxAnimCurve* _animCurveY, FbxAnimCurve* _cnimCurveZ, std::map<int, XMFLOAT3>* _dic)
{
	if (_animCurveX != nullptr)
	{
		const int keyCount = _animCurveX->KeyGetCount();
		m_iFrameCount = (m_iFrameCount > keyCount) ? m_iFrameCount : keyCount;

		for (int i = 0; i < keyCount; ++i)
		{
			int temp, tempindex;

			const float keyValue = static_cast<float>(_animCurveX->KeyGetValue(i));
			FbxTime keyTime = _animCurveX->KeyGetTime(i);
			keyTime.GetTime(temp, temp, temp, tempindex, temp, temp);
			
			if (_dic->find(tempindex) != _dic->end())
				(*_dic)[tempindex].x = keyValue;
			else
				_dic->insert(std::map<int, XMFLOAT3>::value_type(tempindex, XMFLOAT3(keyValue, 0.0f, 0.0f)));
		}
	}

	if (_animCurveY != nullptr)
	{
		const int keyCount = _animCurveY->KeyGetCount();
		m_iFrameCount = (m_iFrameCount > keyCount) ? m_iFrameCount : keyCount;

		for (int i = 0; i < keyCount; ++i)
		{
			int temp, tempindex;

			const float keyValue = static_cast<float>(_animCurveY->KeyGetValue(i));
			FbxTime keyTime = _animCurveY->KeyGetTime(i);
			keyTime.GetTime(temp, temp, temp, tempindex, temp, temp);

			if (_dic->find(tempindex) != _dic->end())
				(*_dic)[tempindex].y = keyValue;
			else
				_dic->insert(std::map<int, XMFLOAT3>::value_type(tempindex, XMFLOAT3(0.0f, keyValue, 0.0f)));
		}
	}

	if (_cnimCurveZ != nullptr)
	{
		const int keyCount = _cnimCurveZ->KeyGetCount();
		m_iFrameCount = (m_iFrameCount > keyCount) ? m_iFrameCount : keyCount;

		for (int i = 0; i < keyCount; ++i)
		{
			int temp, tempindex;

			const float keyValue = static_cast<float>(_cnimCurveZ->KeyGetValue(i));
			FbxTime keyTime = _cnimCurveZ->KeyGetTime(i);
			keyTime.GetTime(temp, temp, temp, tempindex, temp, temp);

			if (_dic->find(tempindex) != _dic->end())
				(*_dic)[tempindex].z = keyValue;
			else
				_dic->insert(std::map<int, XMFLOAT3>::value_type(tempindex, XMFLOAT3(0.0f, 0.0f, keyValue)));
		}
	}
}

BOOL ResourceMgr::MeshLoad(std::vector<VTXBONE*>* _vertices, std::vector<Material*>* _materials, std::vector<Node*>* _nodeHierarchy)
{
	FbxNode* _root = m_pFbxScene->GetRootNode();
	for (int i = 0; i < _root->GetChildCount(); ++i)
		RecursiveNode(_root->GetChild(i), _vertices, _materials, _nodeHierarchy);

	return TRUE;
}

void ResourceMgr::RecursiveNode(FbxNode* _node, std::vector<VTXBONE*>* _vertices, std::vector<Material*>* _materials, std::vector<Node*>* _nodeHierarchy)
{
	const int attrCount = _node->GetNodeAttributeCount();

	for (int _index = 0; _index < attrCount; ++_index)
	{
		FbxNodeAttribute* attr = _node->GetNodeAttributeByIndex(_index);
		FbxNodeAttribute::EType _type = attr->GetAttributeType();

		if (_type != FbxNodeAttribute::eMesh)
			continue;

		FbxMesh* _mesh = FbxCast <FbxMesh>(attr);

		// Material
		FbxSurfaceMaterialPtr _surfaceMatptr = (FbxSurfaceMaterialPtr)_node->GetSrcObject<FbxSurfaceMaterial>(_index);
		ExtractMaterial(_surfaceMatptr, new string(_node->GetName()), _mesh->GetPolygonCount(), _materials);
		
		// Vertices
		ExtractVertex(_node, _mesh, _vertices, _nodeHierarchy);
	}

	for (int i = 0; i < _node->GetChildCount(); ++i)
		RecursiveNode(_node->GetChild(i), _vertices, _materials, _nodeHierarchy);
}

void ResourceMgr::ExtractVertex(FbxNode* _node, FbxMesh* _mesh, std::vector<VTXBONE*>* _vertices, std::vector<Node*>* _nodeHierarchy)
{
	// Control Point
	std::vector<ControlPoint*>* _cps = new std::vector<ControlPoint*>();
	ExtractControlPoints(_mesh, _cps);
	ExtractWeight(_mesh, _node, _cps, _nodeHierarchy);

	// Indexing, Normal, UV
	int _vertexCounter = 0;
	const int _triCount = _mesh->GetPolygonCount();
	for (int _triIndex = 0; _triIndex < _triCount; ++_triIndex)
	{
		// �ﰢ�� ����
		for (int i = 0; i < 3; ++i)
		{
			// Control Point�� �ε���
			const int cpIndex = _mesh->GetPolygonVertex(_triIndex, i);
			
			VTXBONE* v = new VTXBONE();
			v->vPos = (*_cps)[cpIndex]->vPos;
			v->vNormal = ExtractNormal(_mesh, cpIndex, _vertexCounter);
			v->vTexUV = ExtractUV(_mesh, cpIndex, _vertexCounter);

			for (int _wIndex = 0; _wIndex < (*_cps)[cpIndex]->Weights->size(); ++_wIndex)
			{
				v->Weights->push_back((*(*_cps)[cpIndex]->Weights)[_wIndex]);
			}

			_vertices->push_back(v);

			++_vertexCounter;
		}
	}
}

void ResourceMgr::ExtractControlPoints(FbxMesh* _mesh, std::vector<ControlPoint*>* _cps)
{
	const int _cpCount = _mesh->GetControlPointsCount();
	for (int _cpi = 0; _cpi < _cpCount; ++_cpi)
	{
		ControlPoint* _cp = new ControlPoint();

		XMFLOAT3 _pos
		(static_cast<float>(_mesh->GetControlPointAt(_cpi).mData[0]), 
		static_cast<float>(_mesh->GetControlPointAt(_cpi).mData[1]), 
		static_cast<float>(_mesh->GetControlPointAt(_cpi).mData[2]));

		_cp->vPos = _pos;

		_cps->push_back(_cp);
	}
}

void ResourceMgr::ExtractWeight(FbxMesh* _mesh, FbxNode* _node, std::vector<ControlPoint*>* _cps, std::vector<Node*>* _nodeHierarchy)
{
	const int _skinCount = _mesh->GetDeformerCount(FbxDeformer::eSkin);

	for (int _skinIndex = 0; _skinIndex < _skinCount; ++_skinIndex)
	{
		FbxSkin* _skin = (FbxSkin*)(_mesh->GetDeformer(_skinIndex, FbxDeformer::eSkin));
		
		const int _jointCount = _skin->GetClusterCount();

		for (int _jointIndex = 0; _jointIndex < _jointCount; ++_jointIndex)
		{
			FbxCluster* _cluster = _skin->GetCluster(_jointIndex);

			string* _jointName = new string(_cluster->GetLink()->GetName());
			const int _jointNum = FindNodeIndexUsingName(_jointName, _nodeHierarchy);
			if (_jointNum == -1)
				continue;

			FbxAMatrix _matInvers = ExtractBoneGlobalToLocalMatrix(_cluster, _node);
			(*_nodeHierarchy)[_jointNum]->GlobalBindposeInverse = Math_Helper::FbxMatrix_to_XMMATRIX(_matInvers);

			const int _cpIndicesCount = _cluster->GetControlPointIndicesCount();
			for (int _cpIndicesIndex = 0; _cpIndicesIndex < _cpIndicesCount; ++_cpIndicesIndex)
			{
				BlendingIndexWeightPair* _currBlendingIndexWeightPair = new BlendingIndexWeightPair();
				_currBlendingIndexWeightPair->BlendingIndex = _jointNum;
				_currBlendingIndexWeightPair->BlendingWeight = _cluster->GetControlPointWeights()[_cpIndicesIndex];

				if (_currBlendingIndexWeightPair->BlendingWeight == 0)
				{
					delete _currBlendingIndexWeightPair;
					continue;
				}

				const int _cpIndex = _cluster->GetControlPointIndices()[_cpIndicesIndex];

				(*_cps)[_cpIndex]->Weights->push_back(_currBlendingIndexWeightPair);
			}
		}
	}
}

XMFLOAT3 ResourceMgr::ExtractNormal(const FbxMesh* _mesh, const int _cpIndex, const int _vertexCounter)
{
	if (_mesh->GetElementNormalCount() < 1)
		return XMFLOAT3();	//normal ����

	//normal ȹ��
	const FbxGeometryElementNormal* _vertexNormal = _mesh->GetElementNormal(0);

	XMFLOAT3 _normal;
	int _index = 0;
	switch (_vertexNormal->GetMappingMode())
	{
		//control point mapping
		//�� ������(����)���� �ϳ��� ���� ��ǥ�� ������ �ǹ�.
		//�븻 ������ �迭, �������� �迭�� �־����� �븻 ������ �迭�� �������� �迭�� �����Ǵ� ����� �� �� ����.
		//GetReferenceMode() ��� �Լ��� �����Ǵ� ����� �� �� ����.
	case FbxGeometryElement::eByControlPoint:
		switch (_vertexNormal->GetReferenceMode())
		{
			//n��° ��ҿ� ���� ���� ������ FbxLayerElementTemplate::mDirectArray���� n��° ��ġ���� �߰ߵǾ����� ��Ÿ��.
		case FbxGeometryElement::eDirect:
			_normal.x = static_cast<float>(_vertexNormal->GetDirectArray().GetAt(_cpIndex).mData[0]);
			_normal.y = static_cast<float>(_vertexNormal->GetDirectArray().GetAt(_cpIndex).mData[1]);
			_normal.z = static_cast<float>(_vertexNormal->GetDirectArray().GetAt(_cpIndex).mData[2]);
			break;

			//FbxLayerElementTemplate::mIndexArray�� �� ��ҿ� FbxLayerElementTemplate::mDirectArray�� ��Ҹ� ��Ÿ���� �ε����� ������ ��Ÿ��.
		case FbxGeometryElement::eIndexToDirect:
			_index = _vertexNormal->GetIndexArray().GetAt(_cpIndex);	//index�� ����

			_normal.x = static_cast<float>(_vertexNormal->GetDirectArray().GetAt(_index).mData[0]);
			_normal.y = static_cast<float>(_vertexNormal->GetDirectArray().GetAt(_index).mData[1]);
			_normal.z = static_cast<float>(_vertexNormal->GetDirectArray().GetAt(_index).mData[2]);
			break;
		default:
			std::cout << "Error : Invalid vertex reference mode !" << std::endl;
		}
		break;

		//�� ������(����)���� �ϳ��� ���� ��ǥ�� ������ �ǹ�.
	case FbxGeometryElement::eByPolygonVertex:	//polygon vertex mapping
		switch (_vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			_normal.x = static_cast<float>(_vertexNormal->GetDirectArray().GetAt(_cpIndex).mData[0]);
			_normal.y = static_cast<float>(_vertexNormal->GetDirectArray().GetAt(_cpIndex).mData[1]);
			_normal.z = static_cast<float>(_vertexNormal->GetDirectArray().GetAt(_cpIndex).mData[2]);
			break;

		case FbxGeometryElement::eIndexToDirect:
			_index = _vertexNormal->GetIndexArray().GetAt(_cpIndex);	//index�� ����

			_normal.x = static_cast<float>(_vertexNormal->GetDirectArray().GetAt(_index).mData[0]);
			_normal.y = static_cast<float>(_vertexNormal->GetDirectArray().GetAt(_index).mData[1]);
			_normal.z = static_cast<float>(_vertexNormal->GetDirectArray().GetAt(_index).mData[2]);
			break;
		default:
			std::cout << "Error : Invalid vertex reference mode !" << std::endl;
		}
		break;
	}

	return _normal;
}

XMFLOAT2 ResourceMgr::ExtractUV(const FbxMesh* _mesh, const int _cpIndex, const int _vertexCounter)
{
	if (_mesh->GetElementUVCount() < 1)
		return XMFLOAT2();	//UV ����

	const FbxGeometryElementUV* _vertexUV = _mesh->GetElementUV(0);
	XMFLOAT2 _uv;
	int _index = 0;

	switch (_vertexUV->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (_vertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			_uv.x = static_cast<float>(_vertexUV->GetDirectArray().GetAt(_cpIndex).mData[0]);
			_uv.y = static_cast<float>(_vertexUV->GetDirectArray().GetAt(_cpIndex).mData[1]);
			break;

		case FbxGeometryElement::eIndexToDirect:
			_index = _vertexUV->GetIndexArray().GetAt(_cpIndex);	//index�� ����

			_uv.x = static_cast<float>(_vertexUV->GetDirectArray().GetAt(_index).mData[0]);
			_uv.y = static_cast<float>(_vertexUV->GetDirectArray().GetAt(_index).mData[1]);
			break;
		default:
			std::cout << "Error : Invalid vertex reference mode !" << std::endl;
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch (_vertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			_uv.x = static_cast<float>(_vertexUV->GetDirectArray().GetAt(_cpIndex).mData[0]);
			_uv.y = static_cast<float>(_vertexUV->GetDirectArray().GetAt(_cpIndex).mData[1]);
			break;

		case FbxGeometryElement::eIndexToDirect:
			_index = _vertexUV->GetIndexArray().GetAt(_cpIndex);	//index�� ����

			_uv.x = static_cast<float>(_vertexUV->GetDirectArray().GetAt(_index).mData[0]);
			_uv.y = static_cast<float>(_vertexUV->GetDirectArray().GetAt(_index).mData[1]);
			break;
		default:
			std::cout << "Error : Invalid vertex reference mode !" << std::endl;
		}
		break;
	}

	return _uv;
}

FbxAMatrix ResourceMgr::ExtractBoneGlobalToLocalMatrix(FbxCluster* _cluster, FbxNode* _node)
{
	FbxAMatrix transformMatrix;
	FbxAMatrix transformLinkMatrix;

	_cluster->GetTransformMatrix(transformMatrix);	// The transformation of the mesh at binding time.
	// ���� ��� ä�ο� "freeze transformation"�� ������ ��, ��������� ����

	_cluster->GetTransformLinkMatrix(transformLinkMatrix);	// The transformation of the cluster(joint) at binding time from joint space to world space
	// ������ Joint space ���� World space���� ��ȯ ���

	// Global -> Local ��ȯ ���
	FbxAMatrix globalBindPoseInverseMatrix = transformLinkMatrix.Inverse() * transformMatrix * GetGeometryTransform(_node);

	return globalBindPoseInverseMatrix;
}

FbxAMatrix ResourceMgr::GetGeometryTransform(FbxNode* _node)
{
	const FbxVector4 lT = _node->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 lR = _node->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 lS = _node->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxAMatrix(lT, lR, lS);
}

void ResourceMgr::ExtractMaterial(FbxSurfaceMaterialPtr _surfaceMatPtr, string* _nodeName, const int _polygonCount, std::vector<Material*>* _materials)
{
	if (_surfaceMatPtr != nullptr)
	{
		FbxProperty _prop = _surfaceMatPtr->FindProperty(FbxSurfaceMaterial::sDiffuse);

		Material* _subset = new Material();
		_subset->TriangleCount = _polygonCount;
		_subset->NodeName = _nodeName;
		_subset->MaterialName = new string(_surfaceMatPtr->GetName());
		_subset->TextureName = ExtractTextureName(_prop);

		_materials->push_back(_subset);
	}
	else
	{
		Material* _subset = new Material();
		_subset->TriangleCount = _polygonCount;
		_subset->NodeName = _nodeName;
		(*_subset->MaterialName) = "No Material";
		(*_subset->TextureName) = "No Texture";
		
		_materials->push_back(_subset);
	}
}

string* ResourceMgr::ExtractTextureName(FbxProperty _prop)
{
	const int _layeredTextureCount = _prop.GetSrcObjectCount<FbxLayeredTexture>();

	string* _filename = new string("");
	
	if (_layeredTextureCount > 0)
	{
		//layered texture �� �ʾ�
	}
	else
	{
		//Directly get textures
		int _texCount = _prop.GetSrcObjectCount<FbxTexture>();

		if (_texCount == 1)
		{
			FbxTexture* _texture = FbxCast<FbxTexture>(_prop.GetSrcObject <FbxTexture>(0));

			FbxFileTexture* _textureFile = FbxCast<FbxFileTexture>(_prop.GetSrcObject<FbxFileTexture>(0));
			_filename = new string(_textureFile->GetRelativeFileName());
		}
	}
	return _filename;
}

int ResourceMgr::FindNodeIndexUsingName(string* _name, std::vector<Node*>* _nodeHierarchy)
{
	int _index = -1;
	for (int i = 0; i < _nodeHierarchy->size(); ++i)
	{
		if ((*_nodeHierarchy)[i]->Name == _name)
		{
			_index = (*_nodeHierarchy)[i]->Index;
			break;
		}
	}

	return _index;
}

