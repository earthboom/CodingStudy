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
	//bool Init_Graphic(const WINDOW_MODE _mode, const HWND _hwnd, const WORD& _sizeX, const WORD& _sizeY, const BOOL _bMSAA, const float _screedepth = SCREEN_DEPTH, const float _screennear = SCREEN_NEAR);
	//void ShutDown(void);

	bool Init_Graphic(void);
	void ShutDown(void);

	void LogAdapters(void);	//시스템에 있는 모든 어댑터를 열거
	void LogAdapterOutputs(IDXGIAdapter* adapter);	//주어진 한 어댑터에 연관된 모든 출력을 열거
	void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);	//주어진 출력과 디스플레이 형식을 지원하는 모든 디스플레이 모드를 담은 목록을 얻음

	void BeginScene(const float _r, const float _g, const float _b, const float _alpha);
	void EndScene(void);

protected:
	void CreateCommandObjects(void);
	void CreateSwapChain(void);
	void CreateRtvAndDsvDescriptorHeaps(void);

protected:
	void OnResize(void);

	void FlushCommandQueue(void);

	ID3D12Resource* Get_CurrentBackBuffer_Resource(void);
	D3D12_CPU_DESCRIPTOR_HANDLE Get_CurrentBackBufferView_Handle(void) const;
	D3D12_CPU_DESCRIPTOR_HANDLE Get_DepthStencilView_Handle(void) const;
private:
	//ID3D11Device*				m_pDevice;
	//ID3D11DeviceContext*		m_pContext;
	//IDXGISwapChain*				m_pSwapChain;

	//ID3D11RenderTargetView*		m_pRenderTargetView;
	//ID3D11DepthStencilState*	m_pDepthStencilState;
	//ID3D11DepthStencilView*		m_pDepthStencilView;
	//ID3D11Texture2D*			m_pDepthStencilBuffer;

	//ID3D11RasterizerState*		m_pRasterState;

	//D3D11_VIEWPORT				m_pViewport;

	//bool	m_vsync_enabled;
	//int		m_vidioCardMemory;
	//char	m_vidioCardDescription[128];

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
};

typedef std::shared_ptr<CGraphicDev>	PGRAPHIC;