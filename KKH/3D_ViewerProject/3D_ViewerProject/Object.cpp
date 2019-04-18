#include "stdafx.h"
#include "Object.h"
#include "MathHelper.h"
#include "Const.h"

Object::Object(void)
	: mWorld(MathHelper::Indentity4x4())
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
