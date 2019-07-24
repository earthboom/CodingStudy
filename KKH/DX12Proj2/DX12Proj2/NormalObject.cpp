#include "pch.h"
#include "NormalObject.h"
#include "Utility_Manager.h"

NormalObject::NormalObject(void)
	: Object()
	, m_eShapeType(ST_END)
{
}

NormalObject::NormalObject(Object::COM_TYPE _type, std::string _name, std::string _submeshname, std::string _texname, std::string _matname, ShapeType eType)
	: Object(_type, _name, _submeshname, _texname, _matname)
	, m_eShapeType(eType)
{
}

NormalObject::~NormalObject(void)
{
}

bool NormalObject::Ready(void)
{
	BuildDescriptorHeaps();
	BuildGeometry();
	BuildMaterials();
	BuildRenderItem();

	return TRUE;
}

bool NormalObject::Update(const CTimer& mt)
{
	return TRUE;
}

bool NormalObject::Render(const CTimer& mt)
{
	return TRUE;
}

bool NormalObject::BuildDescriptorHeaps(void)
{
	bool bTexLoad = Object::BuildDescriptorHeaps();
	if (!bTexLoad)
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(UTIL.Get_SrvDiscriptorHeap()->GetCPUDescriptorHandleForHeapStart(), g_MatCBcount, UTIL.Get_CbvSrvDescriptorSize());

		auto defaultTex = TEX.Get_Textures()[m_texName]->Resource;
		TEX.Get_Textures()[m_texName]->matCount = g_MatCBcount++;

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = defaultTex->GetDesc().Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = defaultTex->GetDesc().MipLevels;
		DEVICE->CreateShaderResourceView(defaultTex.Get(), &srvDesc, hDescriptor);
	}

	return TRUE;
}

void NormalObject::BuildMaterials(void)
{
	auto pMat = std::make_unique<Material>();
	pMat->Name = m_matName;
	pMat->MatCBIndex = TEX.Get_Textures()[m_texName]->matCount;
	pMat->DiffuseSrvHeapIndex = TEX.Get_Textures()[m_texName]->matCount;
	pMat->DiffuseAlbedo = DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	pMat->FresnelR0 = DirectX::XMFLOAT3(0.02f, 0.02f, 0.02f);
	pMat->Roughness = 0.1f;

	UTIL.Get_Materials()[m_matName] = std::move(pMat);
}

void NormalObject::BuildRenderItem(void)
{
	auto ritem = std::make_unique<RenderItem>();

	ritem->objCBIndex = g_ObjCBcount++;
	ritem->Mat = UTIL.Get_Materials()[m_matName].get();
	ritem->Geo = UTIL.Get_Geomesh()[m_Name].get();
	ritem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	ritem->InstanceCount = 0;
	ritem->IndexCount = ritem->Geo->DrawArgs[m_submeshName].IndexCount;
	ritem->StartIndexLocation = ritem->Geo->DrawArgs[m_submeshName].StartIndexLocation;
	ritem->BaseVertexLocation = ritem->Geo->DrawArgs[m_submeshName].BaseVertexLocation;
	ritem->Bounds = ritem->Geo->DrawArgs[m_submeshName].Bounds;

	ritem->Instances.resize(1);
	ritem->Instances[0].World = MathHelper::Identity4x4();
	ritem->Instances[0].MaterialIndex = 0;
	XMStoreFloat4x4(&ritem->Instances[0].TexTransform, XMMatrixScaling(2.0f, 2.0f, 1.0f));
}

void NormalObject::BuildGeometry(void)
{
	switch (m_eShapeType)
	{
	case ST_BOX:
		break;

	case ST_GRID:
		break;

	case ST_SHPERE:
		break;

	case ST_CYLINEDER:
		break;

	default:
		return;
	}
}

std::shared_ptr<NormalObject> NormalObject::Create(Object::COM_TYPE _type, std::string _name, std::string _submeshname, std::string _texname, std::string _matname, ShapeType eType)
{
	NORMALOBJECT pNormalObject = std::make_unique<NormalObject>(_type, _name, _submeshname, _texname, _matname, eType);

	if (!pNormalObject)
		return nullptr;

	return pNormalObject;
}
