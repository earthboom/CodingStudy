#pragma once

#include "Object.h"

class Box : public Object
{
public:
	explicit Box(void);
	Box(const Box&) = delete;
	Box& operator=(const Box&) = delete;
	~Box(void);

public:
	virtual bool Ready(void);
	virtual bool Update(const float& dt);
	virtual bool Render(const float& dt);

private:
	void BuildBox(void);
	void BuildRenderItem(void);

public:
	static std::shared_ptr<Box> Create(std::string _name, Object::COM_TYPE _type);
};

typedef std::shared_ptr<Box> BOX;