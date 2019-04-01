#include "stdafx.h"
#include "GraphicDev.h"
#include "Function.h"

CGraphicDev::CGraphicDev(void)
	//: m_pDevice(nullptr)
	//, m_pContext(nullptr)
	//, m_pSwapChain(nullptr)
	//, m_pRenderTargetView(nullptr)
	//, m_pDepthStencilState(nullptr)
	//, m_pDepthStencilView(nullptr)
	//, m_pDepthStencilBuffer(nullptr)
	//, m_pRasterState(nullptr)
{
}

CGraphicDev::~CGraphicDev(void)
{
}

//bool CGraphicDev::Init_Graphic(const WINDOW_MODE _mode, const HWND _hwnd, const WORD & _sizeX, const WORD & _sizeY, const BOOL _bMSAA, const float _screedepth, const float _screennear)
//{
//	UINT numModes = 0, numerator = 0, denomiator = 0, stringLength = 0, error = 0;
//	float fieldOfView = 0.0f, screenAspect = 0.0f;
//
//	//DirectX 그래픽 인터페이스 팩토리 생성
//	IDXGIFactory* factory;
//	if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory)))
//		return FALSE;
//
//	//팩토리 객체를 이용해 첫번째 그래픽 카드 인터페이스에 대한 어댑터 생성.
//	IDXGIAdapter* adapter;
//	if (FAILED(factory->EnumAdapters(0, &adapter)))
//		return FALSE;
//
//	//출력(모니터)에 대한 첫 아답터 나열.
//	IDXGIOutput* adapterOuput;
//	if (FAILED(adapter->EnumOutputs(0, &adapterOuput)))
//		return FALSE;
//	
//	//DXGI_FORMAT_R8G8B8A8_UNORM 모니터 출력 디스플레이 포멧에 맞는 모드의 개수를 구함.
//	if (FAILED(adapterOuput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL)))
//		return FALSE;
//
//	//가능한 모든 모니터와 그래픽카드 조합을 저장할 리스트 생성.
//	DXGI_MODE_DESC*	displayModeList = new DXGI_MODE_DESC[numModes];
//	if (!displayModeList)
//		return FALSE;
//
//	//디스플레이 모드에 대한 리스트 구조를 채움.
//	if (FAILED(adapterOuput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList)))
//		return FALSE;
//
//
//	//모든 디스플레이 모드에 대해 화면 너비 / 높이에 맞는 디스플레이 모드 찾기.
//	//적합한 것을 찾으면 모니터 새로고침 비율의 분모와 분자값을 저장.
//	for (UINT i = 0; i < numModes; ++i)
//	{
//		if ((displayModeList[i].Width == (UINT)_sizeX) && (displayModeList[i].Height == (UINT)_sizeY))
//		{
//			numerator = displayModeList[i].RefreshRate.Numerator;
//			denomiator = displayModeList[i].RefreshRate.Denominator;
//		}
//	}
//
//	//어댑터(그래픽 카드)의 desciption을 가져온다.
//	DXGI_ADAPTER_DESC adapterDesc;
//	if (FAILED(adapter->GetDesc(&adapterDesc)))
//		return FALSE;
//
//	//현재 그래픽 카드 메모리 용량을 메가바이트 단위로 저장
//	m_vidioCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1204);
//
//	//그래픽카드의 이름을 char형 문자열 배열로 바꾼 뒤 저장.
//	error = wcstombs_s(&stringLength, m_vidioCardDescription, 128, adapterDesc.Description, 128);
//	if (error != 0)
//		return FALSE;
//
//	//디스플레이 모드 리스트 할당 해제
//	delete[] displayModeList;
//	displayModeList = nullptr;
//
//	//출력 아답터 할당 해제
//	adapterOuput->Release();
//	adapterOuput = nullptr;
//
//	//아답터 할당 해제
//	adapter->Release();
//	adapter = nullptr;
//
//	//팩토리 객체를 할당 해제
//	factory->Release();
//	factory = nullptr;
//
//
//	//===========================================
//
//	//스왑체인 description을 초기화
//	DXGI_SWAP_CHAIN_DESC swapChainDesc;
//	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
//	
//	//하나의 백버퍼만을 사용하도록 함.
//	swapChainDesc.BufferCount = 1;
//	
//	//백버퍼의 너비와 높이 설정
//	swapChainDesc.BufferDesc.Width  = _sizeX;
//	swapChainDesc.BufferDesc.Height = _sizeY;
//
//	//백버퍼로 일반적 32bit surface 지정
//	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//
//	//백버퍼의 새로고침 비율을 설정
//	if (m_vsync_enabled)
//	{
//		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
//		swapChainDesc.BufferDesc.RefreshRate.Denominator = denomiator;
//	}
//	else
//	{
//		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
//		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
//	}
//
//	//백버퍼의 용도를 설정
//	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
//
//	//랜더링이 이루어질 윈도우 핸들 설정
//	swapChainDesc.OutputWindow = _hwnd;
//
//	//멀티 샘플링 끔
//	swapChainDesc.SampleDesc.Count = 1;
//	swapChainDesc.SampleDesc.Quality = 0;
//
//	//윈도우 모드 또는 풀스크린 모드 설정
//	if (_mode == WINDOW_MODE::MODE_FULL)
//	{
//		swapChainDesc.Windowed = FALSE;
//	}
//	else
//	{
//		swapChainDesc.Windowed = TRUE;
//	}
//
//	//스캔라인의 정렬과 스캔라이닝을 지정되지 않음으로(unspecified) 설정.
//	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
//	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
//
//	//출력된 이후 백버퍼의 내용을 버림
//	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
//
//	//추가 옵션 플래그를 사용하지 않음.
//	swapChainDesc.Flags = 0;
//	
//	//피쳐 레벨을 DirectX11로 설정
//	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
//	
//	//스왑체인, Direct3D Device, Driect3D deviceContex 설정
//	if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
//		D3D11_SDK_VERSION, &swapChainDesc, &m_pSwapChain, &m_pDevice, NULL, &m_pContext))
//		return FALSE;
//
//	//백버퍼 포인터 가져오기
//	ID3D11Texture2D* backBufferPtr;
//	if (FAILED(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr)))
//		return FALSE;
//	
//	//백버퍼의 포인터로 렌더타겟 뷰 생성
//	if (FAILED(m_pDevice->CreateRenderTargetView(backBufferPtr, NULL, &m_pRenderTargetView)))
//		return FALSE;
//
//	//백버퍼 포인터를 더 이상 사용하지 않아 할당 해제
//	backBufferPtr->Release();
//	backBufferPtr = nullptr;
//
//	//깊이 버퍼의 description 초기화
//	D3D11_TEXTURE2D_DESC depthBufferDesc;
//	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
//
//	//깊이 버퍼 description 작성
//	depthBufferDesc.Width = _sizeX;
//	depthBufferDesc.Height = _sizeY;
//	depthBufferDesc.MipLevels = 1;
//	depthBufferDesc.ArraySize = 1;
//	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
//	depthBufferDesc.SampleDesc.Count = 1;
//	depthBufferDesc.SampleDesc.Quality = 0;
//	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
//	depthBufferDesc.CPUAccessFlags = 0;
//	depthBufferDesc.MiscFlags = 0;
//	
//	//description을 사용해 깊이 버퍼 텍스쳐 생성
//	if (FAILED(m_pDevice->CreateTexture2D(&depthBufferDesc, NULL, &m_pDepthStencilBuffer)))
//		return FALSE;
//	
//	
//	
//	//스텐실 상태의 description 초기화
//	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
//	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
//	
//	//스텐실 상태의 description 작성
//	depthStencilDesc.DepthEnable = TRUE;
//	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
//	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
//
//	depthStencilDesc.StencilEnable = TRUE;
//	depthStencilDesc.StencilReadMask = 0xFF;
//	depthStencilDesc.StencilWriteMask = 0xFF;
//
//	//Stencil operations if pixel is front-facing.
//	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
//	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
//	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
//	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
//
//	//stencil operations if pixel is back-facing;
//	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
//	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
//	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
//	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
//
//	//깊이-스탠실 상태 생성
//	if (FAILED(m_pDevice->CreateDepthStencilState(&depthStencilDesc, &m_pDepthStencilState)))
//		return FALSE;
//
//	//깊이-스텐실 버퍼의 뷰에 대한 description
//	//이 작업을 수행해야 Direct3D가 깊이 버퍼를 깊이-스텐실 텍스쳐로 인식함.
//
//	//깊이-스텐실 뷰의 Dscription 초기화
//	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
//	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
//
//	//깊이-스텐실 뷰의 description 작성
//	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
//	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
//	depthStencilViewDesc.Texture2D.MipSlice = 0;
//
//	//깊이-스텐실 뷰 생성
//	if (FAILED(m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView)))
//		return FALSE;
//
//	//렌더타겟 뷰와 깊이 스텐실 버퍼를 가각 출력 파이프 라인에 바인딩
//	//이를 통해 파이프라인을 이용한 렌더링 수행될 때, 만들었던 백버퍼에 장면이 그려지게 된다.
//	//그리고 백버퍼에 그려진 것을 프론트 버퍼와 바꿔치기하여 유저 모니터에 출력함.
//	m_pContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
//	
//	//어떤 도형을 어떻게 그릴 것인가를 결정하는 레스터화기 Description 작성
//	D3D11_RASTERIZER_DESC rasterDesc;
//	rasterDesc.AntialiasedLineEnable = FALSE;
//	rasterDesc.CullMode = D3D11_CULL_BACK;
//	rasterDesc.DepthBias = 0;
//	rasterDesc.DepthBiasClamp = 0.0f;
//	rasterDesc.DepthClipEnable = TRUE;
//	rasterDesc.FillMode = D3D11_FILL_SOLID;
//	rasterDesc.FrontCounterClockwise = FALSE;
//	rasterDesc.MultisampleEnable = FALSE;
//	rasterDesc.ScissorEnable = FALSE;
//	rasterDesc.SlopeScaledDepthBias = 0.0f;
//
//	//작성한 Description 으로부터 래지터화기를 생성
//	if (FAILED(m_pDevice->CreateRasterizerState(&rasterDesc, &m_pRasterState)))
//		return FALSE;
//
//
//	//랜더링을 위한 뷰포트 설정
//	//뷰포트가 있어야 랜더타겟 공간에서 클리핑이 가능함. 
//	D3D11_VIEWPORT viewport;
//	viewport.Width = (float)_sizeX;
//	viewport.Height = (float)_sizeY;
//	viewport.MinDepth = 0.0f;
//	viewport.MaxDepth = 1.0f;
//	viewport.TopLeftX = 0.0f;
//	viewport.TopLeftY = 0.0f;
//
//	//뷰포트 생성
//	m_pContext->RSSetViewports(1, &viewport);
//	
//
//	return TRUE;
//}
//
//void CGraphicDev::ShutDown(void)
//{
//	if (m_pSwapChain)
//		m_pSwapChain->SetFullscreenState(FALSE, NULL);
//
//	if (m_pRasterState)
//	{
//		m_pRasterState->Release();
//		m_pRasterState = nullptr;
//	}
//
//	if (m_pDepthStencilView)
//	{
//		m_pDepthStencilView->Release();
//		m_pDepthStencilView = nullptr;
//	}
//
//	if (m_pDepthStencilState)
//	{
//		m_pDepthStencilState->Release();
//		m_pDepthStencilState = nullptr;
//	}
//
//	if (m_pDepthStencilBuffer)
//	{
//		m_pDepthStencilBuffer->Release();
//		m_pDepthStencilBuffer = nullptr;
//	}
//
//	if (m_pRenderTargetView)
//	{
//		m_pRenderTargetView->Release();
//		m_pRenderTargetView = nullptr;
//	}
//
//	if (m_pContext)
//	{
//		m_pContext->Release();
//		m_pContext = nullptr;
//	}
//
//	if (m_pDevice)
//	{
//		m_pDevice->Release();
//		m_pDevice = nullptr;
//	}
//
//	if (m_pSwapChain)
//	{
//		m_pSwapChain->Release();
//		m_pSwapChain = nullptr;
//	}
//
//	return;
//}

