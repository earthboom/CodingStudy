#include "stdafx.h"
#include "Mirror.h"
#include "GeometryGenerator.h"
#include "FrameResource.h"
#include "Utility_Manager.h"
#include "Texture_Manger.h"

Mirror::Mirror(void)
	: Object()
{
}

Mirror::Mirror(Object::COM_TYPE _type, std::string _name, std::string _submeshname, std::string _texname, std::string _matname)
	: Object(_type, _name, _submeshname, _texname, _matname)
{
}

Mirror::~Mirror(void)
{
}

bool Mirror::Ready(void)
{
	BuildDescriptorHeaps();
	BuildGeometry();
	BuildMaterials();
	BuildRenderItem();

	return TRUE;
}

bool Mirror::Update(const CTimer& mt)
{
	return TRUE;
}

bool Mirror::Render(const CTimer& mt)
{
	return TRUE;
}

void Mirror::BuildDescriptorHeaps(void)
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

void Mirror::BuildMaterials(void)
{
	auto mat = std::make_unique<Material>();
	mat->Name = m_matName;
	mat->MatCBIndex = g_MatCBcount;
	mat->DiffuseSrvHeapIndex = g_MatCBcount;
	mat->DiffuseAlbedo = m_Material.DiffuseAlbedo;
	mat->FresnelR0 = m_Material.FresnelR0;
	mat->Roughness = m_Material.Roughness;

	UTIL.Get_Materials()[m_matName] = std::move(mat);

	++g_MatCBcount;
}

void Mirror::BuildRenderItem(void)
{
	auto ritem = std::make_unique<RenderItem>();
	ritem->World = MathHelper::Identity4x4();
	ritem->TexTransform = MathHelper::Identity4x4();
	ritem->objCBIndex = g_ObjCBcount;
	ritem->Mat = UTIL.Get_Materials()[m_matName].get();
	ritem->Geo = UTIL.Get_Geomesh()[m_Name].get();
	ritem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	ritem->IndexCount = ritem->Geo->DrawArgs[m_submeshName].IndexCount;
	ritem->StartIndexLocation = ritem->Geo->DrawArgs[m_submeshName].StartIndexLocation;
	ritem->BaseVertexLocation = ritem->Geo->DrawArgs[m_submeshName].BaseVertexLocation;
	
	UTIL.Get_Drawlayer((int)DrawLayer::DL_MIRROR).push_back(ritem.get());
	UTIL.Get_Drawlayer((int)DrawLayer::DL_TRANSPARENT).push_back(ritem.get());

	UTIL.Get_Ritemvec().push_back(std::move(ritem));

	++g_ObjCBcount;
}

void Mirror::BuildGeometry(void)
{
	std::array<std::uint16_t, 30> indices =
	{
		0, 1, 2,
		0, 2, 3,
	};

	SubmeshGeometry submesh;
	submesh.IndexCount = 6;
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	const UINT vbByteSize = (UINT)surface_vt.size() * sizeof(VERTEX);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(uint16_t);

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = m_Name;

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), surface_vt.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = D3DUTIL.CreateDefaultBuffer(GRAPHIC_DEV.Get(),
		COM_LIST.Get(), surface_vt.data(), vbByteSize, geo->VertexBufferUploader);

	geo->IndexBufferGPU = D3DUTIL.CreateDefaultBuffer(GRAPHIC_DEV.Get(),
		COM_LIST.Get(), indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(VERTEX);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	geo->DrawArgs[m_submeshName] = submesh;

	UTIL.Get_Geomesh()[geo->Name] = std::move(geo);
}

std::shared_ptr<Mirror> Mirror::Create(Object::COM_TYPE _type, std::string _name, std::string _submeshname, std::string _texname, std::string _matname)
{
	MIRROR pMirror = std::make_shared<Mirror>(_type, _name, _submeshname, _texname, _matname);

	if (!pMirror) return nullptr;

	return pMirror;
}
