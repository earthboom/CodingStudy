#pragma once

class ShadowMap
{
public:
	explicit ShadowMap(void);
	explicit ShadowMap(ID3D12Device* device, UINT _w, UINT _h);
	ShadowMap(const ShadowMap&) = delete;
	ShadowMap& operator = (const ShadowMap&) = delete;
	~ShadowMap(void);

	UINT Width(void) const;
	UINT Height(void) const;
	ID3D12Resource* Resorce(void);
	CD3DX12_GPU_DESCRIPTOR_HANDLE Srv(void) const;
	CD3DX12_CPU_DESCRIPTOR_HANDLE Dsv(void) const;

	D3D12_VIEWPORT Viewport(void) const;
	D3D12_RECT ScissorRect(void) const;

	void BuildDescriptor(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv,
						 CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv,
						 CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDev);

	void OnResize(UINT newWidth, UINT newHeight);

private:
	void BuildDescriptors(void);
	void BuildResource(void);

private:
	ID3D12Device* m_Device;
	D3D12_VIEWPORT m_Viewport;
	D3D12_RECT m_ScissorRect;

	UINT m_Width = 0;
	UINT m_Height = 0;
	DXGI_FORMAT m_Format;

	CD3DX12_GPU_DESCRIPTOR_HANDLE m_hGpuSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE m_hCpuDev;
	CD3DX12_CPU_DESCRIPTOR_HANDLE m_hCpuSrv;

	Microsoft::WRL::ComPtr<ID3D12Resource> m_ShadowMap;
};