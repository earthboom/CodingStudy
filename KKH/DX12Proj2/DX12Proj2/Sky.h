#pragma once

#include "Object.h"

class Sky : public Object
{
public:
	explicit Sky(void);
	Sky(Object::COM_TYPE _type, std::string _geoname, std::string _submeshname, std::string _texname, std::string _matname);
	Sky(const Sky&) = delete;
	Sky& operator=(const Sky&) = delete;
	~Sky(void);

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

public:
	static std::shared_ptr<Sky> Create(Object::COM_TYPE _type, std::string _geoname,
		std::string _submeshname, std::string _texname, std::string _matname);
};

typedef std::shared_ptr<Sky> SKY;