#pragma once

#include "d3dutil_Manager.h"
#include "UploadBuffer.h"
#include "../Common/MathHelper.h"

struct ObjectConstants
{
	//DirectX::XMFLOAT4X4	WorldViewPorj = MathHelper::Indentity4x4();
	DirectX::XMFLOAT4X4	World = MathHelper::Indentity4x4();
};

struct PassConstants
{
	DirectX::XMFLOAT4X4 View = MathHelper::Indentity4x4();
	DirectX::XMFLOAT4X4 InvView = MathHelper::Indentity4x4();
	DirectX::XMFLOAT4X4 Proj = MathHelper::Indentity4x4();
	DirectX::XMFLOAT4X4 InvProj = MathHelper::Indentity4x4();
	DirectX::XMFLOAT4X4 ViewProj = MathHelper::Indentity4x4();
	DirectX::XMFLOAT4X4 InvViewProj = MathHelper::Indentity4x4();

	DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };

	float cbPerObjectPad1 = 0.0f;

	DirectX::XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
	DirectX::XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };

	float NearZ = 0.0f;
	float FarZ = 0.0f;
	float TotalTime = 0.0f;
	float DeltaTime = 0.0f;
};

struct Vertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT4 Color;
};

struct FrameResource
{
public:
	FrameResource(ID3D12Device* device);//, UINT passCount, UINT objectCount, UINT waveCount);
	FrameResource(const FrameResource&) = delete;
	FrameResource& operator=(const FrameResource&) = delete;
	~FrameResource(void);

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CmdListAlloc;

	std::unique_ptr<UploadBuffer<PassConstants>> PassCB = nullptr;
	std::unique_ptr<UploadBuffer<ObjectConstants>> ObjectCB = nullptr;

	std::unique_ptr<UploadBuffer<Vertex>> WavesVB = nullptr;

	UINT64 Fence = 0;

public:
	void Set_Pass(ID3D12Device* device, UINT _size) { PassCB = std::make_unique<UploadBuffer<PassConstants>>(device, _size, TRUE); };
	void Set_Object(ID3D12Device* device, UINT _size) { ObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(device, _size, TRUE); };
	void Set_Wave(ID3D12Device* device, UINT _size) { WavesVB = std::make_unique<UploadBuffer<Vertex>>(device, _size, TRUE); };
};