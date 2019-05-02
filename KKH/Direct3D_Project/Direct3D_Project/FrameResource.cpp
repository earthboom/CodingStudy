#include "stdafx.h"
#include "FrameResource.h"

FrameResource::FrameResource(ID3D12Device * device, UINT passCount, UINT objectCount, UINT materialCount)
{
	ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(CmdListAlloc.GetAddressOf())));

	PassCB = std::make_unique<UploadBuffer<PassConstants>>(device, passCount, TRUE);
	MaterialCB = std::make_unique<UploadBuffer<MaterialConstants>>(device, materialCount, TRUE);
	ObjectCB = std::make_unique<UploadBuffer< ObjectConstants>>(device, objectCount, TRUE);	
}

FrameResource::~FrameResource(void)
{
}
