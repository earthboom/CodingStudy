#include "pch.h"
#include "ResourceMgr.h"

ResourceMgr::ResourceMgr(void)
	: m_pFbxmgr(nullptr)
{
}

ResourceMgr::~ResourceMgr(void)
{
	if(m_pFbxmgr)
		m_pFbxmgr->Destroy();
}

void ResourceMgr::Ready_FbxManager(void)
{
	if (m_pFbxmgr == nullptr)
	{
		m_pFbxmgr = FbxManager::Create();

		//FbxIOSettings 클래스는 씬의 요소를 파일에서 가져오거나 파일로 내보낼지 여부를 지정.
		FbxIOSettings* ios = FbxIOSettings::Create(m_pFbxmgr, IOSROOT);
		m_pFbxmgr->SetIOSettings(ios);
	}
}

BOOL ResourceMgr::Ready_Mesh(void)
{	
	FbxImporter* importer = FbxImporter::Create(m_pFbxmgr, "");
	FbxScene* scene = FbxScene::Create(m_pFbxmgr, "");
	
	if (!importer->Initialize("", EOF, m_pFbxmgr->GetIOSettings()))
	{
		std::cout << "Call to importer->Initialize Failed!!" << std::endl;
		std::cout << "Error : " << importer->GetStatus().GetErrorString() << std::endl;
		exit(-1);
	}
	
	if (!importer->Import(scene))
	{
		std::cout << "Call to importer->Import Failed!!" << std::endl;
		std::cout << "Error : " << importer->GetStatus().GetErrorString() << std::endl;
		exit(-1);
	}

	importer->Destroy();


	
	FbxNode* node = FbxNode::Create(scene, "");

	//Scene의 루트 노드를 가져온다.
	FbxNode* rootNode = scene->GetRootNode();

	//Scene의 축 시스템, 주변 조명 및 시간 설정은 FbxGlobalSettings 객체에 정의. 
	//이 객체는 FbxScene::GlobalSettings()를 통해 접근
	FbxAxisSystem sceneAxisSystem = scene->GetGlobalSettings().GetAxisSystem();

	//Scene 내의 좌표축을 바꿈.
	FbxAxisSystem::MayaYUp.ConvertScene(scene);

	//Scene 내에서 삼각형화 할 수 있는 모든 노드를 삼각형화 시킨다.
	FbxGeometryConverter geometryConverter(m_pFbxmgr);
	geometryConverter.Triangulate(scene, TRUE);

	LoadNode(rootNode);

	FbxMesh* mesh = FbxMesh::Create(scene, "");

	FbxCameraManipulator* cameraManipulator = FbxCameraManipulator::Create(m_pFbxmgr, "");
	

	return TRUE;
}

void ResourceMgr::LoadNode(FbxNode* _node)
{
	//Scene의 요소(mesh, light, camera 등)는 FbxNode와 FbxNodeAttribute의 하위 클래스와 결합하여 정의.
	//FbxNodeAttribute는 Scene에서의 위치를 설명하기 위해 FbxNode에 바인딩 됨.
	//이 바인딩 속성은FbxNode::GetNodeAttribute() 멤버 함수를 통해 접근 가능
	FbxNodeAttribute* _nodeAtt = _node->GetNodeAttribute();

	if (_nodeAtt)
	{
		if (_nodeAtt->GetAttributeType() == FbxNodeAttribute::eMesh)
		{

		}

	}

	const UINT _cnt = _node->GetChildCount();
	for (UINT i = 0; i < _cnt; ++i)
		LoadNode(_node->GetChild(i));
}

