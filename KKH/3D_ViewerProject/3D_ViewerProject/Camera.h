#pragma once

class CCamera
{
public:
	explicit CCamera(void);
	~CCamera(void);

public:
	void LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp);
	void LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up);

	void Strafe(float d);
	void Wlak(float d);

	void Pitch(float angle);
	void RotateV(float angle);

	void UpdateViewMatrix(void);

public:
	void SetLen(const float _fov, const float _aspect, const float zn, const float zf);

	std::function<void(const float x, const float y, const float z)> SetFloat_Position = [this](const float x, const float y, const float z) { m_vPosition = XMFLOAT3(x, y, z); };
	std::function<void(const XMFLOAT3& _pos)> SetVector_Position = [this](const XMFLOAT3& _pos) {m_vPosition = _pos; };

private:
	XMFLOAT3	m_vPosition;
	XMFLOAT3	m_vRight;
	XMFLOAT3	m_vUp;
	XMFLOAT3	m_vLook;

	float		m_NearZ;
	float		m_FarZ;
	float		m_Aspect;
	float		m_Fov;
	float		m_NearWindowHeight;
	float		m_FarWindowHeight;

	XMFLOAT4X4	m_View;
	XMFLOAT4X4	m_Proj;
};

typedef std::shared_ptr<CCamera>	PCAM;