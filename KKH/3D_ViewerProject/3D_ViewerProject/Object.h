#pragma once

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
	DirectX::XMFLOAT4X4 mWorld; //= MathHelper::Indentity4x4();
	DirectX::XMFLOAT4X4 mView; //= MathHelper::Indentity4x4();
	DirectX::XMFLOAT4X4 mProj; //= MathHelper::Indentity4x4();

	float mTheta;	// = 1.5f * PI;
	float mPhi;		// = DirectX::XM_PIDIV4;
	float mRadius;	// = 5.0f;

public:
	float& Get_Theta(void) { return mTheta; }
	float& Get_Phi(void) { return mPhi; }
	float& Get_Radius(void) { return mRadius; }
};

typedef std::shared_ptr<Object> OBJECT;