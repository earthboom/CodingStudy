#pragma once

#include "..\Common\StepTimer.h"

class Component
{
protected:
	explicit Component(void);
	Component(const Component&) = delete;
	Component& operator=(const Component&) = delete;
	virtual ~Component(void);

protected:
	virtual bool Initialize(void) PURE;
	virtual bool Update(DX::StepTimer const& timer) PURE;
	virtual bool Render(void) PURE;
};