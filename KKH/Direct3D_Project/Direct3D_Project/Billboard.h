#pragma once

#include "Object.h"

class Billboard : public Object
{
public:
	explicit Billboard(void);
	Billboard(Object::COM_TYPE _type, std::string _name, std::string _submeshname, std::string _texname, std::string _matname);
	Billboard(const Billboard&) = delete;
	Billboard& operator = (const Billboard&) = delete;
	~Billboard(void);

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

public:
	static std::shared_ptr<Billboard> Create(Object::COM_TYPE _type, std::string _name, std::string _submeshname,
		std::string _texname, std::string _matname);
};

typedef std::shared_ptr<Billboard> BILLBOARD;