#include "pch.h"
#include "Sky.h"
#include "Utility_Manager.h"
#include "Texture_Manger.h"

Sky::Sky(void)
	: Object()
{
}

Sky::Sky(Object::COM_TYPE _type, std::string _geoname, std::string _submeshname, 
	std::string _texname, std::string _matname, std::string _normalMap)
	: Object(_type, _geoname, _submeshname, _texname, _matname, _normalMap)
{
}

Sky::~Sky(void)
{
}

bool Sky::Ready(void)
{
	BuildDescriptorHeaps();
	BuildGeometry();
	BuildMaterials();
	BuildRenderItem();

	return TRUE;
}

bool Sky::Update(const CTimer& mt)
{
	return TRUE;
}

bool Sky::Render(const CTimer& mt)
{
	return TRUE;
}

bool Sky::BuildDescriptorHeaps(void)
{
	if (TEX.Get_Textures()[m_texName]->bRegister)
	{
		m_matCount = TEX.Get_Textures()[m_texName]->matCBCount;
		return FALSE;
	}
	
	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(UTIL.Get_SrvDiscriptorHeap()->GetCPUDescriptorHandleForHeapStart(), g_SrvHeapCount, UTIL.Get_CbvSrvDescriptorSize());

	auto defaultTex = TEX.Get_Textures()[m_texName]->Resource;
	m_matCount = g_MatCBcount++;
	TEX.Get_Textures()[m_texName]->matCBCount = m_matCount;
	TEX.Get_Textures()[m_texName]->srvHeapCount = g_SrvHeapCount++;
	//TEX.Get_Textures()[m_normalTex]->srvHeapCount = g_SrvHeapCount++;
	TEX.Get_Textures()[m_texName]->bRegister = TRUE;

	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> tex2DList =
	{
		TEX.Get_Textures()["bricksTex"]->Resource,
		TEX.Get_Textures()["brickNormalTex"]->Resource,
		TEX.Get_Textures()["tileTex"]->Resource,
		TEX.Get_Textures()["tileNormalTex"]->Resource,
		TEX.Get_Textures()["defaultTex"]->Resource,
		TEX.Get_Textures()["defaultNormalTex"]->Resource,
	};

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	for (UINT i = 0; i < (UINT)tex2DList.size(); ++i)
	{
		srvDesc.Format = tex2DList[i]->GetDesc().Format;
		srvDesc.Texture2D.MipLevels = tex2DList[i]->GetDesc().MipLevels;
		DEVICE->CreateShaderResourceView(tex2DList[i].Get(), &srvDesc, hDescriptor);

		//hDescriptor.Offset(g_SrvHeapCount, g_Graphics->Get_CbvSrvUavDescriptorSize());
	}

	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = defaultTex->GetDesc().MipLevels;
	srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
	srvDesc.Format = defaultTex->GetDesc().Format;
	DEVICE->CreateShaderResourceView(defaultTex.Get(), &srvDesc, hDescriptor);

	UINT skyTexHeapIndex = (UINT)tex2DList.size();
	UINT shadowMapHeapIndex = skyTexHeapIndex + 1;

	UINT nullCubeSrvIndex = shadowMapHeapIndex + 1;
	UINT nullTexSrvIndex = nullCubeSrvIndex + 1;

	auto srvCpuStart = UTIL.Get_SrvDiscriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	auto srvGpuStart = UTIL.Get_SrvDiscriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	auto dsvCpuStart = g_Graphics->Get_DepthStencilView_Handle();

	auto nullSrv = CD3DX12_CPU_DESCRIPTOR_HANDLE(srvCpuStart, nullCubeSrvIndex, g_Graphics->Get_CbvSrvUavDescriptorSize());
	UTIL.Get_NullSrv() = CD3DX12_GPU_DESCRIPTOR_HANDLE(srvGpuStart, nullCubeSrvIndex, g_Graphics->Get_CbvSrvUavDescriptorSize());

	DEVICE->CreateShaderResourceView(nullptr, &srvDesc, nullSrv);
	nullSrv.Offset(1, g_Graphics->Get_CbvSrvUavDescriptorSize());

	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	DEVICE->CreateShaderResourceView(nullptr, &srvDesc, nullSrv);

	UTIL.Get_ShadowMap()->BuildDescriptor(
		CD3DX12_CPU_DESCRIPTOR_HANDLE(srvCpuStart, shadowMapHeapIndex, g_Graphics->Get_CbvSrvUavDescriptorSize()),
		CD3DX12_GPU_DESCRIPTOR_HANDLE(srvGpuStart, shadowMapHeapIndex, g_Graphics->Get_CbvSrvUavDescriptorSize()),
		CD3DX12_CPU_DESCRIPTOR_HANDLE(dsvCpuStart, 1, g_Graphics->Get_DsvDescriptiorSize()));

	UTIL.Get_SkyTexHeapIndex() = skyTexHeapIndex;
	UTIL.Get_ShadowMapHeapIndex() = shadowMapHeapIndex;
	UTIL.Get_NullCubeSrvIndex() = nullCubeSrvIndex;
	UTIL.Get_NullTexSrvIndex() = nullTexSrvIndex;

	return TRUE;
}

void Sky::BuildMaterials(void)
{
	auto pMat = std::make_unique<Material>();
	pMat->Name = m_matName;
	pMat->MatCBIndex = m_matCount;
	pMat->DiffuseSrvHeapIndex = TEX.Get_Textures()[m_texName]->srvHeapCount;
	pMat->NormalSrvHeapIndex = TEX.Get_Textures()[m_texName]->srvHeapCount + 1;
	pMat->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pMat->FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	pMat->Roughness = 1.0f;

	UTIL.Get_Materials()[m_matName] = std::move(pMat);
}

void Sky::BuildRenderItem(void)
{
	auto ritem = std::make_unique<RenderItem>();

	ritem->objCBIndex = g_ObjCBcount++;
	ritem->World = m_vecInstanceData[0].World;
	ritem->TexTransform = m_vecInstanceData[0].TexTransform;
	ritem->Mat = UTIL.Get_Materials()[m_matName].get();
	ritem->Geo = UTIL.Get_Geomesh()[m_Name].get();
	ritem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	//ritem->InstanceCount = m_vecInstanceData.size();
	ritem->IndexCount = ritem->Geo->DrawArgs[m_submeshName].IndexCount;
	ritem->StartIndexLocation = ritem->Geo->DrawArgs[m_submeshName].StartIndexLocation;
	ritem->BaseVertexLocation = ritem->Geo->DrawArgs[m_submeshName].BaseVertexLocation;
	//ritem->Bounds = ritem->Geo->DrawArgs[m_submeshName].Bounds;

	UTIL.Get_Drawlayer((int)DrawLayer::DL_OPAUQE).push_back(ritem.get());
	UTIL.Get_Ritemvec().push_back(std::move(ritem));
}

void Sky::BuildGeometry(void)
{
	m_eShapeType = ST_SHPERE;
	Object::BuildGeometry();
}

std::shared_ptr<Sky> Sky::Create(Object::COM_TYPE _type, std::string _geoname, 
	std::string _submeshname, std::string _texname, std::string _matname, std::string _normalMap)
{
	SKY pSky = std::make_shared<Sky>(_type, _geoname, _submeshname, _texname, _matname, _normalMap);

	if (!pSky) return nullptr;

	return pSky;
}
