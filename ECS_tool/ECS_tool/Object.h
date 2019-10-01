#pragma once

#include <Component.h>

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
};

typedef std::shared_ptr<Object> POBJECT;