#pragma once

#include <Windows.h>
#include <DirectXMath.h>

using namespace DirectX;

class MathHelper
{
public:
	static float	RandF(void) { return (float)(rand()) / (float)RAND_MAX; }
	static float	RandF(float a, float b) { return a + RandF() * (b - a); }
	static int		Rand(int a, int b) { return a + rand() % ((b - a) + 1); }

	template<typename T>
	static T Clamp(const T& x, const T& low, const T& high) { return x < low ? low : (x > high ? high : x); }

	static XMFLOAT4X4 Indentity4x4(void)
	{
		static XMFLOAT4X4 I
		{
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
		return I;
	}

	static DirectX::XMVECTOR ComputeNormal(DirectX::FXMVECTOR p0, DirectX::FXMVECTOR p1, DirectX::FXMVECTOR p2)
	{		
		XMVECTOR u = p1 - p0;
		XMVECTOR v = p2 - p0;

		return XMVector3Normalize(XMVector3Cross(u, v));
	}

	static XMMATRIX InverseTranspose(CXMMATRIX M)
	{
		XMMATRIX A = M;
		A.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		XMVECTOR det = XMMatrixDeterminant(A);
		return XMMatrixTranspose(XMMatrixInverse(&det, A));
	}
};