#pragma once

#include <MathHelper.h>
#include "UploadBuffer.h"

struct Vertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT4 Color;
};

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

struct FrameResource
{
	FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount)
	{
		ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(CmdListAlloc.GetAddressOf())));

		PassCB = std::make_unique<UploadBuffer<PassConstants>>(device, passCount, TRUE);
		ObjectCB = std::make_unique<UploadBuffer< ObjectConstants>>(device, objectCount, TRUE);
	}

	FrameResource(const FrameResource&) = delete;
	FrameResource& operator=(const FrameResource&) = delete;
	~FrameResource(void) {}

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CmdListAlloc;

	std::unique_ptr<UploadBuffer<PassConstants>> PassCB = nullptr;
	std::unique_ptr<UploadBuffer<ObjectConstants>> ObjectCB = nullptr;

	UINT64 Fence = 0;
};

struct RenderItem
{
	RenderItem(void) = default;

	DirectX::XMFLOAT4X4 World = MathHelper::Indentity4x4();

	int NumFramesDirty = NumFrameResources;
	
	UINT objCBIndex = -1;

	MeshGeometry* Geo = nullptr;

	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	int BaseVertexLocation = 0;
};