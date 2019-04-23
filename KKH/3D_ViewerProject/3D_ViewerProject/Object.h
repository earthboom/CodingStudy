#pragma once

#include "Component.h"

class Object : public Component
{
public:
	enum COM_TYPE { CT_STATIC, CT_DYNAMIC, DT_END };

protected:
	explicit Object(void);
	Object(const Object&) = delete;
	Object& operator=(const Object&) = delete;
	~Object(void);

public:
	virtual bool Ready(void);
	virtual bool Update(const float& dt);
	virtual bool Render(const float& dt) PURE;

protected:
	COM_TYPE m_Comtype;
	std::string m_Name;

	DirectX::XMFLOAT4X4 mWorld; //= MathHelper::Indentity4x4();
	DirectX::XMFLOAT4X4 mView; //= MathHelper::Indentity4x4();
	DirectX::XMFLOAT4X4 mProj; //= MathHelper::Indentity4x4();

	float mTheta;	// = 1.5f * PI;
	float mPhi;		// = DirectX::XM_PIDIV4;
	float mRadius;	// = 5.0f;

public:
	std::function<COM_TYPE&()> Get_Comtype = [&]()->COM_TYPE& {return m_Comtype; };
	std::function<std::string&()> Get_Objname = [&]()->std::string& {return m_Name; };

	std::function<float&()> Get_Theta = [&]()->float& {return mTheta; };
	std::function<float&()> Get_Phi = [&]()->float& {return mPhi; };
	std::function<float&()> Get_Radius = [&]()->float& {return mRadius; };
	
	//float& Get_Theta(void) { return mTheta; }
	//float& Get_Phi(void) { return mPhi; }
	//float& Get_Radius(void) { return mRadius; }
};

typedef std::shared_ptr<Object> OBJECT;