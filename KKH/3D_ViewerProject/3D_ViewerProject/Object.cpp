#include "stdafx.h"
#include "Object.h"
#include "MathHelper.h"
#include "Const.h"

Object::Object(void)
	: Component()
	, m_Comtype(Object::COM_TYPE::CT_STATIC)
	, mWorld(MathHelper::Indentity4x4())
	, mView(MathHelper::Indentity4x4())
	, mProj(MathHelper::Indentity4x4())
	, mTheta(1.5f * PI)
	, mPhi(DirectX::XM_PIDIV4)
	, mRadius(5.0f)
{
}

Object::~Object(void)
{
}

bool Object::Ready(void)
{
	return TRUE;
}

bool Object::Update(const float & dt)
{
	return TRUE;
}