bool CGraphicDev::Init_Graphic(void)
{
#if defined(DEBUG) || defined (_DEBUG)
	//D3D12 Debug Layer 활성화
	Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
	ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
	debugController->EnableDebugLayer();
#endif

	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&m_Factory)));

	//하드웨어 어댑터를 나타내는 장치 생성
	HRESULT hardwareResult = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device));

	//실패하면 WARP 어댑터를 나타내는 장치 생성
	if (FAILED(hardwareResult))
	{
		Microsoft::WRL::ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(m_Factory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));

		ThrowIfFailed(D3D12CreateDevice(pWarpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device)));
	}

	ThrowIfFailed(m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));

	m_iRtvDescriptiorSize		= m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_iDsvDescriptiorSize		= m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	m_CbvSrvUavDescriptorSize	= m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = m_BackBufferFormat;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;
	ThrowIfFailed(m_Device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLevels, sizeof(msQualityLevels)));

	m_4xMsaaQuality = msQualityLevels.NumQualityLevels;
	assert(m_4xMsaaQuality > 0 && "Unexpected MSAA quality level.");

#ifdef _DEBUG
	LogAdapters();
#endif

	CreateCommandObjects();
	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();

	return TRUE;
}

void CGraphicDev::LogAdapters(void)
{
	IDXGIAdapter* adapter = nullptr;
	std::vector<IDXGIAdapter*>	vecAdapter;

	UINT i = 0;
	while (m_Factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC	desc;
		adapter->GetDesc(&desc);

		std::wstring text = L"***Adapter : ";
		text += desc.Description;
		text += L"\n";

		OutputDebugString(text.c_str());

		vecAdapter.push_back(adapter);

		++i;
	}

	for (size_t i = 0; i < vecAdapter.size(); ++i)
	{
		LogAdapterOutputs(vecAdapter[i]);
		ReleaseCom(vecAdapter[i]);
	}
}

