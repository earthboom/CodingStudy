#pragma once

#include "MathHelper.h"

class Object
{
protected:
	explicit Object(void);
	Object(const Object&) = delete;
	Object& operator=(const Object&) = delete;
	~Object(void);

protected:
	virtual bool Ready(void) PURE;
	virtual bool Update(const float& dt) PURE;
	virtual bool Render(const float& dt) PURE;

protected:
	DirectX::XMFLOAT4X4 mWorld = MathHelper::Indentity4x4();
	DirectX::XMFLOAT4X4 mView = MathHelper::Indentity4x4();
	DirectX::XMFLOAT4X4 mProj = MathHelper::Indentity4x4();
};

typedef std::shared_ptr<Object> OBJECT;