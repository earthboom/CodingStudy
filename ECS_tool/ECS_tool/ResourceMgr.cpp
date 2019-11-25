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

		//FbxIOSettings Ŭ������ ���� ��Ҹ� ���Ͽ��� �������ų� ���Ϸ� �������� ���θ� ����.
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

	//Scene�� ��Ʈ ��带 �����´�.
	FbxNode* rootNode = scene->GetRootNode();

	//Scene�� �� �ý���, �ֺ� ���� �� �ð� ������ FbxGlobalSettings ��ü�� ����. 
	//�� ��ü�� FbxScene::GlobalSettings()�� ���� ����
	FbxAxisSystem sceneAxisSystem = scene->GetGlobalSettings().GetAxisSystem();

	//Scene ���� ��ǥ���� �ٲ�.
	FbxAxisSystem::MayaYUp.ConvertScene(scene);

	//Scene ������ �ﰢ��ȭ �� �� �ִ� ��� ��带 �ﰢ��ȭ ��Ų��.
	FbxGeometryConverter geometryConverter(m_pFbxmgr);
	geometryConverter.Triangulate(scene, TRUE);

	LoadNode(rootNode);

	FbxMesh* mesh = FbxMesh::Create(scene, "");

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
		if (_nodeAtt->GetAttributeType() == FbxNodeAttribute::eMesh)
		{

		}

	}

	const UINT _cnt = _node->GetChildCount();
	for (UINT i = 0; i < _cnt; ++i)
		LoadNode(_node->GetChild(i));
}

