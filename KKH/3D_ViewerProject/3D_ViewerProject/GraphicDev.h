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
	bool Init_Graphic(const WINDOW_MODE _mode, const HWND _hwnd, const WORD& _sizeX, const WORD& _sizeY, const BOOL _bMSAA, const float _screedepth = SCREEN_DEPTH, const float _screennear = SCREEN_NEAR);
	void ShutDown(void);

	void BeginScene(const float _r, const float _g, const float _b, const float _alpha);
	void EndScene(void);

private:
	ID3D11Device*				m_pDevice;
	ID3D11DeviceContext*		m_pContext;
	IDXGISwapChain*				m_pSwapChain;

	ID3D11RenderTargetView*		m_pRenderTargetView;
	ID3D11DepthStencilState*	m_pDepthStencilState;
	ID3D11DepthStencilView*		m_pDepthStencilView;
	ID3D11Texture2D*			m_pDepthStencilBuffer;

	ID3D11RasterizerState*		m_pRasterState;

	D3D11_VIEWPORT				m_pViewport;


	bool	m_vsync_enabled;
	int		m_vidioCardMemory;
	char	m_vidioCardDescription[128];
};

typedef std::shared_ptr<CGraphicDev>	PGRAPHIC;