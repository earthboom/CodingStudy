#include "stdafx.h"
#include "Component.h"

Component::Component(void)
{
}

Component::~Component(void)
{
}

bool Component::Ready(void)
{
	return TRUE;
}

bool Component::Update(const CTimer& mt)
{
	return TRUE;
}
