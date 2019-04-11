#pragma once

#include <MathHelper.h>

struct Vertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT4 Color;
};

struct ObjectConstants
{
	DirectX::XMFLOAT4X4	WorldViewPorj = MathHelper::Indentity4x4();
};