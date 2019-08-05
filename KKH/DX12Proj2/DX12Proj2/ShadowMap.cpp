#include "pch.h"
#include "ShadowMap.h"

ShadowMap::ShadowMap(void)
	: m_Device(nullptr)
	, m_Width(0), m_Height(0)
	, m_Format(DXGI_FORMAT_R24G8_TYPELESS)
	, m_ShadowMap(nullptr)
{
	m_Viewport = { 0, };
	m_ScissorRect = { 0, };
}

ShadowMap::ShadowMap(ID3D12Device* device, UINT _w, UINT _h)
	: m_Device(device)
	, m_Width(_w), m_Height(_h)
{
	m_Viewport = { 0.0f, 0.0f, (float)_w, (float)_h, 0.0f, 0.0f };
	m_ScissorRect = { 0, 0, (int)_w, (int)_h };

	BuildResource();
}

ShadowMap::~ShadowMap(void)
{
}

UINT ShadowMap::Width(void) const
{
	return 0;
}

UINT ShadowMap::Height(void) const
{
	return 0;
}

ID3D12Resource* ShadowMap::Resorce(void)
{
	return m_ShadowMap.Get();
}

CD3DX12_GPU_DESCRIPTOR_HANDLE ShadowMap::Srv(void) const
{
	return m_hGpuSrv;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE ShadowMap::Dsv(void) const
{
	return m_hCpuDev;
}

D3D12_VIEWPORT ShadowMap::Viewport(void) const
{
	return D3D12_VIEWPORT();
}

D3D12_RECT ShadowMap::ScissorRect(void) const
{
	return D3D12_RECT();
}

void ShadowMap::BuildDescriptor(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv, CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv, CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDev)
{
}

void ShadowMap::OnResize(UINT newWidth, UINT newHeight)
{
}

void ShadowMap::BuildDescriptors(void)
{
}

void ShadowMap::BuildResource(void)
{
	D3D12_RESOURCE_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));

	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Alignment = 0;
	texDesc.Width = m_Width;
	texDesc.Height = m_Height;
	texDesc.DepthOrArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.Format = m_Format;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear;
	optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;

	ThrowIfFailed(m_Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&optClear,
		IID_PPV_ARGS(&m_ShadowMap)));
}
