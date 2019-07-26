#include "pch.h"
#include "Object.h"

Object::Object(void)
	: Component()
	, m_Comtype(Object::COM_TYPE::CT_STATIC)
	, m_Name(""), m_geoName(""), m_submeshName(""), m_texName(""), m_matName("")
	, mWorld(MathHelper::Identity4x4())
	//, mView(MathHelper::Identity4x4())
	, mProj(MathHelper::Identity4x4())
	, mTheta(1.5f * PI)
	, mPhi(DirectX::XM_PIDIV4)
	, mRadius(5.0f)
	, m_DrawLayer(DrawLayer::DL_END)
	, mPosition(0.0f, 0.0f, 0.0f), mScaling(0.0f, 0.0f, 0.0f), mRotation(0.0f, 0.0f, 0.0f)
{
}

Object::Object(COM_TYPE _type, std::string _name, std::string _geoname, std::string _submeshname, std::string _texname, std::string _matname)
	: Component()
	, m_Comtype(_type)
	, m_Name(_name), m_geoName(_geoname), m_submeshName(_submeshname)
	, m_texName(_texname), m_matName(_matname)
	, mWorld(MathHelper::Identity4x4())
	, mProj(MathHelper::Identity4x4())
	, mTheta(1.5f * PI)
	, mPhi(DirectX::XM_PIDIV4)
	, mRadius(5.0f)
	, m_DrawLayer(DrawLayer::DL_END)
	, mPosition(0.0f, 0.0f, 0.0f), mScaling(0.0f, 0.0f, 0.0f), mRotation(0.0f, 0.0f, 0.0f)
{
}

Object::~Object(void)
{
}

bool Object::Ready(void)
{
	return TRUE;
}

bool Object::Update(const CTimer& mt)
{
	return TRUE;
}

bool Object::Render(const CTimer& mt)
{
	return TRUE;
}

bool Object::BuildDescriptorHeaps(void)
{
	return TRUE;
}
