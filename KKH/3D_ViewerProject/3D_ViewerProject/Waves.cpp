#include "stdafx.h"
#include "Waves.h"
#include <ppl.h>

Waves::Waves(int m, int n, float dx, float dt, float speed, float damping)
	: mNumRows(m), mNumCols(n)
	, mVertexCount(m * n)
	, mTriangleCount((m-1)*(n-1)*2)
	, mK1(0.0f), mK2(0.0f), mK3(0.0f)
	, mTimeStep(0.0f), mSpatialStep(0.0f)
{
	float d = damping * dt + 2.0f;
	float e = (speed * speed) * (dt * dt) / (dx * dx);

	mK1 = (damping * dt - 2.0f) / d;
	mK2 = (4.0f - 8.0f * e) / d;
	mK3 = (2.0f * e) / d;

	mPrevSolution.resize(m * n);
	mCurrSolution.resize(m * n);
	mNormals.resize(m * n);
	mTangentX.resize(m * n);

	float halfWidth = (n - 1) * dx * 0.5f;
	float halfDepth = (m - 1) * dx * 0.5f;
	for (int i = 0; i < m; ++i)
	{
		float z = halfDepth - i * dx;
		for (int j = 0; j < n; ++j)
		{
			float x = -halfWidth + j * dx;

			mPrevSolution[i * n + j]	= DirectX::XMFLOAT3(x, 0.0f, z);
			mCurrSolution[i * n + j]	= DirectX::XMFLOAT3(x, 0.0f, z);
			mNormals[i * n + j]			= DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
			mTangentX[i * n + j]		= DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
		}
	}
}

Waves::~Waves(void)
{
}

int Waves::RowCount(void) const
{
	return mNumRows;
}

int Waves::ColumnCount(void) const
{
	return mNumCols;
}

int Waves::VertexCount(void) const
{
	return mVertexCount;
}

int Waves::TriangleCount(void) const
{
	return mTriangleCount;
}

float Waves::Width(void) const
{
	return mNumCols * mSpatialStep;
}

float Waves::Depth(void) const
{
	return mNumRows * mSpatialStep;
}

void Waves::Update(float dt)
{
	static float t = 0;

	t += dt;
	if (t >= mTimeStep)
	{
		concurrency::parallel_for(1, mNumRows - 1, [this](int i)
		//for(int i = 1; i < mNumRowsl - 1; ++i)
		{
			for (int j = 1; j < mNumCols - 1; ++j)
			{
				mPrevSolution[i * mNumCols + j].y =
					mK1 * mPrevSolution[i * mNumCols + j].y + 
					mK2 * mCurrSolution[i * mNumCols + j].y +
					mK3 * (mCurrSolution[(i + 1) * mNumCols + j].y + 
							mCurrSolution[(i - 1) * mNumCols + j].y +
							mCurrSolution[i * mNumCols + j + 1].y + 
							mCurrSolution[i * mNumCols + j - 1].y);
			}
		});

		std::swap(mPrevSolution, mCurrSolution);

		t = 0.0f;

		concurrency::parallel_for(1, mNumRows - 1, [this](int i)
		//for(int i = 1; i < mNumRowsl - 1; ++i)
		{
			for (int j = 1; j < mNumCols - 1; ++j)
			{
				float l = mCurrSolution[i * mNumCols + j - 1].y;
				float r = mCurrSolution[i * mNumCols + j + 1].y;
				float t = mCurrSolution[(i-1) * mNumCols + j].y;
				float b = mCurrSolution[(i+1) * mNumCols + j].y;

				mNormals[i * mNumCols + j].x = -r + l;
				mNormals[i * mNumCols + j].y = 2.0f * mSpatialStep;
				mNormals[i * mNumCols + j].z = b - t;

				DirectX::XMVECTOR n = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&mNormals[i * mNumCols + j]));
				DirectX::XMStoreFloat3(&mNormals[i * mNumCols + j], n);

				mTangentX[i * mNumCols + j] = DirectX::XMFLOAT3(2.0f * mSpatialStep, r - 1, 0.0f);
				DirectX::XMVECTOR T = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&mTangentX[i * mNumCols + j]));
				DirectX::XMStoreFloat3(&mTangentX[i * mNumCols + j], T);
			}
		});
	}
}

void Waves::Disturb(int i, int j, float magnitude)
{
	assert(i > 1 && i < mNumRows - 2);
	assert(j > 1 && j < mNumRows - 2);

	float halfMag = 0.5f * magnitude;

	mCurrSolution[i * mNumCols + j].y		+= magnitude;
	mCurrSolution[i * mNumCols + j + 1].y	+= halfMag;
	mCurrSolution[i * mNumCols + j - 1].y	+= halfMag;
	mCurrSolution[(i + 1) * mNumCols + j].y += halfMag;
	mCurrSolution[(i - 1) * mNumCols + j].y += halfMag;
}
