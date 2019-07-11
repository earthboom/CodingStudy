#include "pch.h"
#include "Mouse_Manager.h"
#include "Utility_Manager.h"
#include "Object.h"
#include "Camera_Manager.h"

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
	//if ((btnState & MK_LBUTTON) != 0)
	//{
	//	float dx = DirectX::XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
	//	float dy = DirectX::XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));

	//	for (auto& all : UTIL.Get_Allobjvec())
	//	{
	//		for (auto& objvec : *all)
	//		{
	//			//if (UTIL.Get_CurrState() == Utility_Manager::ObjState::OS_UPDATE)
	//			//{
	//				objvec.second->Get_Theta() += dx;

	//				objvec.second->Get_Phi() += dy;
	//				objvec.second->Get_Phi() = MathHelper::Clamp(objvec.second->Get_Phi(), 0.1f, PI - 0.1f);
	//			//}
	//		}
	//	}
	//}
	//else if ((btnState & MK_RBUTTON) != 0)
	//{
	//	float dx = 0.05f * static_cast<float>(x - mLastMousePos.x);
	//	float dy = 0.05f * static_cast<float>(y - mLastMousePos.y);

	//	for (auto& all : UTIL.Get_Allobjvec())
	//	{
	//		for (auto& objvec : *all)
	//		{
	//			//if (UTIL.Get_CurrState() == Utility_Manager::ObjState::OS_UPDATE)
	//			//{
	//				objvec.second->Get_Radius() += dx - dy;
	//				objvec.second->Get_Radius() = MathHelper::Clamp(objvec.second->Get_Radius(), 5.0f, 150.0f);
	//			//}
	//		}
	//	}
	//}

	if ((btnState & MK_LBUTTON) != 0)
	{
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));

		CURR_CAM->Pitch(dy);
		CURR_CAM->RotateY(dx);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}
