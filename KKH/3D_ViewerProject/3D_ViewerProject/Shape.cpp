#include "stdafx.h"
#include "Shape.h"
#include "Const.h"
#include "Graphic_Manager.h"

Shape::Shape(void)
{
}

Shape::~Shape(void)
{
}

bool Shape::Ready(void)
{
	return TRUE;
}

bool Shape::Update(const float & dt)
{
	mCurrFrameResourceIndex = (mCurrFrameResourceIndex + 1) % NumFrameResources;
	mCurrFrameResource = mFrameResources[mCurrFrameResourceIndex].get();

	if (mCurrFrameResource->Fence != 0 && GRAPHIC->Get_Fence()->GetCompletedValue() < mCurrFrameResource->Fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, FALSE, FALSE , EVENT_ALL_ACCESS);
		ThrowIfFailed(GRAPHIC->Get_Fence()->SetEventOnCompletion(mCurrFrameResource->Fence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	return TRUE;
}

bool Shape::Render(const float & dt)
{
	return TRUE;
}

void Shape::BuildFrameResources(void)
{
	for (int i = 0; i < NumFrameResources; ++i)
		mFrameResources.push_back(std::make_unique<FrameResource>(GRAPHIC->Get_Device().Get(), 1, (UINT)mAllRitems.size()));
}
