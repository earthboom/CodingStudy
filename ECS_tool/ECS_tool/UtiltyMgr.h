#pragma once

#include "Common\DeviceResources.h"
#include "Content\ShaderStructures.h"
#include "Common\StepTimer.h"
#include "Common\DirectXHelper.h"

#include "Object.h"

#define UTIL ECS_tool::UtiltyMgr::GetInstance()

namespace ECS_tool
{
	class UtiltyMgr : public Singleton<UtiltyMgr>
	{
	public:
		explicit UtiltyMgr(void);
		UtiltyMgr(const UtiltyMgr&) = delete;
		UtiltyMgr& operator=(const UtiltyMgr&) = delete;
		~UtiltyMgr(void);

	public:
		void Object_Initialize(void);
		void Object_Update(DX::StepTimer const& timer);
		bool Object_Render(DX::StepTimer const& timer);

	public:
		void CreateDeviceDependentResources(void);
		void CreateWindowSizeDependentResources(void);
		void SaveState(void);
		void LoadState(void);
	
	private:		
		void Rotate(float radians);

	private:
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		typedef std::vector<std::shared_ptr<Object>> VECOBJECT;
		VECOBJECT m_vecObject;

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>	m_commandList;
		Microsoft::WRL::ComPtr<ID3D12RootSignature>			m_rootSignature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState>			m_pipelineState;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>		m_cbvHeap;
		ModelViewProjectionConstantBuffer					m_constantBufferData;
		std::vector<BYTE>									m_vertexShader;
		std::vector<BYTE>									m_pixelShader;
		D3D12_VERTEX_BUFFER_VIEW							m_vertexBufferView;
		D3D12_INDEX_BUFFER_VIEW								m_indexBufferView;
		D3D12_RECT											m_scissorRect;

		UINT8* m_mappedConstantBuffer;
		UINT	m_cbvDescriptorSize;

		float	m_fAngle;
		bool	m_bTracking;
		bool	m_bLoadingComplete;

	public:
		static const UINT c_alignedConstantBufferSize = (sizeof(ModelViewProjectionConstantBuffer) + 255) & ~255;

		VECOBJECT& GetVecObject(void) { return m_vecObject; }

	public:
		void SetDeviceResources(const std::shared_ptr<DX::DeviceResources>& _res) { m_deviceResources = _res; }
		std::shared_ptr<DX::DeviceResources>& GetDeviceResource(void) { return m_deviceResources; }

		Microsoft::WRL::ComPtr<ID3D12PipelineState>& GetPipelineState(void) { return m_pipelineState; }

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& GetCommandList(void) { return m_commandList; }

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& GetCbvHeap(void) { return m_cbvHeap; }

		ModelViewProjectionConstantBuffer& GetconstantBufferData(void) { return m_constantBufferData; }

		UINT& GetCbvDecriptorSize(void) { return m_cbvDescriptorSize; }
		UINT8*& GetmappedConstantBuffer(void) { return m_mappedConstantBuffer; }

		D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView(void) { return m_vertexBufferView; }
		D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView(void) { return m_indexBufferView; }
	};
}
