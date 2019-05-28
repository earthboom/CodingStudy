#pragma once

#include "Object.h"


class Wave : public Object
{
public:
	explicit Wave(void);
	Wave(Object::COM_TYPE _type, std::string _name, std::string _submeshname, std::string _texname, std::string _matname);
	Wave(const Wave&) = delete;
	Wave& operator=(const Wave&) = delete;
	~Wave(void);

public:
	virtual bool Ready(void);
	virtual bool Update(const CTimer& mt);
	virtual bool Render(const CTimer& mt);

protected:
	virtual void BuildDescriptorHeaps(void);

private:
	void BuildMaterials(void);
	void BuildRenderItem(void);
	
	void AnimateMaterials(const CTimer& mt);
	void UpdateWaves(const CTimer& mt);

private:
	void BuildGeometry(void);

private:
	RenderItem* mWaveRitem;

public:
	static std::shared_ptr<Wave> Create(Object::COM_TYPE _type, std::string _name, std::string _submeshname,
		std::string _texname, std::string _matname);
};

typedef std::shared_ptr<Wave> WAVE;
