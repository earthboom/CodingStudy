#pragma once

#include <Component.h>
#include "Content\ShaderStructures.h"

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
	virtual bool Render(DX::StepTimer const& timer);

protected:
	Microsoft::WRL::ComPtr<ID3D12Resource>	m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource>	m_indexBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource>	m_constantBuffer;

	ECS_tool::ModelViewProjectionConstantBuffer	m_constantBufferData;
};

typedef std::shared_ptr<Object> POBJECT;