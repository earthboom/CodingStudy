#pragma once

#include "Object.h"
#include "Struct.h"

class Shape : public Object
{
public:
	explicit Shape(void);
	Shape(const Shape&) = delete;
	Shape& operator = (const Shape&) = delete;
	~Shape(void);

public:
	virtual bool Ready(void);
	virtual bool Update(const float& dt);
	virtual bool Render(const float& dt);

private:
	void BuildFrameResources(void);

private:
	std::vector<std::unique_ptr<FrameResource>> mFrameResources;
	FrameResource* mCurrFrameResource = nullptr;
	int mCurrFrameResourceIndex = 0;

	std::vector<std::unique_ptr<RenderItem>> mAllRitems;

	std::vector<RenderItem> mOpaqueRitems;
	std::vector<RenderItem> mTransparentRitems;
};