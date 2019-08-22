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
	, m_Format(DXGI_FORMAT_R24G8_TYPELESS)
	, m_ShadowMap(nullptr)
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
	return m_Width;
}

UINT ShadowMap::Height(void) const
{
	return m_Height;
}

ID3D12Resource* ShadowMap::Resource(void)
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
	return m_Viewport;
}

D3D12_RECT ShadowMap::ScissorRect(void) const
{
	return m_ScissorRect;
}

void ShadowMap::BuildDescriptor(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv, CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv, CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDsv)
{
	// Save references to the descriptors. 
	m_hGpuSrv = hGpuSrv;
	m_hCpuDev = hCpuDsv;
	m_hCpuSrv = hCpuSrv;

	//  Create the descriptors
	BuildDescriptors();
}

void ShadowMap::OnResize(UINT newWidth, UINT newHeight)
{
	if ((m_Width != newWidth) || (m_Height != newHeight))
	{
		m_Width = newWidth;
		m_Height = newHeight;
		
		BuildResource();		
		BuildDescriptors();	// New resource, so we need new descriptors to that resource.
	}
}

void ShadowMap::BuildDescriptors(void)
{
	// Create SRV to resource so we can sample the shadow map in a shader program.
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	srvDesc.Texture2D.PlaneSlice = 0;
	m_Device->CreateShaderResourceView(m_ShadowMap.Get(), &srvDesc, m_hCpuSrv);

	// Create DSV to resource so we can render to the shadow map.
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.Texture2D.MipSlice = 0;
	m_Device->CreateDepthStencilView(m_ShadowMap.Get(), &dsvDesc, m_hCpuDev);
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
