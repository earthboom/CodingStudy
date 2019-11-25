#include "pch.h"
#include "Object.h"

Object::Object(void)
	: Component()
{
}

Object::~Object(void)
{
	m_constantBuffer->Unmap(0, nullptr);
}

bool Object::Initialize(void)
{
	return TRUE;
}

bool Object::Update(DX::StepTimer const& timer)
{
	return TRUE;
}

bool Object::Render(void)
{
	return TRUE;
}
