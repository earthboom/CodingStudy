#include "pch.h"
#include "Skull.h"
#include "Utility_Manager.h"
#include "Texture_Manger.h"

Skull::Skull(void)
	: Object()
	, m_Skull(nullptr)
	, mSkullTranslation(0.0f, 1.0f, -5.0f)
{
}

Skull::Skull(Object::COM_TYPE _type, std::string _name, std::string _submeshname, std::string _texname, std::string _matname)
	: Object(_type, _name, _submeshname, _texname, _matname)
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

void Skull::BuildDescriptorHeaps(void)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(UTIL.Get_SrvDiscriptorHeap()->GetCPUDescriptorHandleForHeapStart(), g_MatCBcount, UTIL.Get_CbvSrvDescriptorSize());

	auto bricksTex	= TEX.Get_Textures()["bricksTex"]->Resource;
	auto stoneTex	= TEX.Get_Textures()["stoneTex"]->Resource;
	auto tileTex	= TEX.Get_Textures()["tileTex"]->Resource;
	auto crateTex	= TEX.Get_Textures()["crateTex"]->Resource;
	auto iceTex		= TEX.Get_Textures()["iceTex"]->Resource;
	auto grassTex	= TEX.Get_Textures()["grassTex"]->Resource;
	auto defaultTex = TEX.Get_Textures()["defaultTex"]->Resource;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = bricksTex->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = bricksTex->GetDesc().MipLevels;
	DEVICE->CreateShaderResourceView(bricksTex.Get(), &srvDesc, hDescriptor);

	hDescriptor.Offset(1, UTIL.Get_CbvSrvDescriptorSize());
	srvDesc.Format = stoneTex->GetDesc().Format;
	srvDesc.Texture2D.MipLevels = stoneTex->GetDesc().MipLevels;
	DEVICE->CreateShaderResourceView(stoneTex.Get(), &srvDesc, hDescriptor);

	hDescriptor.Offset(1, UTIL.Get_CbvSrvDescriptorSize());
	srvDesc.Format = tileTex->GetDesc().Format;
	srvDesc.Texture2D.MipLevels = tileTex->GetDesc().MipLevels;
	DEVICE->CreateShaderResourceView(tileTex.Get(), &srvDesc, hDescriptor);

	hDescriptor.Offset(1, UTIL.Get_CbvSrvDescriptorSize());
	srvDesc.Format = crateTex->GetDesc().Format;
	srvDesc.Texture2D.MipLevels = crateTex->GetDesc().MipLevels;
	DEVICE->CreateShaderResourceView(crateTex.Get(), &srvDesc, hDescriptor);

	hDescriptor.Offset(1, UTIL.Get_CbvSrvDescriptorSize());
	srvDesc.Format = iceTex->GetDesc().Format;
	srvDesc.Texture2D.MipLevels = iceTex->GetDesc().MipLevels;
	DEVICE->CreateShaderResourceView(iceTex.Get(), &srvDesc, hDescriptor);

	hDescriptor.Offset(1, UTIL.Get_CbvSrvDescriptorSize());
	srvDesc.Format = grassTex->GetDesc().Format;
	srvDesc.Texture2D.MipLevels = grassTex->GetDesc().MipLevels;
	DEVICE->CreateShaderResourceView(grassTex.Get(), &srvDesc, hDescriptor);

	hDescriptor.Offset(1, UTIL.Get_CbvSrvDescriptorSize());
	srvDesc.Format = defaultTex->GetDesc().Format;
	srvDesc.Texture2D.MipLevels = defaultTex->GetDesc().MipLevels;
	DEVICE->CreateShaderResourceView(defaultTex.Get(), &srvDesc, hDescriptor);
}

