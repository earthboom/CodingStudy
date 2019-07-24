#pragma once

#include "Object.h"

class NormalObject : public Object
{
public:
	enum ShapeType
	{
		ST_BOX,
		ST_GRID,
		ST_SHPERE,
		ST_CYLINEDER,
		ST_END
	};

public:
	explicit NormalObject(void);
	NormalObject(Object::COM_TYPE _type, std::string _name, std::string _submeshname, std::string _texname, std::string _matname, ShapeType eType);
	NormalObject(const NormalObject&) = delete;
	NormalObject& operator=(const NormalObject&) = delete;
	~NormalObject(void);

public:
	virtual bool Ready(void);
	virtual bool Update(const CTimer& mt);
	virtual bool Render(const CTimer& mt);

protected:
	virtual bool BuildDescriptorHeaps(void);

private:
	void BuildMaterials(void);
	void BuildRenderItem(void);

private:
	void BuildGeometry(void);

private:
	ShapeType m_eShapeType;

public:
	static std::shared_ptr<NormalObject> Create(Object::COM_TYPE _type, std::string _name, std::string _submeshname,
		std::string _texname, std::string _matname, ShapeType eType);
};

typedef std::shared_ptr<NormalObject> NORMALOBJECT;