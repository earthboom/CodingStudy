#pragma once

#include "Object.h"

class Grid : public Object
{
public:
	explicit Grid(void);
	Grid(std::string _name, std::string _submeshname, std::string _texname, std::string _matname);
	Grid(const Grid&) = delete;
	Grid& operator=(const Grid&) = delete;
	~Grid(void);

public:
	virtual bool Ready(void);
	virtual bool Update(const float& dt);
	virtual bool Render(const float& dt);

protected:
	virtual void BuildDescriptorHeaps(void);

	virtual void AnimateMaterials(const float& dt) {}
	virtual void UpdateObjectCBs(const float& dt);
	virtual void UpdateMaterialCBs(const float& dt) {}
	virtual void UpdateMainPassCB(const float& dt) {}

private:
	void BuildMaterials(void);
	void BuildRenderItem(void);

private:
	void BuildGeometry(void);

public:
	static std::shared_ptr<Grid> Create(std::string _name, std::string _submeshname, 
		std::string _texname, std::string _matname);
};

typedef std::shared_ptr<Grid> GRID;