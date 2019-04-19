#pragma once

class Waves
{
public:
	explicit Waves(int m, int n, float dx, float dt, float speed, float damping);
	Waves(const Waves&) = delete;
	Waves& operator=(const Waves&) = delete;
	~Waves(void);

	int RowCount(void) const;
	int ColumnCount(void) const;
	int VertexCount(void) const;
	int TriangleCount(void) const;
	float Width(void) const;
	float Depth(void) const;

	const DirectX::XMFLOAT3& Position(int i) const { return mCurrSolution[i]; }
	const DirectX::XMFLOAT3& Normal(int i) const { return mNormals[i]; }
	const DirectX::XMFLOAT3& TangentX(int i) const { return mTangentX[i]; }

	void Update(float dt);
	void Disturb(int i, int j, float magnitude);
	
private:
	int mNumRows;
	int mNumCols;

	int mVertexCount;
	int mTriangleCount;

	float mK1, mK2, mK3;

	float mTimeStep;
	float mSpatialStep;

	std::vector<DirectX::XMFLOAT3> mPrevSolution;
	std::vector<DirectX::XMFLOAT3> mCurrSolution;
	std::vector<DirectX::XMFLOAT3> mNormals;
	std::vector<DirectX::XMFLOAT3> mTangentX;
};
