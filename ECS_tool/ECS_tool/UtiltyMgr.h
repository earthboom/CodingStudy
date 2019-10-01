#pragma once

#include "Common\DeviceResources.h"
#include "Content\ShaderStructures.h"
#include "Common\StepTimer.h"

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
		void Object_Render(DX::StepTimer const& timer);

	public:
		void CreateDeviceDependentResources(void);
		void CreateWindowSizeDependentResources(void);

	private:
		void LoadState(void);
		void Rotate(float radians);

	public:
		void SetDeviceResources(const std::shared_ptr<DX::DeviceResources>& _res) { m_deviceResources = _res; }

	private:
		static const UINT c_alignedConstantBufferSize = (sizeof(ModelViewProjectionConstantBuffer) + 255) & ~255;

		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		typedef std::vector<Object> VECOBJECT;
		VECOBJECT m_vecObject;

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>	m_commandList;
		Microsoft::WRL::ComPtr<ID3D12RootSignature>			m_rootSignature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState>			m_pipelineState;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>		m_cbvHeap;
		ModelViewProjectionConstantBuffer					m_constantBufferData;
		std::vector<BYTE>									m_vertexShader;
		std::vector<BYTE>									m_pixelShader;

		float	m_fAngle;
		bool	m_bTracking;
	};
}
