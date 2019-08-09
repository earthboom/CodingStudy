#include "pch.h"
#include "Sky.h"
#include "Utility_Manager.h"
#include "Texture_Manger.h"

Sky::Sky(void)
	: Object()
{
}

Sky::Sky(Object::COM_TYPE _type, std::string _geoname, std::string _submeshname, std::string _texname, std::string _matname)
	: Object(_type, _geoname, _submeshname, _texname, _matname)
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

	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> tex2DList =
	{
		TEX.Get_Textures()["bricksTex"]->Resource,
		TEX.Get_Textures()["brickNormalTex"]->Resource,
		TEX.Get_Textures()["tileTex"]->Resource,
		TEX.Get_Textures()["tileNormalTex"]->Resource,
		TEX.Get_Textures()["defaultTex"]->Resource,
		TEX.Get_Textures()["defaultNormalTex"]->Resource,
	};

	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(UTIL.Get_SrvDiscriptorHeap()->GetCPUDescriptorHandleForHeapStart(), g_SrvHeapCount, UTIL.Get_CbvSrvDescriptorSize());

	auto defaultTex = TEX.Get_Textures()[m_texName]->Resource;
	m_matCount = g_MatCBcount++;
	TEX.Get_Textures()[m_texName]->matCBCount = m_matCount;
	TEX.Get_Textures()[m_texName]->srvHeapCount = g_SrvHeapCount++;
	TEX.Get_Textures()[m_texName]->bRegister = TRUE;

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

		hDescriptor.Offset(1, g_Graphics->Get_CbvSrvUavDescriptorSize());
	}

	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = defaultTex->GetDesc().MipLevels;
	srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
	srvDesc.Format = defaultTex->GetDesc().Format;
	DEVICE->CreateShaderResourceView(defaultTex.Get(), &srvDesc, hDescriptor);

	return TRUE;
}

void Sky::BuildMaterials(void)
{
}

void Sky::BuildRenderItem(void)
{
}

void Sky::BuildGeometry(void)
{
}

std::shared_ptr<Sky> Sky::Create(Object::COM_TYPE _type, std::string _geoname, std::string _submeshname, std::string _texname, std::string _matname)
{
	SKY pSky = std::make_shared<Sky>(_type, _geoname, _submeshname, _texname, _matname);

	if (!pSky) return nullptr;

	return pSky;
}
