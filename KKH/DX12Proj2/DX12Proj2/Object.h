#pragma once

#include "Component.h"

class Object : public Component
{
public:
	enum COM_TYPE { CT_STATIC, CT_DYNAMIC, DT_END };

public:
	enum ShapeType
	{
		ST_BOX,
		ST_GRID,
		ST_SHPERE,
		ST_CYLINEDER,
		ST_END
	};

protected:
	explicit Object(void);
	Object(COM_TYPE _type, std::string _name, std::string _submeshname, 
		std::string _texname, std::string _matname, std::string _normalMap = "", 
		ShapeType eType = ST_END);
	Object(const Object&) = delete;
	Object& operator=(const Object&) = delete;
	~Object(void);

public:
	virtual bool Ready(void);
	virtual bool Update(const CTimer& mt);
	virtual bool Render(const CTimer& mt);

protected:
	virtual bool BuildDescriptorHeaps(void);
	virtual void BuildGeometry(void);

public:
	std::string m_Name;
	std::string m_submeshName;
	std::string m_texName;
	std::string m_matName;
	std::string m_normalTex;

protected:	
	COM_TYPE	m_Comtype;
	DrawLayer	m_DrawLayer;
	Material	m_Material;

	DirectX::XMFLOAT4X4 mWorld; //= MathHelper::Identity4x4();
	//DirectX::XMFLOAT4X4 mView; //= MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 mProj; //= MathHelper::Identity4x4();

	float mTheta;	// = 1.5f * PI;
	float mPhi;		// = DirectX::XM_PIDIV4;
	float mRadius;	// = 5.0f;

	DirectX::XMFLOAT3 mPosition;
	DirectX::XMFLOAT3 mScaling;
	DirectX::XMFLOAT3 mRotation;

	std::vector<InstanceData>	m_vecInstanceData;

	UINT	m_matCount;

	ShapeType m_eShapeType;

public:
	//std::function<COM_TYPE&()> Get_Comtype = [&]()->COM_TYPE& {return m_Comtype; };
	//std::function<std::string&()> Get_Objname = [&]()->std::string& {return m_Name; };
	COM_TYPE& Get_Comtype(void){return m_Comtype; }
	std::string& Get_Objname(void){return m_Name; }
	std::string& Get_submeshName(void) { return m_submeshName; }

	//std::function<float&()> Get_Theta = [&]()->float& {return mTheta; };
	//std::function<float&()> Get_Phi = [&]()->float& {return mPhi; };
	//std::function<float&()> Get_Radius = [&]()->float& {return mRadius; };
	float& Get_Theta(void) { return mTheta; }
	float& Get_Phi(void) { return mPhi; }
	float& Get_Radius(void) { return mRadius; }

	Material& Get_Material(void) { return m_Material; }
	DrawLayer& Get_DrawLayer(void) { return m_DrawLayer; }

	DirectX::XMFLOAT3& Get_Postion(void) { return mPosition; }
	DirectX::XMFLOAT3& Get_Scaling(void) { return mScaling; }
	DirectX::XMFLOAT3& Get_Rotation(void) { return mRotation; }

	std::vector<InstanceData>& Get_vectorInstanceData(void) { return m_vecInstanceData; }
};

typedef std::shared_ptr<Object> OBJECT;