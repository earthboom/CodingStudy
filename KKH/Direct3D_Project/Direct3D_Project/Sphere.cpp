#include "stdafx.h"
#include "Object.h"
#include "Sphere.h"
#include "GeometryGenerator.h"
#include "FrameResource.h"
#include "Utility_Manager.h"
#include "Texture_Manger.h"

Sphere::Sphere(void)
	: Object()
{
}

Sphere::Sphere(Object::COM_TYPE _type, std::string _name, std::string _submeshname, std::string _texname, std::string _matname)
	: Object(_type, _name, _submeshname, _texname, _matname)
{
}

Sphere::~Sphere(void)
{
}

bool Sphere::Ready(void)
{
	BuildDescriptorHeaps();
	BuildGeometry();
	BuildMaterials();
	BuildRenderItem();

	return TRUE;
}

bool Sphere::Update(const CTimer& mt)
{
	return TRUE;
}

bool Sphere::Render(const CTimer& mt)
{
	return TRUE;
}

void Sphere::BuildDescriptorHeaps(void)
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

void Sphere::BuildMaterials(void)
{
}

void Sphere::BuildRenderItem(void)
{
}

void Sphere::BuildGeometry(void)
{
}

std::shared_ptr<Sphere> Sphere::Create(Object::COM_TYPE _type, std::string _name, std::string _submeshname, std::string _texname, std::string _matname)
{
	SPHERE pSphere = std::make_shared<Sphere>(_type, _name, _submeshname, _texname, _matname);
	if (!pSphere) return nullptr;

	return pSphere;
}
