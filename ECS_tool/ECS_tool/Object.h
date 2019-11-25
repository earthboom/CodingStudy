#pragma once

#include <Component.h>
#include "Content\ShaderStructures.h"
#include "Common\DeviceResources.h"

class Object : public Component
{
public:
	explicit Object(void);
	Object(const Object&) = delete;
	Object& operator=(const Object&) = delete;
	virtual ~Object(void);

public:
	virtual bool Initialize(void);
	virtual bool Update(DX::StepTimer const& timer);
	virtual bool Render(void);

protected:
	std::shared_ptr<DX::DeviceResources>	m_deviceResource;

	Microsoft::WRL::ComPtr<ID3D12Resource>	m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource>	m_indexBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource>	m_constantBuffer;

	ECS_tool::ModelViewProjectionConstantBuffer	m_constantBufferData;

public:
	void Set_DeviceResource(const std::shared_ptr<DX::DeviceResources>& _dr) { m_deviceResource = _dr; }
};

typedef std::shared_ptr<Object> POBJECT;