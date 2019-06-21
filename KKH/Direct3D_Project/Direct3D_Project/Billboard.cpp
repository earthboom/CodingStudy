#include "stdafx.h"
#include "Billboard.h"
#include "GeometryGenerator.h"
#include "FrameResource.h"
#include "Utility_Manager.h"
#include "Texture_Manger.h"

Billboard::Billboard(void)
	: Object()
{
}

Billboard::Billboard(Object::COM_TYPE _type, std::string _name, std::string _submeshname, std::string _texname, std::string _matname)
	: Object(_type, _name, _submeshname, _texname, _matname)
{
}

Billboard::~Billboard(void)
{
}

bool Billboard::Ready(void)
{
	BuildDescriptorHeaps();
	BuildGeometry();
	BuildMaterials();
	BuildRenderItem();

	return TRUE;
}

bool Billboard::Update(const CTimer& mt)
{
	return TRUE;
}

bool Billboard::Render(const CTimer& mt)
{
	return TRUE;
}

void Billboard::BuildDescriptorHeaps(void)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(UTIL.Get_SrvDiscriptorHeap()->GetCPUDescriptorHandleForHeapStart(), g_MatCBcount, UTIL.Get_CbvSrvDescriptorSize());

	auto tex = TEX.Get_Textures()[m_texName]->Resource;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = tex->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;
	//srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	GRAPHIC_DEV->CreateShaderResourceView(tex.Get(), &srvDesc, hDescriptor);
}

void Billboard::BuildMaterials(void)
{
	auto mat = std::make_unique<Material>();
	mat->Name = m_matName;
	mat->MatCBIndex = g_MatCBcount;
	mat->DiffuseSrvHeapIndex = g_MatCBcount;
	mat->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mat->FresnelR0 = XMFLOAT3(0.01f, 0.01f, 0.01f);
	mat->Roughness = 0.125f;

	UTIL.Get_Materials()[m_matName] = std::move(mat);

	++g_MatCBcount;
}

void Billboard::BuildRenderItem(void)
{
	auto ritem = std::make_unique<RenderItem>();
	ritem->World = MathHelper::Identity4x4();
	ritem->TexTransform = MathHelper::Identity4x4();
	ritem->objCBIndex = g_ObjCBcount;
	ritem->Mat = UTIL.Get_Materials()[m_matName].get();
	ritem->Geo = UTIL.Get_Geomesh()[m_Name].get();
	ritem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
	ritem->IndexCount = ritem->Geo->DrawArgs[m_submeshName].IndexCount;
	ritem->StartIndexLocation = ritem->Geo->DrawArgs[m_submeshName].StartIndexLocation;
	ritem->BaseVertexLocation = ritem->Geo->DrawArgs[m_submeshName].BaseVertexLocation;

	UTIL.Get_Drawlayer((int)DrawLayer::DL_ALPHATESTED_TREESPRITE).push_back(ritem.get());

	UTIL.Get_Ritemvec().push_back(std::move(ritem));

	++g_ObjCBcount;
}

void Billboard::BuildGeometry(void)
{
	struct TreeSpriteVertex
	{
		XMFLOAT3 Pos;
		XMFLOAT2 Size;
	};

	static const int treeCount = 16;
	std::array<TreeSpriteVertex, 16> vertices;

	for (UINT i = 0; i < treeCount; ++i)
	{
		float x = MathHelper::RandF(-45.0f, 45.0f);
		float z = MathHelper::RandF(-45.0f, 45.0f);
		float y = GetHillsHeight(x, z);

		y += 8.0f;

		vertices[i].Pos = XMFLOAT3(x, y, z);
		vertices[i].Size = XMFLOAT2(20.0f, 20.0f);
	}

	std::array<std::uint16_t, 16> indices =
	{
		0, 1, 2, 3, 4, 5, 6, 7,
		8, 9, 10, 11, 12, 13, 14, 15
	};

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(TreeSpriteVertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(uint16_t);

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = m_Name;

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = D3DUTIL.CreateDefaultBuffer(GRAPHIC_DEV.Get(),
		COM_LIST.Get(), vertices.data(), vbByteSize, geo->VertexBufferUploader);

	geo->IndexBufferGPU = D3DUTIL.CreateDefaultBuffer(GRAPHIC_DEV.Get(),
		COM_LIST.Get(), indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(TreeSpriteVertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	geo->DrawArgs[m_submeshName] = submesh;

	UTIL.Get_Geomesh()[geo->Name] = std::move(geo);
}

float Billboard::GetHillsHeight(float x, float z) const
{
	return 0.3f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));
}

std::shared_ptr<Billboard> Billboard::Create(Object::COM_TYPE _type, std::string _name, std::string _submeshname, std::string _texname, std::string _matname)
{
	BILLBOARD pBillboard = std::make_shared<Billboard>(_type, _name, _submeshname, _texname, _matname);

	if (!pBillboard) return nullptr;

	return pBillboard; 
}
