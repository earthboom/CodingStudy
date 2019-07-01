#pragma once

class Camera
{
public:
	explicit Camera(void);
	Camera(const Camera&) = delete;
	Camera& operator=(const Camera&) = delete;
	~Camera(void);

	//Getting or Setting method of world space camera position
	XMVECTOR GetPosition(void)const;
	XMFLOAT3 GetPosition3(void)const;
	void SetPosition(float x, float y, float z);
	void SetPosition(const XMFLOAT3& v);

	//Camera basis vectors getting
	XMVECTOR GetRight(void)const;
	XMFLOAT3 GetRight3f(void)const;
	XMVECTOR GetUp(void)const;
	XMFLOAT3 GetUp3f(void)const;
	XMVECTOR GetLook(void)const;
	XMFLOAT3 GetLook3f(void)const;

	//Frustum's attributes getting
	float GetNearZ(void)const;
	float GetFarZ(void)const;
	float GetAspect(void)const;
	float GetFovY(void)const;
	float GetFovX(void)const;

	//Near or far window getting in view space
	float GetNearWindowWidth(void)const;
	float GetNearWindowHeight(void)const;
	float GetFarWindowWidth(void)const;
	float GetFarWindowHeight(void)const;

	//Setting vew frustum
	void SetLens(float fovY, float aspect, float zn, float zf);

	//Setting coordinate system by using camera's position, viewing vector and up vector
	void LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp);
	void LookAt(XMFLOAT3& pos, XMFLOAT3& target, XMFLOAT3& worldUp);

	//Getting View or project matrix
	XMMATRIX GetView(void) const;
	XMMATRIX GetProj(void) const;

	XMFLOAT4X4 GetView4x4f(void) const;
	XMFLOAT4X4 GetProj4x4f(void) const;

	//Move camera transverse or axial movement as much as distance d.
	void Strafe(float d);
	void Walk(float d);

	//Rotate camera
	void Pitch(float angle);
	void RotateY(float angle);

	//You should rebuild by call this method after fix the camera's position or rotation
	void UpdateViewMatrix(void);

private:
	//Coordinate system of camera based on world space.
	XMFLOAT3 mPosition;
	XMFLOAT3 mRight;
	XMFLOAT3 mUp;
	XMFLOAT3 mLook	;

	//Frustum's attributes
	float mNearZ;
	float mFarZ;
	float mAspect;
	float mFovY;
	float mNearWindowHeight;
	float mFarWindowHeight;

	bool mViewDirty = TRUE;

	//View of Project Matrix
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;
};