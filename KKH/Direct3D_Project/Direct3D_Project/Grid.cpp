#include "stdafx.h"
#include "Grid.h"
#include "Utility_Manager.h"
#include "GeometryGenerator.h"
#include "FrameResource.h"
#include "Texture_Manger.h"

Grid::Grid(void)
	: Object()
{
}

Grid::Grid(Object::COM_TYPE _type, std::string _name, std::string _submeshname, std::string _texname, std::string _matname)
	: Object(_type, _name, _submeshname, _texname, _matname)
{
}

Grid::~Grid(void)
{
}

bool Grid::Ready(void)
{
	BuildDescriptorHeaps();
	BuildGeometry();
	BuildMaterials();
	BuildRenderItem();

	return TRUE;
}

bool Grid::Update(const float & dt)
{
	return TRUE;
}

bool Grid::Render(const float & dt)
{
	return TRUE;
}

void Grid::BuildDescriptorHeaps(void)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(UTIL.Get_SrvDiscriptorHeap()->GetCPUDescriptorHandleForHeapStart());

	auto tex = TEX.Get_Textures()[m_texName]->Resource;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = tex->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;
	//srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	GRAPHIC_DEV->CreateShaderResourceView(tex.Get(), &srvDesc, hDescriptor);

	hDescriptor.Offset(1, UTIL.Get_CbvSrvDescriptorSize());

	//srvDesc.Format = {};
	//GRAPHIC_DEV->CreateShaderResourceView(nullptr, &srvDesc, hDescriptor);

	// next descriptor
	//hDescriptor.Offset(1, UTIL.Get_CbvSrvDescriptorSize());

	//srvDesc.Format = {};
	//GRAPHIC_DEV->CreateShaderResourceView(nullptr, &srvDesc, hDescriptor);
}

void Grid::BuildMaterials(void)
{
	auto mat = std::make_unique<Material>();
	mat->Name = m_matName;
	mat->MatCBIndex = g_MatCBcount;
	mat->DiffuseSrvHeapIndex = g_MatCBcount;
	mat->DiffuseAlbedo = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mat->FresnelR0 = DirectX::XMFLOAT3(0.01f, 0.01f, 0.01f);
	mat->Roughness = 0.125f;

	UTIL.Get_Materials()[m_matName] = std::move(mat);

	++g_MatCBcount;
}

void Grid::BuildRenderItem(void)
{
	auto ritem = std::make_unique<RenderItem>();
	ritem->World = MathHelper::Identity4x4();
	DirectX::XMStoreFloat4x4(&ritem->TexTransform, DirectX::XMMatrixScaling(5.0f, 5.0f, 1.0f));
	ritem->objCBIndex = g_ObjCBcount;
	ritem->Mat = UTIL.Get_Materials()[m_matName].get();
	ritem->Geo = UTIL.Get_Geomesh()[m_Name].get();
	ritem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	ritem->IndexCount = ritem->Geo->DrawArgs[m_submeshName].IndexCount;
	ritem->StartIndexLocation = ritem->Geo->DrawArgs[m_submeshName].StartIndexLocation;
	ritem->BaseVertexLocation = ritem->Geo->DrawArgs[m_submeshName].BaseVertexLocation;

	UTIL.Get_Drawlayer((int)DrawLayer::DL_OPAUQE).push_back(ritem.get());

	UTIL.Get_Ritemvec().push_back(std::move(ritem));

	++g_ObjCBcount;
}

void Grid::BuildGeometry(void)
{
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData grid = geoGen.CreateGrid(160.0f, 160.0f, 50, 50);

	std::vector<Vertex> vertices(grid.Vertices.size());
	for (size_t i = 0; i < grid.Vertices.size(); ++i)
	{
		auto& p = grid.Vertices[i].Position;
		vertices[i].Pos = p;
		vertices[i].Pos.y = GeometryGenerator::GetHillHeight(p.x, p.z);
		vertices[i].Normal = GeometryGenerator::GetHillsNormal(p.x, p.z);
		vertices[i].TexC = grid.Vertices[i].TexC;
	}

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);

	std::vector<std::uint16_t> indices = grid.GetIndices16();
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = m_Name;

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = D3DUTIL.CreateDefaultBuffer(GRAPHIC->Get_Device().Get(), 
		COM_LIST.Get(), vertices.data(), vbByteSize, geo->VertexBufferUploader);

	geo->IndexBufferGPU = D3DUTIL.CreateDefaultBuffer(GRAPHIC->Get_Device().Get(), 
		COM_LIST.Get(), indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Vertex);
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

std::shared_ptr<Grid> Grid::Create(Object::COM_TYPE _type, std::string _name, std::string _submeshname,
					std::string _texname, std::string _matname)
{
	GRID pGrid = std::make_shared<Grid>(_type, _name, _submeshname, _texname, _matname);
	if (!pGrid) return nullptr;
	
	return pGrid;
}
