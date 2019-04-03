#include "stdafx.h"
#include "MainApp.h"
#include "Graphic_Manager.h"
#include "Function.h"

CMainApp::CMainApp(void)
	: m_fTime(0.0f)
{
}

CMainApp::~CMainApp(void)
{
	Free();
}

bool CMainApp::Ready_MainApp(void)
{
	//GRAPHIC_MGR.Init_Graphic(WINSIZE_X, WINSIZE_Y, g_hWnd);
	if (!GRAPHIC_MGR.Init_Graphic())
		return FALSE;

	return TRUE;
}

int CMainApp::Update_MainApp(const float & dt)
{
	return 0;
}

void CMainApp::Render_MainApp(const float& dt)
{
	//Reuse the memory associated with command recording.
	//We can only reset when the associated command list have finished execution on the GPU.
	ThrowIfFailed(GRAPHIC_MGR.Get_Graphic()->Get_CommandAllocator()->Reset());

	//A command list can be reset after it has been added to the command queue via ExecutecommandList.
	//Reusing the command list reuses moemory.
	ThrowIfFailed(GRAPHIC_MGR.Get_Graphic()->Get_CommandList()->Reset(GRAPHIC_MGR.Get_Graphic()->Get_CommandAllocator().Get(), nullptr));

	//Indicate a state transition on the resource usage.
	GRAPHIC_MGR.Get_Graphic()->Get_CommandList()->ResourceBarrier(
		1, &CD3DX12_RESOURCE_BARRIER::Transition(
			GRAPHIC_MGR.Get_Graphic()->Get_CurrentBackBuffer_Resource(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET));

	//Set the viewport and scissor rect.
	//This needs to be reset whenever the command list is reset.
	GRAPHIC_MGR.Get_Graphic()->Get_CommandList()->RSSetViewports(1, &GRAPHIC_MGR.Get_Graphic()->Get_ScreenViewport());
	GRAPHIC_MGR.Get_Graphic()->Get_CommandList()->RSSetScissorRects(1, &GRAPHIC_MGR.Get_Graphic()->Get_ScissorRect());

	//Clear the back buffer and depth buffer
	GRAPHIC_MGR.Get_Graphic()->Get_CommandList()->ClearRenderTargetView(
		GRAPHIC_MGR.Get_Graphic()->Get_CurrentBackBufferView_Handle(),
		DirectX::Colors::LightSteelBlue, 0, nullptr);

	GRAPHIC_MGR.Get_Graphic()->Get_CommandList()->ClearDepthStencilView(
		GRAPHIC_MGR.Get_Graphic()->Get_DepthStencilView_Handle(),
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
		1.0f, 0, 0, nullptr);

	//Specify the buffers we are going to render to.
	GRAPHIC_MGR.Get_Graphic()->Get_CommandList()->OMSetRenderTargets(
		1, &GRAPHIC_MGR.Get_Graphic()->Get_CurrentBackBufferView_Handle(), TRUE, &GRAPHIC_MGR.Get_Graphic()->Get_DepthStencilView_Handle());

	//Indicate a state transition on the resource usage.
	GRAPHIC_MGR.Get_Graphic()->Get_CommandList()->ResourceBarrier(
		1, &CD3DX12_RESOURCE_BARRIER::Transition(
			GRAPHIC_MGR.Get_Graphic()->Get_CurrentBackBuffer_Resource(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT));
}

void CMainApp::Free(void)
{
}
