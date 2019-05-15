#pragma once

#include "Object.h"

class Wave : public Object
{
	explicit Wave(void);
	Wave(const Wave&) = delete;
	Wave& operator=(const Wave&) = delete;
	~Wave(void);

public:
	virtual bool Ready(void);
	virtual bool Update(const float& dt);
	virtual bool Render(const float& dt);

protected:
	virtual void BuildDescriptorHeaps(void);

private:
	void BuildMaterials(void);
	void BuildRenderItem(void);

private:
	void BuildGeometry(void);

public:
	static std::shared_ptr<Wave> Create(Object::COM_TYPE _type, std::string _name, std::string _submeshname,
		std::string _texname, std::string _matname);
};

typedef std::shared_ptr<Wave> WAVE;
