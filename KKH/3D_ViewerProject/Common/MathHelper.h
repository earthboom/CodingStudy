#pragma once

#include <Windows.h>
#include <DirectXMath.h>

class MathHelper
{
public:
	template<typename T>
	static T Clamp(const T& x, const T& low, const T& high) { return x < low ? low : (x > high ? high : x); }

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