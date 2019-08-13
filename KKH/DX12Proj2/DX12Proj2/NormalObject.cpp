#include "pch.h"
#include "NormalObject.h"
#include "Utility_Manager.h"
#include "GeometryGenerator.h"

NormalObject::NormalObject(void)
	: Object()	
{
}

NormalObject::NormalObject(Object::COM_TYPE _type, std::string _name, 
	std::string _submeshname, std::string _texname, std::string _matname, 
	std::string _normalMap, ShapeType eType)
	: Object(_type, _name, _submeshname, _texname, _matname, _normalMap, eType)
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
	if (TEX.Get_Textures()[m_texName]->bRegister)
	{
		m_matCount = TEX.Get_Textures()[m_texName]->matCBCount;
		return FALSE;
	}

	if (TEX.Get_Textures()[m_normalTex]->bRegister)
		return FALSE;	

	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(UTIL.Get_SrvDiscriptorHeap()->GetCPUDescriptorHandleForHeapStart(), g_SrvHeapCount, UTIL.Get_CbvSrvDescriptorSize());

	auto defaultTex = TEX.Get_Textures()[m_texName]->Resource;
	m_matCount = g_MatCBcount++;
	TEX.Get_Textures()[m_texName]->matCBCount = m_matCount;
	TEX.Get_Textures()[m_texName]->srvHeapCount = g_SrvHeapCount++;
	TEX.Get_Textures()[m_normalTex]->srvHeapCount = g_SrvHeapCount++;
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

void NormalObject::BuildMaterials(void)
{
	auto pMat = std::make_unique<Material>();
	pMat->Name = m_matName;
	pMat->MatCBIndex = m_matCount;
	pMat->DiffuseSrvHeapIndex = TEX.Get_Textures()[m_texName]->srvHeapCount;
	pMat->NormalSrvHeapIndex = TEX.Get_Textures()[m_normalTex]->srvHeapCount;
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
	ritem->InstanceCount = m_vecInstanceData.size();
	ritem->IndexCount = ritem->Geo->DrawArgs[m_submeshName].IndexCount;
	ritem->StartIndexLocation = ritem->Geo->DrawArgs[m_submeshName].StartIndexLocation;
	ritem->BaseVertexLocation = ritem->Geo->DrawArgs[m_submeshName].BaseVertexLocation;
	ritem->Bounds = ritem->Geo->DrawArgs[m_submeshName].Bounds;

	ritem->Instances.resize(m_vecInstanceData.size());
	for (int i = 0; i < m_vecInstanceData.size(); ++i)
	{
		ritem->Instances[i] = m_vecInstanceData[i];
		ritem->Instances[i].MaterialIndex = m_matCount;
	}
	
	UTIL.Get_Drawlayer((int)DrawLayer::DL_OPAUQE).push_back(ritem.get());
	UTIL.Get_Ritemvec().push_back(std::move(ritem));
}

void NormalObject::BuildGeometry(void)
{
	Object::BuildGeometry();
	//GeometryGenerator geoGen;
	//GeometryGenerator::MeshData mesh;

	//SubmeshGeometry	SubMesh;
	//
	//switch (m_eShapeType)
	//{
	//case ST_BOX:
	//	mesh = geoGen.CreateBox(1.0f, 1.0f, 1.0f, 3);
	//	break;

	//case ST_GRID:
	//	mesh = geoGen.CreateGrid(20.0f, 30.0f, 60, 40);
	//	break;

	//case ST_SHPERE:
	//	mesh = geoGen.CreateSphere(0.5f, 20, 20);
	//	break;

	//case ST_CYLINEDER:
	//	mesh = geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20);
	//	break;

	//default:
	//	return;
	//}

	//SubMesh.IndexCount = (UINT)mesh.Indices32.size();
	//SubMesh.StartIndexLocation = 0;// g_totalIndexOffset;
	//SubMesh.BaseVertexLocation = 0;// g_totalVertexOffset;

	////g_totalIndexOffset = (UINT)mesh.Indices32.size();
	////g_totalVertexOffset = (UINT)mesh.Vertices.size();

	//XMFLOAT3 vMinf3(Infinity, Infinity, Infinity);
	//XMFLOAT3 vMaxf3(-Infinity, -Infinity, -Infinity);

	//XMVECTOR vMin = XMLoadFloat3(&vMinf3);
	//XMVECTOR vMax = XMLoadFloat3(&vMaxf3);

	//std::vector<VERTEX> vertices(mesh.Vertices.size());

	//for (size_t i = 0; i < vertices.size(); ++i)
	//{
	//	vertices[i].Pos = mesh.Vertices[i].Position;
	//	vertices[i].Normal = mesh.Vertices[i].Normal;
	//	vertices[i].TexC = mesh.Vertices[i].TexC;

	//	XMVECTOR P = XMLoadFloat3(&vertices[i].Pos);
	//	vMin = XMVectorMin(vMin, P);
	//	vMax = XMVectorMax(vMax, P);
	//}

	//BoundingBox bounds;
	//XMStoreFloat3(&bounds.Center, 0.5f * (vMin + vMax));
	//XMStoreFloat3(&bounds.Extents, 0.5f * (vMax - vMin));
	//SubMesh.Bounds = bounds;

	//std::vector<std::uint16_t> indices(3 * mesh.Indices32.size());

	//const UINT vbByteSize = (UINT)vertices.size() * sizeof(VERTEX);
	//const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	//
	//if (UTIL.Get_Geomesh().find(m_Name) == UTIL.Get_Geomesh().end())
	//{
	//	auto geo = std::make_unique<MeshGeometry>();
	//	geo->Name = m_Name;

	//	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	//	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	//	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	//	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	//	geo->VertexBufferGPU = d3dutil::CreateDefaultBuffer(DEVICE.Get(), COM_LIST.Get(), vertices.data(), vbByteSize, geo->VertexBufferUploader);
	//	geo->IndexBufferGPU = d3dutil::CreateDefaultBuffer(DEVICE.Get(), COM_LIST.Get(), indices.data(), ibByteSize, geo->IndexBufferUploader);

	//	geo->VertexByteStride = sizeof(VERTEX);
	//	geo->VertexBufferByteSize = vbByteSize;
	//	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	//	geo->IndexBufferByteSize = ibByteSize;

	//	geo->DrawArgs[m_submeshName] = SubMesh;
	//	UTIL.Get_Geomesh()[geo->Name] = std::move(geo);
	//}
	//else
	//{
	//	UTIL.Get_Geomesh()[m_Name]->DrawArgs[m_submeshName] = SubMesh;
	//}
}


std::shared_ptr<NormalObject> NormalObject::Create(Object::COM_TYPE _type, 
	std::string _name, std::string _submeshname, std::string _texname, 
	std::string _matname, std::string _normalMap, ShapeType eType)
{
	NORMALOBJECT pNormalObject = std::make_unique<NormalObject>(_type, _name, _submeshname, _texname, _matname, _normalMap, eType);

	if (!pNormalObject)
		return nullptr;

	return pNormalObject;
}
