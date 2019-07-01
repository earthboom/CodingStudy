#include "stdafx.h"
#include "Camera.h"

Camera::Camera(void)
	: mPosition({ 0.0f, 0.0f, 0.0f })
	, mRight({ 1.0f, 0.0f, 0.0f })
	, mUp({ 0.0f, 1.0f, 0.0f })
	, mLook({ 0.0f, 0.0f, 1.0f })
	, mNearZ(0.0f)
	, mFarZ(0.0f)
	, mAspect(0.0f)
	, mFovY(0.0f)
	, mNearWindowHeight(0.0f)
	, mFarWindowHeight(0.0f)
	, mView(MathHelper::Identity4x4())
	, mProj(MathHelper::Identity4x4())
{
}

Camera::~Camera(void)
{
}

XMVECTOR Camera::GetPosition(void) const
{
	return XMLoadFloat3(&mPosition);
}

XMFLOAT3 Camera::GetPosition3(void) const
{
	return mPosition;
}

void Camera::SetPosition(float x, float y, float z)
{
}

void Camera::SetPosition(const XMFLOAT3& v)
{
	
}

XMVECTOR Camera::GetRight(void) const
{
	return XMVECTOR();
}

XMFLOAT3 Camera::GetRight3f(void) const
{
	return XMFLOAT3();
}

XMVECTOR Camera::GetUp(void) const
{
	return XMVECTOR();
}

XMFLOAT3 Camera::GetUp3f(void) const
{
	return XMFLOAT3();
}

XMVECTOR Camera::GetLook(void) const
{
	return XMVECTOR();
}

XMFLOAT3 Camera::GetLook3f(void) const
{
	return XMFLOAT3();
}

float Camera::GetNearZ(void) const
{
	return 0.0f;
}

float Camera::GetFarZ(void) const
{
	return 0.0f;
}

float Camera::GetAspect(void) const
{
	return 0.0f;
}

float Camera::GetFovY(void) const
{
	return mFovY;
}

float Camera::GetFovX(void) const
{
	float halfWidth = 0.5f * GetNearWindowWidth();
	return 2.0f * atan(halfWidth / mNearZ);
}

float Camera::GetNearWindowWidth(void) const
{
	return mAspect * mNearWindowHeight;
}

float Camera::GetNearWindowHeight(void) const
{
	return mNearWindowHeight;
}

float Camera::GetFarWindowWidth(void) const
{
	return mAspect * mFarWindowHeight;
}

float Camera::GetFarWindowHeight(void) const
{
	return mFarWindowHeight;
}

void Camera::SetLens(float fovY, float aspect, float zn, float zf)
{
	//Save attributes
	mFovY = fovY;
	mAspect = aspect;
	mNearZ = zn;
	mFarZ = zf;

	mNearWindowHeight = 2.0f * mNearZ * tanf(0.5f * mFovY);
	mFarWindowHeight = 2.0f * mFarZ * tanf(0.5f * mFovY);

	XMMATRIX P = XMMatrixPerspectiveFovLH(mFovY, mAspect, mNearZ, mFarZ);
	XMStoreFloat4x4(&mProj, P);
}

void Camera::LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp)
{
}

void Camera::LookAt(XMFLOAT3& pos, XMFLOAT3& target, XMFLOAT3& worldUp)
{
}

XMMATRIX Camera::GetView(void) const
{
	return XMMATRIX();
}

XMMATRIX Camera::GetProj(void) const
{
	return XMMATRIX();
}

XMFLOAT4X4 Camera::GetView4x4f(void) const
{
	return XMFLOAT4X4();
}

XMFLOAT4X4 Camera::GetProj4x4f(void) const
{
	return XMFLOAT4X4();
}

void Camera::Strafe(float d)
{
	//mPosition += d * mRight
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR r = XMLoadFloat3(&mUp);
	XMVECTOR p = XMLoadFloat3(&mPosition);
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, r, p));
}

void Camera::Walk(float d)
{
	//mPositoin += d * mLook
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&mLook);
	XMVECTOR p = XMLoadFloat3(&mPosition);
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, l, p));
}

void Camera::Pitch(float angle)
{
	//Up and view vectors rotate about the vector of right
	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&mRight), angle);

	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));

	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), R));
}

void Camera::RotateY(float angle)
{
	//A basis vectors rotate about Y axis of world space
	XMMATRIX R = XMMatrixRotationY(angle);

	XMStoreFloat3(&mRight, XMVector3TransformNormal(XMLoadFloat3(&mRight), R));
	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), R));
}

void Camera::UpdateViewMatrix(void)
{
	if (mViewDirty)
	{
		XMVECTOR R = XMLoadFloat3(&mRight);
		XMVECTOR U = XMLoadFloat3(&mLook);
		XMVECTOR L = XMLoadFloat3(&mUp);
		XMVECTOR P = XMLoadFloat3(&mPosition);


	}
}
