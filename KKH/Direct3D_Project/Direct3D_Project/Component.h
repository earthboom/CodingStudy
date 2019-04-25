#pragma once

#include "combaseapi.h"

class Component
{
protected:
	explicit Component(void);
	~Component(void);

protected:
	virtual bool Ready(void) PURE;
	virtual bool Update(const float& dt) PURE;	
};

typedef std::shared_ptr<Component> COM;