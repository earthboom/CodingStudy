#include "stdafx.h"
#include "Object.h"
#include "MathHelper.h"
#include "Const.h"

Object::Object(void)
	: Component()
	, m_Comtype(Object::COM_TYPE::CT_STATIC)
	, m_Name("")
	, mWorld(MathHelper::Identity4x4())
	, mView(MathHelper::Identity4x4())
	, mProj(MathHelper::Identity4x4())
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
