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

protected:
	virtual void AnimateMaterials(const float& dt) {}
	virtual void UpdateObjectCBs(const float& dt) {}
	virtual void UpdateMaterialCBs(const float& dt) {}
	virtual void UpdateMainPassCB(const float& dt) {}

private:
	void BuildBox(void);
	void BuildRenderItem(void);

private:
	std::string mGeoName;
	float mWidth;
	float mHeight;
	float mDepth;
	uint32_t mSubdiv;

public:
	void Set_BoxAtt(const std::string& _name, const float& w, const float& h, const float& d, const uint32_t& s);

public:
	static std::shared_ptr<Box> Create(std::string _name, Object::COM_TYPE _type, std::string _geoname, float _w = 0.0f, float _h = 0.0f, float _d = 0.0f, float _subdiv = 3);
};

typedef std::shared_ptr<Box> BOX;