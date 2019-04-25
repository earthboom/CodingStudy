#include "stdafx.h"
#include "Graphic_Manager.h"
#include "Function.h"

CGraphic_Manager::CGraphic_Manager(void)
	: m_pGraphic(nullptr)
{
}

CGraphic_Manager::~CGraphic_Manager(void)
{
}

bool CGraphic_Manager::Init_Graphic(void)
{
	m_pGraphic = std::make_shared<CGraphicDev>();
	if (!m_pGraphic)	return FALSE;

	if (!m_pGraphic->Init_Graphic())
		return FALSE;

	m_pGraphic->OnResize();

	return TRUE;
}

void CGraphic_Manager::Draw(const float & dt)
{
	//Reuse the memory associated with command recording.
	//We can only reset when the associated command list have finished execution on the GPU.
	ThrowIfFailed(m_pGraphic->Get_CommandAllocator()->Reset());

	//A command list can be reset after it has been added to the command queue via ExecutecommandList.
	//Reusing the command list reuses moemory.
	ThrowIfFailed(m_pGraphic->Get_CommandList()->Reset(m_pGraphic->Get_CommandAllocator().Get(), nullptr));

	//Indicate a state transition on the resource usage.
	m_pGraphic->Get_CommandList()->ResourceBarrier(
		1, &CD3DX12_RESOURCE_BARRIER::Transition(
			m_pGraphic->Get_CurrentBackBuffer_Resource(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET));

	//Set the viewport and scissor rect.
	//This needs to be reset whenever the command list is reset.
	m_pGraphic->Get_CommandList()->RSSetViewports(1, &m_pGraphic->Get_ScreenViewport());
	m_pGraphic->Get_CommandList()->RSSetScissorRects(1, &m_pGraphic->Get_ScissorRect());

	//Clear the back buffer and depth buffer
	m_pGraphic->Get_CommandList()->ClearRenderTargetView(
		m_pGraphic->Get_CurrentBackBufferView_Handle(),
		DirectX::Colors::LightSteelBlue, 0, nullptr);

	m_pGraphic->Get_CommandList()->ClearDepthStencilView(
		m_pGraphic->Get_DepthStencilView_Handle(),
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
		1.0f, 0, 0, nullptr);

	//Specify the buffers we are going to render to.
	m_pGraphic->Get_CommandList()->OMSetRenderTargets(
		1, &m_pGraphic->Get_CurrentBackBufferView_Handle(), TRUE, &m_pGraphic->Get_DepthStencilView_Handle());

	//Indicate a state transition on the resource usage.
	m_pGraphic->Get_CommandList()->ResourceBarrier(
		1, &CD3DX12_RESOURCE_BARRIER::Transition(
			m_pGraphic->Get_CurrentBackBuffer_Resource(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT));

	//Done recording commands.
	ThrowIfFailed(m_pGraphic->Get_CommandList()->Close());

	//Add the command list to the queue for execution.
	ID3D12CommandList* cmdsLists[] = { m_pGraphic->Get_CommandList().Get() };
	m_pGraphic->Get_CommandQueue()->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	//swap the back and front buffers.
	ThrowIfFailed(m_pGraphic->Get_SwapChain()->Present(0, 0));
	m_pGraphic->Get_Set_CurrBackBuffer() = (m_pGraphic->Get_Set_CurrBackBuffer() + 1) % m_pGraphic->Get_SwapChainBufferCount();

	//Wait until frame commands are complete.
	//This waiting is inefficient and is done for simplicity.
	//Later we will show how to organize our rendering code so we do not have to wait per frame.
	m_pGraphic->FlushCommandQueue();
}