#include "stdafx.h"
#include "Skull.h"
#include "Utility_Manager.h"
#include "Texture_Manger.h"

Skull::Skull(void)
	: Object()
	, m_Skull(nullptr)
	, m_ReflectionSkull(nullptr)
	, mSkullTranslation(0.0f, 1.0f, -5.0f)
{
}

Skull::Skull(Object::COM_TYPE _type, std::string _name, std::string _submeshname, std::string _texname, std::string _matname)
	: Object(_type, _name, _submeshname, _texname, _matname)
	, m_Skull(nullptr)
	, m_ReflectionSkull(nullptr)
	, mSkullTranslation(0.0f, 1.0f, -5.0f)
{
}

Skull::~Skull(void)
{
}

bool Skull::Ready(void)
{
	BuildDescriptorHeaps();
	BuildGeometry();
	BuildMaterials();
	BuildRenderItem();

	return TRUE;
}

bool Skull::Update(const CTimer& mt)
{
	const float dt = mt.DeltaTime();

	if (GetAsyncKeyState('A') & 0x8000)
		mSkullTranslation.x -= 1.5f * dt;

	if (GetAsyncKeyState('D') & 0x8000)
		mSkullTranslation.x += 1.5f * dt;

	if (GetAsyncKeyState('W') & 0x8000)
		mSkullTranslation.y += 1.5f * dt;

	if (GetAsyncKeyState('S') & 0x8000)
		mSkullTranslation.y -= 1.5f * dt;

	mSkullTranslation.y = MathHelper::Max(mSkullTranslation.y, 0.0f);

	DirectX::XMMATRIX _rotate = DirectX::XMMatrixRotationY(0.5f * PI);
	DirectX::XMMATRIX _scale = DirectX::XMMatrixScaling(0.45f, 0.45f, 0.45f);
	DirectX::XMMATRIX _offset = DirectX::XMMatrixTranslation(mSkullTranslation.x, mSkullTranslation.y, mSkullTranslation.z);
	DirectX::XMMATRIX _world = _rotate * _scale * _offset;
	DirectX::XMStoreFloat4x4(&m_Skull->World, _world);

	DirectX::XMVECTOR _mirrorplane = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	DirectX::XMMATRIX _r = DirectX::XMMatrixReflect(_mirrorplane);
	DirectX::XMStoreFloat4x4(&m_ReflectionSkull->World, _world * _r);

	m_Skull->NumFramesDirty = NumFrameResources;
	m_ReflectionSkull->NumFramesDirty = NumFrameResources;

	return TRUE;
}

bool Skull::Render(const CTimer& mt)
{
	return TRUE;
}

void Skull::BuildDescriptorHeaps(void)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(UTIL.Get_SrvDiscriptorHeap()->GetCPUDescriptorHandleForHeapStart());

	hDescriptor.Offset(g_MatCBcount, UTIL.Get_CbvSrvDescriptorSize());

	auto tex = TEX.Get_Textures()[m_texName]->Resource;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = tex->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;
	GRAPHIC_DEV->CreateShaderResourceView(tex.Get(), &srvDesc, hDescriptor);
}

void Skull::BuildMaterials(void)
{
	auto mat = std::make_unique<Material>();
	mat->Name = m_matName;
	mat->MatCBIndex = g_MatCBcount;
	mat->DiffuseSrvHeapIndex = g_MatCBcount;
	mat->DiffuseAlbedo = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mat->FresnelR0 = DirectX::XMFLOAT3(0.05f, 0.05f, 0.05f);
	mat->Roughness = 0.3f;

	UTIL.Get_Materials()[m_matName] = std::move(mat);

	++g_MatCBcount;
}

void Skull::BuildRenderItem(void)
{
	auto ritem = std::make_unique<RenderItem>();
	ritem->World = MathHelper::Identity4x4();
	ritem->TexTransform = MathHelper::Identity4x4();
	ritem->objCBIndex = g_ObjCBcount++;
	ritem->Mat = UTIL.Get_Materials()[m_matName].get();
	ritem->Geo = UTIL.Get_Geomesh()[m_Name].get();
	ritem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	ritem->IndexCount = ritem->Geo->DrawArgs[m_submeshName].IndexCount;
	ritem->StartIndexLocation = ritem->Geo->DrawArgs[m_submeshName].StartIndexLocation;
	ritem->BaseVertexLocation = ritem->Geo->DrawArgs[m_submeshName].BaseVertexLocation;
	m_Skull = ritem.get();
	UTIL.Get_Drawlayer((int)DrawLayer::DL_OPAUQE).push_back(ritem.get());

	auto reflectionRitem = std::make_unique<RenderItem>();
	*reflectionRitem = *ritem;
	reflectionRitem->objCBIndex = g_ObjCBcount++;
	m_ReflectionSkull = reflectionRitem.get();
	UTIL.Get_Drawlayer((int)DrawLayer::DL_REFLECTED).push_back(reflectionRitem.get());

	UTIL.Get_Ritemvec().push_back(std::move(ritem));
	UTIL.Get_Ritemvec().push_back(std::move(reflectionRitem));
}

void Skull::BuildGeometry(void)
{
	std::ifstream fin("../Models/skull.txt");

	if (!fin)
	{
		MessageBox(0, L"../Models/skull.txt not found.", 0, 0);
		return;
	}

	UINT vcount = 0, tcount = 0;
	std::string ignore;

	fin >> ignore >> vcount;
	fin >> ignore >> tcount;
	fin >> ignore >> ignore >> ignore >> ignore;

	std::vector<VERTEX> vertices(vcount);
	for (UINT i = 0; i < vcount; ++i)
	{
		fin >> vertices[i].Pos.x >> vertices[i].Pos.y >> vertices[i].Pos.z;
		fin >> vertices[i].Normal.x >> vertices[i].Normal.y >> vertices[i].Normal.z;
	}

	fin >> ignore;
	fin >> ignore;
	fin >> ignore;

	std::vector<std::int32_t> indices(3 * tcount);
	for (UINT i = 0; i < tcount; ++i)
	{
		fin >> indices[i * 3 + 0] >> indices[i * 3 + 1] >> indices[i * 3 + 2];
	}

	fin.close();

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(VERTEX);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::int32_t);

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = m_Name;

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = D3DUTIL.CreateDefaultBuffer(GRAPHIC_DEV.Get(), COM_LIST.Get(), vertices.data(), vbByteSize, geo->VertexBufferUploader);
	geo->IndexBufferGPU = D3DUTIL.CreateDefaultBuffer(GRAPHIC_DEV.Get(), COM_LIST.Get(), indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(VERTEX);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R32_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	geo->DrawArgs[m_submeshName] = submesh;

	UTIL.Get_Geomesh()[geo->Name] = std::move(geo);
}

std::shared_ptr<Skull> Skull::Create(Object::COM_TYPE _type, std::string _name, std::string _submeshname, std::string _texname, std::string _matname)
{
	SKULL pSkull = std::make_shared<Skull>(_type, _name, _submeshname, _texname, _matname);

	if (!pSkull) return nullptr;

	return pSkull;
}
