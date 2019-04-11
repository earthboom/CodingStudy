#pragma once

#include <Windows.h>
#include <DirectXMath.h>

class MathHelper
{
public:
	static DirectX::XMFLOAT4X4 Indentity4x4(void)
	{
		static DirectX::XMFLOAT4X4 I
		{
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
		return I;
	}
};