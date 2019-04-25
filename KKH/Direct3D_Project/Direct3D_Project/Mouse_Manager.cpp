#include "stdafx.h"
#include "Mouse_Manager.h"
#include "MathHelper.h"
#include "Const.h"
#include "Object.h"

Mouse_Manager::Mouse_Manager(void)
{
}

Mouse_Manager::~Mouse_Manager(void)
{
}

void Mouse_Manager::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(g_hWnd);
}

void Mouse_Manager::OnMouseUP(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void Mouse_Manager::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		float dx = DirectX::XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
		float dy = DirectX::XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));

		mObj->Get_Theta() += dx;
		mObj->Get_Phi() += dy;

		mObj->Get_Phi() = MathHelper::Clamp(mObj->Get_Phi(), 0.1f, PI - 0.1f);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		float dx = 0.05f * static_cast<float>(x - mLastMousePos.x);
		float dy = 0.05f * static_cast<float>(y - mLastMousePos.y);

		mObj->Get_Radius() += dx - dy;

		mObj->Get_Radius() = MathHelper::Clamp(mObj->Get_Radius(), 5.0f, 150.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}