void CGraphicDev::LogAdapterOutputs(IDXGIAdapter * adapter)
{
	UINT i = 0;
	IDXGIOutput* output = nullptr;

	while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_OUTPUT_DESC desc;
		output->GetDesc(&desc);

		std::wstring text = L"***Output : ";
		text += desc.DeviceName;
		text += L"\n";

		OutputDebugString(text.c_str());

		LogOutputDisplayModes(output, m_BackBufferFormat);

		ReleaseCom(output);

		++i;
	}
}

void CGraphicDev::LogOutputDisplayModes(IDXGIOutput * output, DXGI_FORMAT format)
{
	UINT count = 0, flags = 0;

	//nullptr을 인수로 해서 호출하면 목록의 크기(모드 개수)를 얻음
	output->GetDisplayModeList(format, flags, &count, nullptr);

	std::vector<DXGI_MODE_DESC> modeList(count);
	output->GetDisplayModeList(format, flags, &count, &modeList[0]);

	for (auto& x : modeList)
	{
		UINT n = x.RefreshRate.Numerator;
		UINT d = x.RefreshRate.Denominator;
		
		std::wstring text =
			L"Width = " + std::to_wstring(x.Width) + L" " +
			L"Height = " + std::to_wstring(x.Height) + L" " +
			L"Refresh = " + std::to_wstring(n) + L"/" + std::to_wstring(d) +
			L"\n";

		::OutputDebugString(text.c_str());
	}
}

