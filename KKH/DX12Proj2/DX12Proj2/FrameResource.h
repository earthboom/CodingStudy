#pragma once

struct FrameResource
{
public:
	explicit FrameResource(ID3D12Device* device, UINT passCount, UINT maxInstanceCount, UINT objectCount, UINT materialCount);
	//explicit FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount, UINT waveVertCount);
	//explicit FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount);
	explicit FrameResource(ID3D12Device* device, UINT passCount, UINT maxInstanceCount, UINT materialCount);

	FrameResource(const FrameResource&) = delete;
	FrameResource& operator=(const FrameResource&) = delete;
	~FrameResource(void);

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CmdListAlloc;

	std::unique_ptr<UploadBuffer<PassConstants>> PassCB = nullptr;
	//std::unique_ptr<UploadBuffer<MaterialConstants>> MaterialCB = nullptr;
	std::unique_ptr<UploadBuffer<ObjectConstants>> ObjectCB = nullptr;	

	std::unique_ptr<UploadBuffer<MaterialData>> MaterialBuffer = nullptr;

	std::unique_ptr<UploadBuffer<InstanceData>> InstanceBuffer = nullptr;

	std::unique_ptr<UploadBuffer<VERTEX>> WavesVB = nullptr;

	UINT64 Fence = 0;
};