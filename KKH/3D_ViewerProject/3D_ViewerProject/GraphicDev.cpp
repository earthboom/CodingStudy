#include "stdafx.h"
#include "GraphicDev.h"

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
//	//DirectX �׷��� �������̽� ���丮 ����
//	IDXGIFactory* factory;
//	if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory)))
//		return FALSE;
//
//	//���丮 ��ü�� �̿��� ù��° �׷��� ī�� �������̽��� ���� ����� ����.
//	IDXGIAdapter* adapter;
//	if (FAILED(factory->EnumAdapters(0, &adapter)))
//		return FALSE;
//
//	//���(�����)�� ���� ù �ƴ��� ����.
//	IDXGIOutput* adapterOuput;
//	if (FAILED(adapter->EnumOutputs(0, &adapterOuput)))
//		return FALSE;
//	
//	//DXGI_FORMAT_R8G8B8A8_UNORM ����� ��� ���÷��� ���信 �´� ����� ������ ����.
//	if (FAILED(adapterOuput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL)))
//		return FALSE;
//
//	//������ ��� ����Ϳ� �׷���ī�� ������ ������ ����Ʈ ����.
//	DXGI_MODE_DESC*	displayModeList = new DXGI_MODE_DESC[numModes];
//	if (!displayModeList)
//		return FALSE;
//
//	//���÷��� ��忡 ���� ����Ʈ ������ ä��.
//	if (FAILED(adapterOuput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList)))
//		return FALSE;
//
//
//	//��� ���÷��� ��忡 ���� ȭ�� �ʺ� / ���̿� �´� ���÷��� ��� ã��.
//	//������ ���� ã���� ����� ���ΰ�ħ ������ �и�� ���ڰ��� ����.
//	for (UINT i = 0; i < numModes; ++i)
//	{
//		if ((displayModeList[i].Width == (UINT)_sizeX) && (displayModeList[i].Height == (UINT)_sizeY))
//		{
//			numerator = displayModeList[i].RefreshRate.Numerator;
//			denomiator = displayModeList[i].RefreshRate.Denominator;
//		}
//	}
//
//	//�����(�׷��� ī��)�� desciption�� �����´�.
//	DXGI_ADAPTER_DESC adapterDesc;
//	if (FAILED(adapter->GetDesc(&adapterDesc)))
//		return FALSE;
//
//	//���� �׷��� ī�� �޸� �뷮�� �ް�����Ʈ ������ ����
//	m_vidioCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1204);
//
//	//�׷���ī���� �̸��� char�� ���ڿ� �迭�� �ٲ� �� ����.
//	error = wcstombs_s(&stringLength, m_vidioCardDescription, 128, adapterDesc.Description, 128);
//	if (error != 0)
//		return FALSE;
//
//	//���÷��� ��� ����Ʈ �Ҵ� ����
//	delete[] displayModeList;
//	displayModeList = nullptr;
//
//	//��� �ƴ��� �Ҵ� ����
//	adapterOuput->Release();
//	adapterOuput = nullptr;
//
//	//�ƴ��� �Ҵ� ����
//	adapter->Release();
//	adapter = nullptr;
//
//	//���丮 ��ü�� �Ҵ� ����
//	factory->Release();
//	factory = nullptr;
//
//
//	//===========================================
//
//	//����ü�� description�� �ʱ�ȭ
//	DXGI_SWAP_CHAIN_DESC swapChainDesc;
//	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
//	
//	//�ϳ��� ����۸��� ����ϵ��� ��.
//	swapChainDesc.BufferCount = 1;
//	
//	//������� �ʺ�� ���� ����
//	swapChainDesc.BufferDesc.Width  = _sizeX;
//	swapChainDesc.BufferDesc.Height = _sizeY;
//
//	//����۷� �Ϲ��� 32bit surface ����
//	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//
//	//������� ���ΰ�ħ ������ ����
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
//	//������� �뵵�� ����
//	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
//
//	//�������� �̷���� ������ �ڵ� ����
//	swapChainDesc.OutputWindow = _hwnd;
//
//	//��Ƽ ���ø� ��
//	swapChainDesc.SampleDesc.Count = 1;
//	swapChainDesc.SampleDesc.Quality = 0;
//
//	//������ ��� �Ǵ� Ǯ��ũ�� ��� ����
//	if (_mode == WINDOW_MODE::MODE_FULL)
//	{
//		swapChainDesc.Windowed = FALSE;
//	}
//	else
//	{
//		swapChainDesc.Windowed = TRUE;
//	}
//
//	//��ĵ������ ���İ� ��ĵ���̴��� �������� ��������(unspecified) ����.
//	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
//	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
//
//	//��µ� ���� ������� ������ ����
//	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
//
//	//�߰� �ɼ� �÷��׸� ������� ����.
//	swapChainDesc.Flags = 0;
//	
//	//���� ������ DirectX11�� ����
//	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
//	
//	//����ü��, Direct3D Device, Driect3D deviceContex ����
//	if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
//		D3D11_SDK_VERSION, &swapChainDesc, &m_pSwapChain, &m_pDevice, NULL, &m_pContext))
//		return FALSE;
//
//	//����� ������ ��������
//	ID3D11Texture2D* backBufferPtr;
//	if (FAILED(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr)))
//		return FALSE;
//	
//	//������� �����ͷ� ����Ÿ�� �� ����
//	if (FAILED(m_pDevice->CreateRenderTargetView(backBufferPtr, NULL, &m_pRenderTargetView)))
//		return FALSE;
//
//	//����� �����͸� �� �̻� ������� �ʾ� �Ҵ� ����
//	backBufferPtr->Release();
//	backBufferPtr = nullptr;
//
//	//���� ������ description �ʱ�ȭ
//	D3D11_TEXTURE2D_DESC depthBufferDesc;
//	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
//
//	//���� ���� description �ۼ�
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
//	//description�� ����� ���� ���� �ؽ��� ����
//	if (FAILED(m_pDevice->CreateTexture2D(&depthBufferDesc, NULL, &m_pDepthStencilBuffer)))
//		return FALSE;
//	
//	
//	
//	//���ٽ� ������ description �ʱ�ȭ
//	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
//	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
//	
//	//���ٽ� ������ description �ۼ�
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
//	//����-���Ľ� ���� ����
//	if (FAILED(m_pDevice->CreateDepthStencilState(&depthStencilDesc, &m_pDepthStencilState)))
//		return FALSE;
//
//	//����-���ٽ� ������ �信 ���� description
//	//�� �۾��� �����ؾ� Direct3D�� ���� ���۸� ����-���ٽ� �ؽ��ķ� �ν���.
//
//	//����-���ٽ� ���� Dscription �ʱ�ȭ
//	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
//	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
//
//	//����-���ٽ� ���� description �ۼ�
//	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
//	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
//	depthStencilViewDesc.Texture2D.MipSlice = 0;
//
//	//����-���ٽ� �� ����
//	if (FAILED(m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView)))
//		return FALSE;
//
//	//����Ÿ�� ��� ���� ���ٽ� ���۸� ���� ��� ������ ���ο� ���ε�
//	//�̸� ���� ������������ �̿��� ������ ����� ��, ������� ����ۿ� ����� �׷����� �ȴ�.
//	//�׸��� ����ۿ� �׷��� ���� ����Ʈ ���ۿ� �ٲ�ġ���Ͽ� ���� ����Ϳ� �����.
//	m_pContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
//	
//	//� ������ ��� �׸� ���ΰ��� �����ϴ� ������ȭ�� Description �ۼ�
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
//	//�ۼ��� Description ���κ��� ������ȭ�⸦ ����
//	if (FAILED(m_pDevice->CreateRasterizerState(&rasterDesc, &m_pRasterState)))
//		return FALSE;
//
//
//	//�������� ���� ����Ʈ ����
//	//����Ʈ�� �־�� ����Ÿ�� �������� Ŭ������ ������. 
//	D3D11_VIEWPORT viewport;
//	viewport.Width = (float)_sizeX;
//	viewport.Height = (float)_sizeY;
//	viewport.MinDepth = 0.0f;
//	viewport.MaxDepth = 1.0f;
//	viewport.TopLeftX = 0.0f;
//	viewport.TopLeftY = 0.0f;
//
//	//����Ʈ ����
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