void Skull::BuildMaterials(void)
{
	auto bricks = std::make_unique<Material>();
	bricks->Name = "bricks";
	bricks->MatCBIndex = g_MatCBcount;
	bricks->DiffuseSrvHeapIndex = g_MatCBcount++;
	bricks->DiffuseAlbedo = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	bricks->FresnelR0 = DirectX::XMFLOAT3(0.02f, 0.02f, 0.02f);
	bricks->Roughness = 0.1f;

	auto stone = std::make_unique<Material>();
	stone->Name = "stone";
	stone->MatCBIndex = g_MatCBcount;
	stone->DiffuseSrvHeapIndex = g_MatCBcount++;
	stone->DiffuseAlbedo = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	stone->FresnelR0 = DirectX::XMFLOAT3(0.05f, 0.05f, 0.05f);
	stone->Roughness = 0.3f;

	auto tile = std::make_unique<Material>();
	tile->Name = "tile";
	tile->MatCBIndex = g_MatCBcount;
	tile->DiffuseSrvHeapIndex = g_MatCBcount++;
	tile->DiffuseAlbedo = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	tile->FresnelR0 = DirectX::XMFLOAT3(0.02f, 0.02f, 0.02f);
	tile->Roughness = 0.3f;

	auto crate = std::make_unique<Material>();
	crate->Name = "crate";
	crate->MatCBIndex = g_MatCBcount;
	crate->DiffuseSrvHeapIndex = g_MatCBcount++;
	crate->DiffuseAlbedo = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	crate->FresnelR0 = DirectX::XMFLOAT3(0.02f, 0.02f, 0.02f);
	crate->Roughness = 0.3f;

	auto ice = std::make_unique<Material>();
	ice->Name = "ice";
	ice->MatCBIndex = g_MatCBcount;
	ice->DiffuseSrvHeapIndex = g_MatCBcount++;
	ice->DiffuseAlbedo = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	ice->FresnelR0 = DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f);
	ice->Roughness = 0.0f;

	auto grass = std::make_unique<Material>();
	grass->Name = "grass";
	grass->MatCBIndex = g_MatCBcount;
	grass->DiffuseSrvHeapIndex = g_MatCBcount++;
	grass->DiffuseAlbedo = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	grass->FresnelR0 = DirectX::XMFLOAT3(0.05f, 0.05f, 0.05f);
	grass->Roughness = 0.2f;

	auto skullMat = std::make_unique<Material>();
	skullMat->Name = "skullMat";
	skullMat->MatCBIndex = g_MatCBcount;
	skullMat->DiffuseSrvHeapIndex = g_MatCBcount++;
	skullMat->DiffuseAlbedo = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	skullMat->FresnelR0 = DirectX::XMFLOAT3(0.05f, 0.05f, 0.05f);
	skullMat->Roughness = 0.5f;

	UTIL.Get_Materials()["bricks"]	= std::move(bricks);
	UTIL.Get_Materials()["stone"]	= std::move(stone);
	UTIL.Get_Materials()["tile"]	= std::move(tile);
	UTIL.Get_Materials()["crate"]	= std::move(crate);
	UTIL.Get_Materials()["ice"]		= std::move(ice);
	UTIL.Get_Materials()["grass"]	= std::move(grass);
	UTIL.Get_Materials()["skullMat"]= std::move(skullMat);
}

void Skull::BuildRenderItem(void)
{
	auto ritem = std::make_unique<RenderItem>();
	ritem->World = MathHelper::Identity4x4();
	ritem->TexTransform = MathHelper::Identity4x4();
	ritem->objCBIndex = g_ObjCBcount++;
	ritem->Mat = UTIL.Get_Materials()["tile"].get();
	ritem->Geo = UTIL.Get_Geomesh()[m_Name].get();
	ritem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	ritem->InstanceCount = 0;
	ritem->IndexCount = ritem->Geo->DrawArgs[m_submeshName].IndexCount;
	ritem->StartIndexLocation = ritem->Geo->DrawArgs[m_submeshName].StartIndexLocation;
	ritem->BaseVertexLocation = ritem->Geo->DrawArgs[m_submeshName].BaseVertexLocation;
	ritem->Bounds = ritem->Geo->DrawArgs[m_submeshName].Bounds;

	//Generate instance data
	const int n = 5;
	ritem->Instances.resize(n * n * n);

	float width = 200.0f;
	float height = 200.0f;
	float depth = 200.0f;

	float x = -0.5f * width;
	float y = -0.5f * height;
	float z = -0.5f * depth;

	float dx = width / (n - 1);
	float dy = height / (n - 1);
	float dz = depth / (n - 1);

	for (int k = 0; k < n; ++k)
	{
		for (int i = 0; i < n; ++i)
		{
			for (int j = 0; j < n; ++j)
			{
				int index = k * n * n + i * n + j;

				ritem->Instances[index].World = XMFLOAT4X4(
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					x + j * dx, y + i * dy, z + k * dz, 1.0f);

				XMStoreFloat4x4(&ritem->Instances[index].TexTransform, XMMatrixScaling(2.0f, 2.0f, 1.0f));
				ritem->Instances[index].MaterialIndex = index % UTIL.Get_Materials().size();

			}
		}
	}

	
	UTIL.Get_Ritemvec().push_back(std::move(ritem));
	for (auto& e : UTIL.Get_Ritemvec())
	{
		UTIL.Get_Drawlayer((int)DrawLayer::DL_OPAUQE).push_back(e.get());
	}	
	
}

void Skull::BuildGeometry(void)
{
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
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;
	submesh.Bounds = bounds;

	geo->DrawArgs[m_submeshName] = submesh;

	UTIL.Get_Geomesh()[geo->Name] = std::move(geo);
}

std::shared_ptr<Skull> Skull::Create(Object::COM_TYPE _type, std::string _name, std::string _submeshname, std::string _texname, std::string _matname)
{
	SKULL pSkull = std::make_shared<Skull>(_type, _name, _submeshname, _texname, _matname);

	if (!pSkull) return nullptr;

	return pSkull;
}
