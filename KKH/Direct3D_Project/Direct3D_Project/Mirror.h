#pragma once

#include "Object.h"

class Mirror : public Object
{
public:
	explicit Mirror(void);
	Mirror(Object::COM_TYPE _type, std::string _name, std::string _submeshname, std::string _texname, std::string _matname);
	Mirror(const Mirror&) = delete;
	Mirror& operator=(const Mirror&) = delete;
	~Mirror(void);

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

private:
	std::array<VERTEX, 4> surface_vt;

public:
	void Set_Vertex(std::array<VERTEX, 4>& _vertices) { surface_vt = _vertices; }

public:
	static std::shared_ptr<Mirror> Create(Object::COM_TYPE _type, std::string _name, std::string _submeshname,
		std::string _texname, std::string _matname);
};

typedef std::shared_ptr<Mirror> MIRROR;