void CGraphicDev::LogAdaptor(void)
{
	IDXGIAdapter* adapter = nullptr;
	std::vector<IDXGIAdapter*>	vecAdaptor;

	UINT i = 0;
	while (m_Factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC	desc;
		adapter->GetDesc(&desc);

		std::wstring text = L"***Adapter : ";
		text += desc.Description;
		text += L"\n";

		OutputDebugString(text.c_str());

		vecAdaptor.push_back(adapter);

		++i;
	}

	for (size_t i = 0; i < vecAdaptor.size(); ++i)
	{
		LogAdapterOutputs(vecAdapter[i]);
		ReleaseCom(vecAdaptor[i]);
	}
}

//���۸� ������ �ʱ�ȭ�ϰ� �������� �̷�������� �غ�
//�� �������� ���۸��� 3D ȭ���� �׸��� �����Ҷ� ȣ��
void CGraphicDev::BeginScene(const float _r, const float _g, const float _b, const float _alpha)
{
	float color[4] = { 0.0f, };

	//������ ������ ����
	color[0] = _r;
	color[1] = _g;
	color[2] = _b;
	color[3] = _alpha;

	//����� ���� ����
	m_pContext->ClearRenderTargetView(m_pRenderTargetView, color);

	//���� ���� ����
	m_pContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

//�� ������ ���������� ����ü�ο��� ����ۿ� �׸� 3Dȭ���� ǥ���ϵ��� �ϴ� �Լ�
void CGraphicDev::EndScene(void)
{
	//�������� �Ϸ�Ǿ����Ƿ� ������� ������ ȭ�鿡 ǥ��
	if (m_vsync_enabled)
	{
		//���ΰ�ħ ���� ����
		m_pSwapChain->Present(1, 0);
	}
	else
	{
		//������ ������ ǥ��
		m_pSwapChain->Present(0, 0);
	}

	return;
}
