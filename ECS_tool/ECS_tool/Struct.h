#pragma once

namespace ECS_tool
{
	struct RenderItem
	{
		RenderItem(void) = default;
		RenderItem(const RenderItem&) = delete;

		D3D12_VERTEX_BUFFER_VIEW	VB_View;
		D3D12_INDEX_BUFFER_VIEW		IB_View;
	};
}