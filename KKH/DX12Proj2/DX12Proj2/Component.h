#pragma once

#include "combaseapi.h"
#include "Timer.h"

class Component
{
protected:
	explicit Component(void);
	~Component(void);

protected:
	virtual bool Ready(void) PURE;
	virtual bool Update(const CTimer& mt) PURE;
};

typedef std::shared_ptr<Component> COM;