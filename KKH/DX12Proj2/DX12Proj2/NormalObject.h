#pragma once

#include "Object.h"

class NormalObject : public Object
{
public:
	explicit NormalObject(void);
	NormalObject(Object::COM_TYPE _type, std::string _geoname, 
		std::string _submeshname, std::string _texname, std::string _matname, 
		std::string _normalMap = "", ShapeType eType = ST_END);
	NormalObject(const NormalObject&) = delete;
	NormalObject& operator=(const NormalObject&) = delete;
	~NormalObject(void);

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
	

public:
	static std::shared_ptr<NormalObject> Create(Object::COM_TYPE _type, 
		std::string _geoname, std::string _submeshname, std::string _texname, 
		std::string _matname, std::string _normalMap, ShapeType eType = ST_END);
};

typedef std::shared_ptr<NormalObject> NORMALOBJECT;