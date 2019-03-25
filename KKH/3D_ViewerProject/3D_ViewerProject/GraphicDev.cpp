#include "stdafx.h"
#include "GraphicDev.h"

CGraphicDev::CGraphicDev(void)
	: m_pDevice(nullptr)
	, m_pContext(nullptr)
	, m_pSwapChain(nullptr)
	, m_pRenderTargetView(nullptr)
	, m_pDepthStencilState(nullptr)
	, m_pDepthStencilView(nullptr)
	, m_pDepthStencilBuffer(nullptr)
	, m_pRasterState(nullptr)
{
}

CGraphicDev::~CGraphicDev(void)
{
}

bool CGraphicDev::Init_Graphic(const WINDOW_MODE _mode, const HWND _hwnd, const WORD & _sizeX, const WORD & _sizeY, const BOOL _bMSAA, const float _screedepth, const float _screennear)
{
	UINT numModes = 0, numerator = 0, denomiator = 0, stringLength = 0, error = 0;
	float fieldOfView = 0.0f, screenAspect = 0.0f;

	//DirectX 그래픽 인터페이스 팩토리 생성
	IDXGIFactory* factory;
	if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory)))
		return FALSE;

	//팩토리 객체를 이용해 첫번째 그래픽 카드 인터페이스에 대한 어댑터 생성.
	IDXGIAdapter* adapter;
	if (FAILED(factory->EnumAdapters(0, &adapter)))
		return FALSE;

	//출력(모니터)에 대한 첫 아답터 나열.
	IDXGIOutput* adapterOuput;
	if (FAILED(adapter->EnumOutputs(0, &adapterOuput)))
		return FALSE;
	
	//DXGI_FORMAT_R8G8B8A8_UNORM 모니터 출력 디스플레이 포멧에 맞는 모드의 개수를 구함.
	if (FAILED(adapterOuput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL)))
		return FALSE;

	//가능한 모든 모니터와 그래픽카드 조합을 저장할 리스트 생성.
	DXGI_MODE_DESC*	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
		return FALSE;

	//디스플레이 모드에 대한 리스트 구조를 채움.
	if (FAILED(adapterOuput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList)))
		return FALSE;


	//모든 디스플레이 모드에 대해 화면 너비 / 높이에 맞는 디스플레이 모드 찾기.
	//적합한 것을 찾으면 모니터 새로고침 비율의 분모와 분자값을 저장.
	for (int i = 0; i < numModes; ++i)
	{
		if ((displayModeList[i].Width == (UINT)_sizeX) && (displayModeList[i].Height == (UINT)_sizeY))
		{
			numerator = displayModeList[i].RefreshRate.Numerator;
			denomiator = displayModeList[i].RefreshRate.Denominator;
		}
	}

	//어댑터(그래픽 카드)의 desciption을 가져온다.
	DXGI_ADAPTER_DESC adapterDesc;
	if (FAILED(adapter->GetDesc(&adapterDesc)))
		return FALSE;

	//현재 그래픽 카드 메모리 용량을 메가바이트 단위로 저장
	m_vidioCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1204);

	//그래픽카드의 이름을 char형 문자열 배열로 바꾼 뒤 저장.
	error = wcstombs_s(&stringLength, m_vidioCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
		return FALSE;

	//디스플레이 모드 리스트 할당 해제
	delete[] displayModeList;
	displayModeList = nullptr;

	//출력 아답터 할당 해제
	adapterOuput->Release();
	adapterOuput = nullptr;

	//아답터 할당 해제
	adapter->Release();
	adapter = nullptr;

	//팩토리 객체를 할당 해제
	factory->Release();
	factory = nullptr;


	//===========================================

	//스왑체인 description을 초기화
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	
	//하나의 백버퍼만을 사용하도록 함.
	swapChainDesc.BufferCount = 1;
	
	//백버퍼의 너비와 높이 설정
	swapChainDesc.BufferDesc.Width  = _sizeX;
	swapChainDesc.BufferDesc.Height = _sizeY;

	//백버퍼로 일반적 32bit surface 지정
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	//백버퍼의 새로고침 비율을 설정
	if (m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denomiator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	//백버퍼의 용도를 설정
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	//랜더링이 이루어질 윈도우 핸들 설정
	swapChainDesc.OutputWindow = _hwnd;

	//멀티 샘플링 끔
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	//윈도우 모드 또는 풀스크린 모드 설정
	if (_mode == WINDOW_MODE::MODE_FULL)
	{
		swapChainDesc.Windowed = FALSE;
	}
	else
	{
		swapChainDesc.Windowed = TRUE;
	}

	//스캔라인의 정렬과 스캔라이닝을 지정되지 않음으로(unspecified) 설정.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//출력된 이후 백버퍼의 내용을 버림
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	//추가 옵션 플래그를 사용하지 않음.
	swapChainDesc.Flags = 0;
	
	//피쳐 레벨을 DirectX11로 설정
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	
	//스왑체인, Direct3D Device, Driect3D deviceContex 설정
	if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &m_pSwapChain, &m_pDevice, NULL, &m_pContext))
		return FALSE;

	//백버퍼 포인터 가져오기
	ID3D11Texture2D* backBufferPtr;
	if (FAILED(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr)))
		return FALSE;
	
	//백버퍼의 포인터로 렌더타겟 뷰 생성
	if (FAILED(m_pDevice->CreateRenderTargetView(backBufferPtr, NULL, &m_pRenderTargetView)))
		return FALSE;

	//백버퍼 포인터를 더 이상 사용하지 않아 할당 해제
	backBufferPtr->Release();
	backBufferPtr = nullptr;

	//깊이 버퍼의 description 초기화
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	//깊이 버퍼 description 작성
	depthBufferDesc.Width = _sizeX;
	depthBufferDesc.Height = _sizeY;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;
	
	//description을 사용해 깊이 버퍼 텍스쳐 생성
	if (FAILED(m_pDevice->CreateTexture2D(&depthBufferDesc, NULL, &m_pDepthStencilBuffer)))
		return FALSE;
	
	
	
	//스텐실 상태의 description 초기화
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	
	//스텐실 상태의 description 작성
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = TRUE;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	//Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//stencil operations if pixel is back-facing;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//깊이-스탠실 상태 생성
	if (FAILED(m_pDevice->CreateDepthStencilState(&depthStencilDesc, &m_pDepthStencilState)))
		return FALSE;

	//깊이-스텐실 버퍼의 뷰에 대한 description
	//이 작업을 수행해야 Direct3D가 깊이 버퍼를 깊이-스텐실 텍스쳐로 인식함.

	//깊이-스텐실 뷰의 Dscription 초기화
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	//깊이-스텐실 뷰의 description 작성
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	//깊이-스텐실 뷰 생성
	if (FAILED(m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView)))
		return FALSE;

	//렌더타겟 뷰와 깊이 스텐실 버퍼를 가각 출력 파이프 라인에 바인딩
	//이를 통해 파이프라인을 이용한 렌더링 수행될 때, 만들었던 백버퍼에 장면이 그려지게 된다.
	//그리고 백버퍼에 그려진 것을 프론트 버퍼와 바꿔치기하여 유저 모니터에 출력함.
	m_pContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
	
	
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_VIEWPORT viewport;
	

	return TRUE;
}

void CGraphicDev::ShutDown(void)
{
}

void CGraphicDev::BeginScene(const float _r, const float _g, const float _b, const float _alpha)
{
}

void CGraphicDev::EndScene(void)
{
}