//버퍼를 빈값으로 초기화하고 렌더링이 이루어지도록 준비
//매 프레임의 시작마다 3D 화면을 그리기 시작할때 호출
void CGraphicDev::BeginScene(const float _r, const float _g, const float _b, const float _alpha)
{
	float color[4] = { 0.0f, };

	//버퍼의 색상을 정의
	color[0] = _r;
	color[1] = _g;
	color[2] = _b;
	color[3] = _alpha;

	//백버퍼 내용 삭제
	m_pContext->ClearRenderTargetView(m_pRenderTargetView, color);

	//깊이 버퍼 삭제
	m_pContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

//매 프레임 마지막마다 스왑체인에게 백버퍼에 그린 3D화면을 표시하도록 하는 함수
void CGraphicDev::EndScene(void)
{
	//렌더링이 완료되었으므로 백버퍼의 내용을 화면에 표시
	if (m_vsync_enabled)
	{
		//새로고침 비율 고정
		m_pSwapChain->Present(1, 0);
	}
	else
	{
		//가능한 빠르게 표시
		m_pSwapChain->Present(0, 0);
	}

	return;
}

//Create Command Queue, List
void CGraphicDev::CreateCommandObjects(void)
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ThrowIfFailed(m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CommandQueue)));

	ThrowIfFailed(m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_CommandAllocator.GetAddressOf())));

																				//Associated Command Allocator, Beginning Pipeline State Object
	ThrowIfFailed(m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator.Get(), nullptr, IID_PPV_ARGS(m_CommandList.GetAddressOf())));

	m_CommandList->Close();
}

void CGraphicDev::CreateSwapChain(void)
{
	m_SwapChain.Reset();

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = WINSIZE_X;
	sd.BufferDesc.Height = WINSIZE_Y;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = m_BackBufferFormat;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = m_4xMsaaState ? 4 : 1;
	sd.SampleDesc.Quality = m_4xMsaaQuality ? (m_4xMsaaQuality - 1) : 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = m_iSwapChainBufferCount;
	sd.OutputWindow = g_hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	//Swap chain uses Command queue to perform flush
	ThrowIfFailed(m_Factory->CreateSwapChain(m_CommandQueue.Get(), &sd, m_SwapChain.GetAddressOf()));
}

void CGraphicDev::CreateRtvAndDsvDescriptorHeaps(void)
{
}
