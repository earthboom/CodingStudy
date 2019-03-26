#include "stdafx.h"
#include "Camera.h"
#include "Const.h"

CCamera::CCamera(void)
	: m_vPosition(0.0f, 0.0f, 0.0f)
	, m_vRight(0.0f, 0.0f, 0.0f)
	, m_vUp(0.0f, 0.0f, 0.0f)
	, m_vLook(0.0f, 0.0f, 0.0f)
	, m_NearZ(0.0f), m_FarZ(0.0f), m_Aspect(0.0f), m_Fov(0.0f)
	, m_NearWindowHeight(0.0f), m_FarWindowHeight(0.0f)
{
	XMMATRIX Id = XMMatrixIdentity();
	XMStoreFloat4x4(&m_View, Id);
	XMStoreFloat4x4(&m_Proj, Id);

	SetLen(float(0.25 * PI), 1.0f, 1.0f, 1000.0f);
}

CCamera::~CCamera(void)
{
}

void CCamera::LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp)
{
}

void CCamera::LookAt(const XMFLOAT3 & pos, const XMFLOAT3 & target, const XMFLOAT3 & up)
{
}

void CCamera::Strafe(float d)
{
}

void CCamera::Wlak(float d)
{
}

void CCamera::Pitch(float angle)
{
}

void CCamera::RotateV(float angle)
{
}

void CCamera::UpdateViewMatrix(void)
{
}

void CCamera::SetLen(const float _fov, const float _aspect, const float zn, const float zf)
{
	m_Fov = _fov;
	m_Aspect = _aspect;
	m_NearZ = zn;
	m_FarZ = zf;

	m_NearWindowHeight = 2.0f * m_NearZ * tanf(0.5f * m_Fov);
	m_FarWindowHeight = 2.0f * m_FarZ * tanf(0.5f * m_Fov);

	XMMATRIX P = XMMatrixPerspectiveFovLH(m_Fov, m_Aspect, m_NearZ, m_FarZ);
	XMStoreFloat4x4(&m_Proj, P);
}
