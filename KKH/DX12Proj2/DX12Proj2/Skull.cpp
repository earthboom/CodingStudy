#include "pch.h"
#include "Skull.h"
#include "Utility_Manager.h"
#include "Mouse_Manager.h"

Skull::Skull(void)
	: Object()
	, m_Skull(nullptr)
	, mSkullTranslation(0.0f, 1.0f, -5.0f)
{
}

Skull::Skull(Object::COM_TYPE _type, std::string _geoname, std::string _submeshname, std::string _texname, std::string _matname)
	: Object(_type, _geoname, _submeshname, _texname, _matname)
	, m_Skull(nullptr)
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

	return TRUE;
}

bool Skull::Render(const CTimer& mt)
{
	return TRUE;
}

bool Skull::BuildDescriptorHeaps(void)
{
	if (TEX.Get_Textures()[m_texName]->bRegister)
	{
		m_matCount = g_MatCBcount++;
		return FALSE;
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(UTIL.Get_SrvDiscriptorHeap()->GetCPUDescriptorHandleForHeapStart(), g_SrvHeapCount, UTIL.Get_CbvSrvDescriptorSize());

	auto defaultTex = TEX.Get_Textures()[m_texName]->Resource;
	m_matCount = g_MatCBcount++;
	TEX.Get_Textures()[m_texName]->srvHeapCount = g_SrvHeapCount++;
	TEX.Get_Textures()[m_texName]->bRegister = TRUE;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = defaultTex->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = defaultTex->GetDesc().MipLevels;
	DEVICE->CreateShaderResourceView(defaultTex.Get(), &srvDesc, hDescriptor);

	return TRUE;
}

void Skull::BuildMaterials(void)
{
	auto pMat = std::make_unique<Material>();
	pMat->Name = m_matName;
	pMat->MatCBIndex = m_matCount;
	pMat->DiffuseSrvHeapIndex = TEX.Get_Textures()[m_texName]->srvHeapCount;
	pMat->DiffuseAlbedo = DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	pMat->FresnelR0 = DirectX::XMFLOAT3(0.02f, 0.02f, 0.02f);
	pMat->Roughness = 0.2f;

	//auto highlight0 = std::make_unique<Material>();
	//highlight0->Name = "highlight0";
	//highlight0->MatCBIndex = g_MatCBcount++;
	//highlight0->DiffuseSrvHeapIndex = 0;
	//highlight0->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f);
	//highlight0->FresnelR0 = XMFLOAT3(0.06f, 0.06f, 0.06f);
	//highlight0->Roughness = 0.0f;

	UTIL.Get_Materials()[m_matName]	= std::move(pMat);

	//UTIL.Get_Materials()["highlight0"] = std::move(highlight0);
}

void Skull::BuildRenderItem(void)
{
	auto ritem = std::make_unique<RenderItem>();
	//auto _pieckedRitem = std::make_unique<RenderItem>();

	//ritem->World = MathHelper::Identity4x4();
	//ritem->TexTransform = MathHelper::Identity4x4();
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

	
	//Generate instance data
	//const int n = 5;
	//ritem->Instances.resize(n * n * n);

	//float width = 200.0f;
	//float height = 200.0f;
	//float depth = 200.0f;

	//float x = -0.5f * width;
	//float y = -0.5f * height;
	//float z = -0.5f * depth;

	//float dx = width / (n - 1);
	//float dy = height / (n - 1);
	//float dz = depth / (n - 1);

	//for (int k = 0; k < n; ++k)
	//{
	//	for (int i = 0; i < n; ++i)
	//	{
	//		for (int j = 0; j < n; ++j)
	//		{
	//			int index = k * n * n + i * n + j;

	//			ritem->Instances[index].World = XMFLOAT4X4(
	//				1.0f, 0.0f, 0.0f, 0.0f,
	//				0.0f, 1.0f, 0.0f, 0.0f,
	//				0.0f, 0.0f, 1.0f, 0.0f,
	//				x + j * dx, y + i * dy, z + k * dz, 1.0f);

	//			XMStoreFloat4x4(&ritem->Instances[index].TexTransform, XMMatrixScaling(2.0f, 2.0f, 1.0f));
	//			ritem->Instances[index].MaterialIndex = index % UTIL.Get_Materials().size();
	//		}
	//	}
	//}

	
	/*_pieckedRitem->World = MathHelper::Identity4x4();
	_pieckedRitem->TexTransform = MathHelper::Identity4x4();
	_pieckedRitem->objCBIndex = g_ObjCBcount;
	_pieckedRitem->Mat = UTIL.Get_Materials()["highlight0"].get();
	_pieckedRitem->Geo = UTIL.Get_Geomesh()[m_Name].get();
	_pieckedRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	_pieckedRitem->Visible = FALSE;
	_pieckedRitem->InstanceCount = 0;
	_pieckedRitem->IndexCount = 0;
	_pieckedRitem->StartIndexLocation = 0;
	_pieckedRitem->BaseVertexLocation = 0;

	MOUSE.GetPickedRitem().push_back(_pieckedRitem.get());*/

	
	//for (auto& e : UTIL.Get_Ritemvec())
	//{
	//	UTIL.Get_Drawlayer((int)DrawLayer::DL_OPAUQE).push_back(e.get());
	//}
	UTIL.Get_Drawlayer((int)DrawLayer::DL_OPAUQE).push_back(ritem.get());
	UTIL.Get_Ritemvec().push_back(std::move(ritem));
	//UTIL.Get_Drawlayer((int)DrawLayer::DL_HIGHLIGHT).push_back(_pieckedRitem.get());
	//UTIL.Get_Ritemvec().push_back(std::move(_pieckedRitem));
}

void Skull::BuildGeometry(void)
{
	if (UTIL.Get_Geomesh().find(m_Name) != UTIL.Get_Geomesh().end())
		return;

	std::ifstream fin("./Resource/Models/skull.txt");

	if (!fin)
	{
		MessageBox(0, L"./Resource/Models/skull.txt not found.", 0, 0);
		return;
	}

	UINT vcount = 0, tcount = 0;
	std::string ignore;

	fin >> ignore >> vcount;
	fin >> ignore >> tcount;
	fin >> ignore >> ignore >> ignore >> ignore;

	XMFLOAT3 vMinf3(Infinity, Infinity, Infinity);
	XMFLOAT3 vMaxf3(-Infinity, -Infinity, -Infinity);

	XMVECTOR vMin = XMLoadFloat3(&vMinf3);
	XMVECTOR vMax = XMLoadFloat3(&vMaxf3);

	std::vector<VERTEX> vertices(vcount);
	for (UINT i = 0; i < vcount; ++i)
	{
		fin >> vertices[i].Pos.x >> vertices[i].Pos.y >> vertices[i].Pos.z;
		fin >> vertices[i].Normal.x >> vertices[i].Normal.y >> vertices[i].Normal.z;

		XMVECTOR P = XMLoadFloat3(&vertices[i].Pos);

		XMFLOAT3 spherePos;
		XMStoreFloat3(&spherePos, XMVector3Normalize(P));

		float theta = atan2f(spherePos.z, spherePos.x);

		if (theta < 0.0f)
			theta += XM_2PI;

		float phi = acosf(spherePos.y);

		float u = theta / (2.0f * PI);
		float v = phi / PI;

		vertices[i].TexC = { u, v };

		vMin = XMVectorMin(vMin, P);
		vMax = XMVectorMax(vMax, P);
	}

	BoundingBox bounds;
	XMStoreFloat3(&bounds.Center, 0.5f * (vMin + vMax));
	XMStoreFloat3(&bounds.Extents, 0.5f * (vMax - vMin));

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

	geo->VertexBufferGPU = d3dutil::CreateDefaultBuffer(DEVICE.Get(), COM_LIST.Get(), vertices.data(), vbByteSize, geo->VertexBufferUploader);
	geo->IndexBufferGPU = d3dutil::CreateDefaultBuffer(DEVICE.Get(), COM_LIST.Get(), indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(VERTEX);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R32_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = g_totalIndexOffset;
	submesh.BaseVertexLocation = g_totalVertexOffset;
	submesh.Bounds = bounds;

	g_totalIndexOffset += tcount;
	g_totalVertexOffset += vcount;

	geo->DrawArgs[m_submeshName] = submesh;

	UTIL.Get_Geomesh()[geo->Name] = std::move(geo);
}

std::shared_ptr<Skull> Skull::Create(Object::COM_TYPE _type, std::string _geoname, std::string _submeshname, std::string _texname, std::string _matname)
{
	SKULL pSkull = std::make_shared<Skull>(_type,  _geoname, _submeshname, _texname, _matname);

	if (!pSkull) return nullptr;

	return pSkull;
}
