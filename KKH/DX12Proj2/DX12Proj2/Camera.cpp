#include "pch.h"
#include "Camera.h"

Camera::Camera(void)
	: mRight({ 1.0f, 0.0f, 0.0f })
	, mUp({ 0.0f, 1.0f, 0.0f })
	, mLook({ 0.0f, 0.0f, 1.0f })
	, mNearZ(0.0f)
	, mFarZ(0.0f)
	, mAspect(0.0f)
	, mFovY(0.0f)
	, mNearWindowHeight(0.0f)
	, mFarWindowHeight(0.0f)
	, mViewDirty(FALSE)
	, mView(MathHelper::Identity4x4())
	//, mProj(MathHelper::Identity4x4())
{
}

Camera::~Camera(void)
{
}

//XMVECTOR Camera::GetPosition(void) const
//{
//	return XMLoadFloat3(&mPosition);
//}

//XMFLOAT3 Camera::GetPosition3f(void) const
//{
//	return mPosition;
//}

void Camera::SetPosition(float x, float y, float z)
{
	g_EyePos = XMFLOAT3(x, y, z);
	mViewDirty = TRUE;
}

void Camera::SetPosition(const XMFLOAT3& v)
{
	g_EyePos = v;
	mViewDirty = TRUE;
}

XMVECTOR Camera::GetRight(void) const
{
	return XMLoadFloat3(&mRight);
}

XMFLOAT3 Camera::GetRight3f(void) const
{
	return mRight;
}

XMVECTOR Camera::GetUp(void) const
{
	return XMLoadFloat3(&mUp);
}

XMFLOAT3 Camera::GetUp3f(void) const
{
	return mUp;
}

XMVECTOR Camera::GetLook(void) const
{
	return XMLoadFloat3(&mLook);
}

XMFLOAT3 Camera::GetLook3f(void) const
{
	return mLook;
}

float Camera::GetNearZ(void) const
{
	return mNearZ;
}

float Camera::GetFarZ(void) const
{
	return mFarZ;
}

float Camera::GetAspect(void) const
{
	return mAspect;
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
	//XMStoreFloat4x4(&mProj, P);
	XMStoreFloat4x4(&g_Proj, P);
}

void Camera::LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp)
{
	XMVECTOR L = XMVector3Normalize(XMVectorSubtract(target, pos));
	XMVECTOR R = XMVector3Normalize(XMVector3Cross(worldUp, L));
	XMVECTOR U = XMVector3Cross(L, R);

	XMStoreFloat3(&g_EyePos, pos);
	XMStoreFloat3(&mLook, L);
	XMStoreFloat3(&mRight, R);
	XMStoreFloat3(&mUp, U);

	mViewDirty = TRUE;
}

void Camera::LookAt(XMFLOAT3& pos, XMFLOAT3& target, XMFLOAT3& worldUp)
{
	XMVECTOR P = XMLoadFloat3(&pos);
	XMVECTOR T = XMLoadFloat3(&target);
	XMVECTOR U = XMLoadFloat3(&worldUp);

	LookAt(P, T, U);

	mViewDirty = TRUE;
}

XMMATRIX Camera::GetView(void) const
{
	assert(!mViewDirty);
	return XMLoadFloat4x4(&mView);
}

//XMMATRIX Camera::GetProj(void) const
//{
//	return XMLoadFloat4x4(&mProj);
//}

XMFLOAT4X4 Camera::GetView4x4f(void) const
{
	assert(!mViewDirty);
	return mView;
}

//XMFLOAT4X4 Camera::GetProj4x4f(void) const
//{
//	return mProj;
//}

void Camera::Strafe(float d)
{
	//mPosition += d * mRight
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR r = XMLoadFloat3(&mRight);
	XMVECTOR p = XMLoadFloat3(&g_EyePos);
	XMStoreFloat3(&g_EyePos, XMVectorMultiplyAdd(s, r, p));

	mViewDirty = TRUE;
}

void Camera::Walk(float d)
{
	//mPositoin += d * mLook
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&mLook);
	XMVECTOR p = XMLoadFloat3(&g_EyePos);
	XMStoreFloat3(&g_EyePos, XMVectorMultiplyAdd(s, l, p));

	mViewDirty = TRUE;
}

void Camera::Pitch(float angle)
{
	//Up and view vectors rotate about the vector of right
	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&mRight), angle);

	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));

	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), R));

	mViewDirty = TRUE;
}

void Camera::RotateY(float angle)
{
	//A basis vectors rotate about Y axis of world space
	XMMATRIX R = XMMatrixRotationY(angle);

	XMStoreFloat3(&mRight, XMVector3TransformNormal(XMLoadFloat3(&mRight), R));
	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), R));

	mViewDirty = TRUE;
}

void Camera::UpdateViewMatrix(void)
{
	if (mViewDirty)
	{
		XMVECTOR R = XMLoadFloat3(&mRight);
		XMVECTOR U = XMLoadFloat3(&mUp);
		XMVECTOR L = XMLoadFloat3(&mLook);
		XMVECTOR P = XMLoadFloat3(&g_EyePos);

		L = XMVector3Normalize(L);
		U = XMVector3Normalize(XMVector3Cross(L, R));

		R = XMVector3Cross(U, L);

		float x = -XMVectorGetX(XMVector3Dot(P, R));
		float y = -XMVectorGetX(XMVector3Dot(P, U));
		float z = -XMVectorGetX(XMVector3Dot(P, L));

		XMStoreFloat3(&mRight, R);
		XMStoreFloat3(&mUp, U);
		XMStoreFloat3(&mLook, L);

		mView(0, 0) = mRight.x;
		mView(1, 0) = mRight.y;
		mView(2, 0) = mRight.z;
		mView(3, 0) = x;

		mView(0, 1) = mUp.x;
		mView(1, 1) = mUp.y;
		mView(2, 1) = mUp.z;
		mView(3, 1) = y;

		mView(0, 2) = mLook.x;
		mView(1, 2) = mLook.y;
		mView(2, 2) = mLook.z;
		mView(3, 2) = z;

		mView(0, 3) = 0.0f;
		mView(1, 3) = 0.0f;
		mView(2, 3) = 0.0f;
		mView(3, 3) = 1.0f;

		mViewDirty = FALSE;
	}
}
