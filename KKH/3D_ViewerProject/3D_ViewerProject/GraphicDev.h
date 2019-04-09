#pragma once

#include "Const.h"

class CGraphicDev
{
public:
	enum WINDOW_MODE { MODE_FULL, MODE_WIN };

public:
	explicit CGraphicDev(void);
	CGraphicDev(const CGraphicDev&) = default;
	~CGraphicDev(void);

public:
	bool Init_Graphic(void);
	void OnResize(void);

	void LogAdapters(void);	//Enumerate all adapter in the system.
	void LogAdapterOutputs(IDXGIAdapter* adapter);	//Enumerate all outputs associated with a given adapter.
	void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);	//Obtain a list all display modes that support a given output and display format.

protected:
	void CreateCommandObjects(void);
	void CreateSwapChain(void);
	void CreateRtvAndDsvDescriptorHeaps(void);

public:
	void FlushCommandQueue(void);
	
private:
	//Set true to use 4X MSAA 
	bool m_4xMsaaState = FALSE;	//4X MSAA enalbed
	UINT m_4xMsaaQuality = 0;	//quality level of 4X MSAA

	Microsoft::WRL::ComPtr<ID3D12Device>				m_Device;
	Microsoft::WRL::ComPtr<IDXGIFactory4>				m_Factory;
	Microsoft::WRL::ComPtr<IDXGISwapChain>				m_SwapChain;

	Microsoft::WRL::ComPtr<ID3D12Fence>					m_Fence;
	UINT64												m_iCurrentFence = 0;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue>			m_CommandQueue;			//Command Queue Interface represented
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator>		m_CommandAllocator;		//Command Allocator Interface represented
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>	m_CommandList;			//Command List Interface represented

	static const int m_iSwapChainBufferCount = 2;
	int m_iCurrBackBuffer = 0;
	Microsoft::WRL::ComPtr<ID3D12Resource>				m_SwapChainBuffer[m_iSwapChainBufferCount];
	Microsoft::WRL::ComPtr<ID3D12Resource>				m_DepthStencilBuffer;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>		m_RtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>		m_DsvHeap;

	D3D12_VIEWPORT	m_ScreenViewport;
	D3D12_RECT		m_ScissorRect;

	UINT m_iRtvDescriptiorSize = 0;
	UINT m_iDsvDescriptiorSize = 0;
	UINT m_CbvSrvUavDescriptorSize = 0;

	std::wstring mMainWndCaption = L"D3D_App";
	D3D_DRIVER_TYPE m_DriverType = D3D_DRIVER_TYPE_HARDWARE;
	DXGI_FORMAT m_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT m_DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

public:
	const Microsoft::WRL::ComPtr<IDXGISwapChain>&	Get_SwapChain(void) const { return m_SwapChain; }

	ID3D12Resource* Get_CurrentBackBuffer_Resource(void) const;
	D3D12_CPU_DESCRIPTOR_HANDLE Get_CurrentBackBufferView_Handle(void) const;
	D3D12_CPU_DESCRIPTOR_HANDLE Get_DepthStencilView_Handle(void) const;

	const Microsoft::WRL::ComPtr<ID3D12CommandQueue>&			Get_CommandQueue(void) const { return m_CommandQueue; }
	const Microsoft::WRL::ComPtr<ID3D12CommandAllocator>&		Get_CommandAllocator(void) const { return m_CommandAllocator; }
	const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>&	Get_CommandList(void) const { return m_CommandList; }

	const Microsoft::WRL::ComPtr<ID3D12Device>& Get_Device(void) const { return m_Device; }

	const D3D12_VIEWPORT& Get_ScreenViewport(void) const { return m_ScreenViewport; }
	const D3D12_RECT& Get_ScissorRect(void) const { return m_ScissorRect; }

	int& Get_Set_CurrBackBuffer(void) { return m_iCurrBackBuffer; }
	static const int& Get_SwapChainBufferCount(void) { return m_iSwapChainBufferCount; }
};

typedef std::shared_ptr<CGraphicDev>	PGRAPHIC;