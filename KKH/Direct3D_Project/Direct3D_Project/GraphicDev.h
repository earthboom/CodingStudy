#pragma once

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

	UINT m_iRtvDescriptiorSize;
	UINT m_iDsvDescriptiorSize;
	UINT m_CbvSrvUavDescriptorSize;

	std::wstring mMainWndCaption;
	D3D_DRIVER_TYPE m_DriverType;
	DXGI_FORMAT m_BackBufferFormat;
	DXGI_FORMAT m_DepthStencilFormat;

public:
	ID3D12Resource* Get_CurrentBackBuffer_Resource(void) const;
	D3D12_CPU_DESCRIPTOR_HANDLE Get_CurrentBackBufferView_Handle(void) const;
	D3D12_CPU_DESCRIPTOR_HANDLE Get_DepthStencilView_Handle(void) const;

	//std::function < const Microsoft::WRL::ComPtr<IDXGISwapChain>&()> Get_SwapChain = [&]()->const Microsoft::WRL::ComPtr<IDXGISwapChain>& { return m_SwapChain; };
	//std::function < const Microsoft::WRL::ComPtr<ID3D12Fence>&()> Get_Fence = [&]()->const Microsoft::WRL::ComPtr<ID3D12Fence>& { return m_Fence; };
	const Microsoft::WRL::ComPtr<IDXGISwapChain>& Get_SwapChain(void){ return m_SwapChain; }
	const Microsoft::WRL::ComPtr<ID3D12Fence>& Get_Fence(void){ return m_Fence; }

	//std::function <const Microsoft::WRL::ComPtr<ID3D12CommandQueue>&()> Get_CommandQueue = [&]()->const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& {return m_CommandQueue; };
	//std::function <const Microsoft::WRL::ComPtr<ID3D12CommandAllocator>&()> Get_CommandAllocator = [&]()->const Microsoft::WRL::ComPtr<ID3D12CommandAllocator>& {return m_CommandAllocator; };
	//std::function <const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>&()> Get_CommandList = [&]()->const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& {return m_CommandList; };
	const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& Get_CommandQueue(void) { return m_CommandQueue; }
	const Microsoft::WRL::ComPtr<ID3D12CommandAllocator>& Get_CommandAllocator(void) { return m_CommandAllocator; }
	const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& Get_CommandList(void) { return m_CommandList; }

	//std::function <const Microsoft::WRL::ComPtr<ID3D12Device>&()> Get_Device = [&]()->const Microsoft::WRL::ComPtr<ID3D12Device>&{ return m_Device; };
	const Microsoft::WRL::ComPtr<ID3D12Device>&	Get_Device(void) { return m_Device; }

	//std::function <const D3D12_VIEWPORT&()> Get_ScreenViewport = [&]()->const D3D12_VIEWPORT&{ return m_ScreenViewport; };
	//std::function <const D3D12_RECT&()> Get_ScissorRect = [&]()->const D3D12_RECT&{ return m_ScissorRect; };
	const D3D12_VIEWPORT& Get_ScreenViewport(void) { return m_ScreenViewport; }
	const D3D12_RECT& Get_ScissorRect(void) { return m_ScissorRect; }

	//std::function <const DXGI_FORMAT()> Get_BackBufferFormat = [=]()->const DXGI_FORMAT{ return m_BackBufferFormat; };
	//std::function <const DXGI_FORMAT()> Get_DepthStencilFormat = [=]()->const DXGI_FORMAT{ return m_DepthStencilFormat; };
	const DXGI_FORMAT Get_BackBufferFormat(void) { return m_BackBufferFormat; }
	const DXGI_FORMAT Get_DepthStencilFormat(void) { return m_DepthStencilFormat; }

	//std::function <UINT64&()> Get_CurrentFence = [&]()->UINT64& {return m_iCurrentFence; };
	//std::function <int&()> Get_Set_CurrBackBuffer = [&]()->int& {return m_iCurrBackBuffer; };
	UINT64& Get_CurrentFence(void) { return m_iCurrentFence; }
	int& Get_Set_CurrBackBuffer(void) { return m_iCurrBackBuffer; }

	//std::function <bool()> Get_4xMsaaState = [=]()->bool {return m_4xMsaaState; };
	//std::function <const UINT&()> Get_4xMsaaQuality = [&]()->const UINT&{ return m_4xMsaaQuality; };
	bool Get_4xMsaaState(void) { return m_4xMsaaState; }
	const UINT& Get_4xMsaaQuality(void) { return m_4xMsaaQuality; }

	//std::function <const UINT()> Get_RtvDescriptiorSize = [=]()->const UINT{ return m_iRtvDescriptiorSize; };
	//std::function <const UINT()> Get_DsvDescriptiorSize = [=]()->const UINT{ return m_iDsvDescriptiorSize; };
	//std::function <const UINT()> Get_CbvSrvUavDescriptorSize = [=]()->const UINT{ return m_CbvSrvUavDescriptorSize; };
	const UINT Get_RtvDescriptiorSize(void) const { return m_iRtvDescriptiorSize; }
	const UINT Get_DsvDescriptiorSize(void) const { return m_iDsvDescriptiorSize; }
	const UINT Get_CbvSrvUavDescriptorSize(void) const { return m_CbvSrvUavDescriptorSize; }

	static const int& Get_SwapChainBufferCount(void) { return m_iSwapChainBufferCount; }

	//const Microsoft::WRL::ComPtr<IDXGISwapChain>&	Get_SwapChain(void) const { return m_SwapChain; }
	//const Microsoft::WRL::ComPtr<ID3D12Fence>& Get_Fence(void) const { return m_Fence; }
	
	//const Microsoft::WRL::ComPtr<ID3D12CommandQueue>&			Get_CommandQueue(void) const { return m_CommandQueue; }
	//const Microsoft::WRL::ComPtr<ID3D12CommandAllocator>&		Get_CommandAllocator(void) const { return m_CommandAllocator; }
	//const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>&	Get_CommandList(void) const { return m_CommandList; }

	//const Microsoft::WRL::ComPtr<ID3D12Device>& Get_Device(void) const { return m_Device; }

	//const D3D12_VIEWPORT& Get_ScreenViewport(void) const { return m_ScreenViewport; }
	//const D3D12_RECT& Get_ScissorRect(void) const { return m_ScissorRect; }
};

typedef std::shared_ptr<CGraphicDev>						PGRAPHIC;
typedef Microsoft::WRL::ComPtr<ID3D12Device>				DEVICE;
typedef Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>	COMMANDLIST;