#pragma once

class Object
{
protected:
	explicit Object(void);
	Object(const Object&) = delete;
	Object& operator=(const Object&) = delete;
	~Object(void);

protected:
	virtual bool Ready(void) PURE;
	virtual bool Update(const float& dt) PURE;
	virtual bool Render(const float& dt) PURE;
};

typedef std::shared_ptr<Object> OBJECT;