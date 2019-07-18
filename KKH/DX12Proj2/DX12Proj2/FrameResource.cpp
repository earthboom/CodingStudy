#include "pch.h"
#include "FrameResource.h"

FrameResource::FrameResource(ID3D12Device* device, UINT passCount, UINT maxInstanceCount, UINT objectCount, UINT materialCount)
{
	ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(CmdListAlloc.GetAddressOf())));

	PassCB = std::make_unique<UploadBuffer<PassConstants>>(device, passCount, TRUE);
	ObjectCB = std::make_unique<UploadBuffer< ObjectConstants>>(device, objectCount, TRUE);
	InstanceBuffer = std::make_unique<UploadBuffer<InstanceData>>(device, maxInstanceCount, FALSE);
	MaterialBuffer = std::make_unique<UploadBuffer<MaterialData>>(device, materialCount, FALSE);

	//MaterialCB = std::make_unique<UploadBuffer<MaterialConstants>>(device, materialCount, TRUE);
	//WavesVB = std::make_unique<UploadBuffer<VERTEX>>(device, waveVertCount, FALSE);
}

//FrameResource::FrameResource(ID3D12Device * device, UINT passCount, UINT objectCount, UINT materialCount)
FrameResource::FrameResource(ID3D12Device* device, UINT passCount, UINT maxInstanceCount, UINT materialCount)
{
	ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(CmdListAlloc.GetAddressOf())));

	PassCB = std::make_unique<UploadBuffer<PassConstants>>(device, passCount, TRUE);	
	InstanceBuffer = std::make_unique<UploadBuffer<InstanceData>>(device, maxInstanceCount, FALSE);
	MaterialBuffer = std::make_unique<UploadBuffer<MaterialData>>(device, materialCount, FALSE);

	//MaterialCB = std::make_unique<UploadBuffer<MaterialConstants>>(device, materialCount, TRUE);
	//ObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(device, objectCount, TRUE);
}

FrameResource::~FrameResource(void)
{
}
