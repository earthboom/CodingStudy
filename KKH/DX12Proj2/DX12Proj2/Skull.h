#pragma once

#include "Object.h"

class Skull : public Object
{
public:
	explicit Skull(void);
	Skull(Object::COM_TYPE _type, std::string _geoname, std::string _submeshname, 
		std::string _texname, std::string _matname, std::string _normalMap = "");
	Skull(const Skull&) = delete;
	Skull& operator=(const Skull&) = delete;
	~Skull(void);

public:
	virtual bool Ready(void);
	virtual bool Update(const CTimer& mt);
	virtual bool Render(const CTimer& mt);

protected:
	virtual bool BuildDescriptorHeaps(void);
	virtual void BuildGeometry(void);

private:
	void BuildMaterials(void);
	void BuildRenderItem(void);

private:
	DirectX::XMFLOAT3 mSkullTranslation;

	RenderItem* m_Skull;

public:
	static std::shared_ptr<Skull> Create(Object::COM_TYPE _type, std::string _geoname, 
		std::string _submeshname, std::string _texname, std::string _matname, std::string _normalMap = "");
};

typedef std::shared_ptr<Skull> SKULL;