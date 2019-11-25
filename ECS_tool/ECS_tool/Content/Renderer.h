#pragma once

#include "..\Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "..\Common\StepTimer.h"

#include "Object.h"

namespace ECS_tool
{
	// This sample renderer instantiates a basic rendering pipeline.
	class Renderer
	{
	public:
		Renderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		~Renderer();
		void CreateObject(void);
		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void Update(DX::StepTimer const& timer);
		bool Render();
		void SaveState();

		void StartTracking();
		void TrackingUpdate(float positionX);
		void StopTracking();
		bool IsTracking() { return m_tracking; }

	private:
		void LoadState();
		void Rotate(float radians);

	private:
		// Constant buffers must be 256-byte aligned.
		static const UINT c_alignedConstantBufferSize = (sizeof(ModelViewProjectionConstantBuffer) + 255) & ~255;

		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// Direct3D resources for cube geometry.
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>	m_commandList;
		Microsoft::WRL::ComPtr<ID3D12RootSignature>			m_rootSignature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState>			m_pipelineState;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>		m_cbvHeap;
		Microsoft::WRL::ComPtr<ID3D12Resource>				m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource>				m_indexBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource>				m_constantBuffer;
		ModelViewProjectionConstantBuffer					m_constantBufferData;
		UINT8*												m_mappedConstantBuffer;
		UINT												m_cbvDescriptorSize;
		D3D12_RECT											m_scissorRect;
		std::vector<byte>									m_vertexShader;
		std::vector<byte>									m_pixelShader;
		//D3D12_VERTEX_BUFFER_VIEW							m_vertexBufferView;
		//D3D12_INDEX_BUFFER_VIEW								m_indexBufferView;
		
		std::vector<RenderItem*>							m_vecRenderItem;
		std::vector<POBJECT>								m_vecObject;
		
		std::vector<UINT8*>									m_vecMappedConstantBuffer;

		// Variables used with the rendering loop.
		bool	m_loadingComplete;
		float	m_radiansPerSecond;
		float	m_angle;
		bool	m_tracking;

	public:
		UINT& Get_cbvDescriptorSize(void) { return m_cbvDescriptorSize; }

		std::vector<RenderItem*>&	Get_vecRenderItem(void) { return m_vecRenderItem; }
		std::vector<UINT8*>& Get_vecMappedConstantBuffer(void) { return m_vecMappedConstantBuffer; }
				
		ModelViewProjectionConstantBuffer& Get_constantBufferData(void) { return m_constantBufferData; }

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>&		Get_commandList(void)	{ return m_commandList; }
		Microsoft::WRL::ComPtr<ID3D12PipelineState>&			Get_pipelineState(void)	{ return m_pipelineState; }
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>&			Get_cbvHeap(void)		{ return m_cbvHeap; }
	};
}

