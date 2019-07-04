#pragma once

class Object;
class Cylinder : public Object
{
public:
	explicit Cylinder(void);
	Cylinder(Object::COM_TYPE _type, std::string _name, std::string _submeshname, std::string _texname, std::string _matname);
	Cylinder(const Cylinder&) = delete;
	Cylinder& operator=(const Cylinder&) = delete;
	~Cylinder(void);

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
	static std::shared_ptr<Cylinder> Create(Object::COM_TYPE _type, std::string _name, std::string _submeshname,
		std::string _texname, std::string _matname);
};

typedef std::shared_ptr<Cylinder> CYLINDER;