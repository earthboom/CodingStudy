#pragma once

#include "Common\StepTimer.h"
#include "Common\DeviceResources.h"
#include "Content\Renderer.h"

extern std::unique_ptr<ECS_tool::Renderer> g_sceneRenderer;

#define RENDER_ITEM			g_sceneRenderer->Get_vecRenderItem()
#define COMMANDLIST			g_sceneRenderer->Get_commandList()
#define PIPELINESTATE		g_sceneRenderer->Get_pipelineState()
#define MAPEDCONSTANTBUFFER	g_sceneRenderer->Get_vecMappedConstantBuffer()
#define CONSTANTBUFFERDATA	g_sceneRenderer->Get_constantBufferData()
#define CBV_HEAP			g_sceneRenderer->Get_cbvHeap()
#define CBV_DES_SIZE		g_sceneRenderer->Get_cbvDescriptorSize()

// Renders Direct3D content on the screen.
namespace ECS_tool
{
	class ECS_toolMain
	{
	public:
		ECS_toolMain();
		void CreateRenderers(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void Update();
		bool Render();

		void OnWindowSizeChanged();
		void OnSuspending();
		void OnResuming();
		void OnDeviceRemoved();

	private:
		// TODO: Replace with your own content renderers.
		//std::unique_ptr<Renderer> m_sceneRenderer;

		// Rendering loop timer.
		DX::StepTimer m_timer;
	};
}