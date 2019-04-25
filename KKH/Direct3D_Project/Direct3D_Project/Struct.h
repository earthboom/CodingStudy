#pragma once

#include "d3dutil_Manager.h"

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