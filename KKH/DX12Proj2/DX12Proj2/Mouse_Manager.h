#pragma once

#include "Object.h"

#define MOUSE Mouse_Manager::GetInstnace()

class Mouse_Manager : public CSingleton<Mouse_Manager>
{
public:
	explicit Mouse_Manager(void);
	Mouse_Manager(const Mouse_Manager&) = delete;
	Mouse_Manager& operator=(const Mouse_Manager&) = delete;
	~Mouse_Manager(void);

public:
	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUP(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

	void Pick(int sx, int sy);

private:
	POINT	mLastMousePos;

	std::shared_ptr<Object> mObj;

	std::vector<RenderItem*> mPickedRitem;

public:
	POINT& Get_LastMousePos(void) { return mLastMousePos; }

	void Set_Obj(std::shared_ptr<Object> _obj) { mObj = _obj; }

	//void SetPieckedRitem(void);
	std::vector<RenderItem*>& GetPickedRitem(void) { return mPickedRitem; }
};