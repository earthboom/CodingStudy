#include "stdafx.h"
#include "FrameResource.h"

FrameResource::FrameResource(ID3D12Device * device)//, UINT passCount, UINT objectCount, UINT waveCount)
{
	ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(CmdListAlloc.GetAddressOf())));

	//PassCB = std::make_unique<UploadBuffer<PassConstants>>(device, passCount, TRUE);
	//ObjectCB = std::make_unique<UploadBuffer< ObjectConstants>>(device, objectCount, TRUE);

	//WavesVB = std::make_unique<UploadBuffer<Vertex>>(device, waveCount, FALSE);
}

FrameResource::~FrameResource(void)
{
}
