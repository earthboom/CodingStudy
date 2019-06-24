#pragma once

#include "Object.h"

class Surface : public Object
{
public:
	enum SURFACE_TYPE
	{
		ST_DEFAULT,
		ST_BASIC_TESELL,
	};

public:
	explicit Surface(void);
	Surface(Object::COM_TYPE _type, std::string _name, std::string _submeshname, std::string _texname, std::string _matname);
	Surface(const Surface&) = delete;
	Surface& operator=(const Surface&) = delete;
	~Surface(void);

public:
	virtual bool Ready(void);
	virtual bool Update(const CTimer& mt);
	virtual bool Render(const CTimer& mt);

protected:
	virtual void BuildDescriptorHeaps(void);

private:
	void BuildMaterials(void);
	void BuildRenderItem(void);

private:
	void BuildGeometry(void);
	void BuildGeometry_1(void);

private:
	std::array<VERTEX, 4> surface_vt;
	SURFACE_TYPE m_surfaceType;

public:
	void Set_Vertex(std::array<VERTEX, 4>& _vertices) { surface_vt = _vertices; }
	void Set_SurfaceType(SURFACE_TYPE _type) { m_surfaceType = _type; }

public:
	static std::shared_ptr<Surface> Create(Object::COM_TYPE _type, std::string _name, std::string _submeshname,
		std::string _texname, std::string _matname);
};

typedef std::shared_ptr<Surface> SURFACE;