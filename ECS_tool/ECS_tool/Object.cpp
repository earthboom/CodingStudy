#include "pch.h"
#include "Object.h"

Object::Object(void)
	: Component()
{
}

Object::~Object(void)
{
}

bool Object::Initialize(void)
{
	return TRUE;
}

bool Object::Update(DX::StepTimer const& timer)
{
	return TRUE;
}

bool Object::Render(DX::StepTimer const& timer)
{
	return TRUE;
}
