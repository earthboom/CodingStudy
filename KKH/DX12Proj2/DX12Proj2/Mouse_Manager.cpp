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
	if ((btnState & MK_LBUTTON) != 0)
	{
		mLastMousePos.x = x;
		mLastMousePos.y = y;

		SetCapture(g_hWnd);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		Pick(x, y);
	}	
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

void Mouse_Manager::Pick(int sx, int sy)
{
	XMFLOAT4X4 P = g_Proj;

	// Compute picking ray in view space
	float vx = (2.0f * sx / WINSIZE_X - 1.0f) / P(0, 0);
	float vy = (-2.0f * sy / WINSIZE_Y + 1.0f) / P(1, 1);

	XMVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR rayDir = XMVectorSet(vx, vy, 1.0f, 0.0f);

	XMMATRIX V = CURR_CAM->GetView();
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(V), V);

	for (auto& e : mPickedRitem)
		e->Visible = FALSE;

	for (auto ri : UTIL.Get_Drawlayer((int)DrawLayer::DL_OPAUQE))
	{
		auto geo = ri->Geo;
		if (ri->Visible == FALSE)
			continue;

		XMMATRIX W = XMLoadFloat4x4(&ri->World);
		XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(W), W);

		XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);

		rayOrigin = XMVector3TransformCoord(rayOrigin, toLocal);
		rayDir = XMVector3TransformNormal(rayDir, toLocal);

		rayDir = XMVector3Normalize(rayDir);

		float tmin = 0.0f;
		if (ri->Bounds.Intersects(rayOrigin, rayDir, tmin))
		{
			RenderItem* pickItem = nullptr;
			for (auto e : mPickedRitem)
			{
				if (e->Geo->Name == ri->Geo->Name)
					pickItem = e;
			}

			auto vertices = (VERTEX*)geo->VertexBufferCPU->GetBufferPointer();
			auto indices = (std::uint32_t*)geo->IndexBufferCPU->GetBufferPointer();
			UINT triCount = ri->IndexCount / 3;

			tmin = Infinity;
			for (UINT i = 0; i < triCount; ++i)
			{
				UINT i0 = indices[i * 3 + 0];
				UINT i1 = indices[i * 3 + 1];
				UINT i2 = indices[i * 3 + 2];

				XMVECTOR v0 = XMLoadFloat3(&vertices[i0].Pos);
				XMVECTOR v1 = XMLoadFloat3(&vertices[i1].Pos);
				XMVECTOR v2 = XMLoadFloat3(&vertices[i2].Pos);

				float t = 0.0f;
				if (TriangleTests::Intersects(rayOrigin, rayDir, v0, v1, v2, t))
				{
					if (t < tmin)
					{
						tmin = t;
						UINT pickedTriangle = i;

						pickItem->Visible = TRUE;
						pickItem->IndexCount = 3;
						pickItem->BaseVertexLocation = 0;

						pickItem->World = ri->World;
						pickItem->NumFramesDirty = NumFrameResources;

						pickItem->StartIndexLocation = 3 * pickedTriangle;
					}
				}
			}
		}
	}
}

//void Mouse_Manager::SetPieckedRitem(void)
//{	
//	for (auto& e : UTIL.Get_Ritemvec())
//	{
//		RenderItem* _pieckedRitem = new RenderItem();
//		_pieckedRitem->World = MathHelper::Identity4x4();
//		_pieckedRitem->TexTransform = MathHelper::Identity4x4();
//		_pieckedRitem->objCBIndex = g_ObjCBcount++;
//		_pieckedRitem->Mat = e.get()->Mat;
//		_pieckedRitem->Geo = e.get()->Geo;
//		_pieckedRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//		_pieckedRitem->Visible = FALSE;
//		_pieckedRitem->IndexCount = 0;
//		_pieckedRitem->StartIndexLocation = 0;
//		_pieckedRitem->BaseVertexLocation = 0;
//
//		mPickedRitem.push_back(std::move(_pieckedRitem));
//	}
//
//	for (auto& e : mPickedRitem)
//	{
//		auto _pieckedRitem = std::make_unique<RenderItem>();
//		*_pieckedRitem.get() = *e;
//
//		UTIL.Get_Ritemvec().push_back(std::move(_pieckedRitem));
//		UTIL.Get_Drawlayer((int)DrawLayer::DL_HIGHLIGHT).push_back(e);
//	}
//}
