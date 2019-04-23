#pragma once

#include "Object.h"
#include "UploadBuffer.h"
#include "Struct.h"
#include "FrameResource.h"

class BoxScene : public Object
{
public:
	explicit BoxScene(void);
	virtual ~BoxScene(void);

public:
	virtual bool Ready(void);
	virtual bool Update(const float& dt);
	virtual bool Render(const float& dt);

	void OnResize(void);

private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;

	std::unique_ptr<UploadBuffer<ObjectConstants>> mObjectCB = nullptr;
	std::unique_ptr<MeshGeometry> mBoxGeo = nullptr;

	Microsoft::WRL::ComPtr<ID3DBlob> mvsByteCode = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> mpsByteCode = nullptr;

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> mPSO = nullptr;

	//float mTheta = 1.5f * PI;
	//float mPhi = DirectX::XM_PIDIV4;
	//float mRadius = 5.0f;

private:
	void BuildDescriptorHeaps(void);
	void BuildConstantBuffers(void);
	void BuildRootSignature(void);
	void BuildShadersAndInputLayout(void);
	void BuildBoxGeometry(void);
	void BuildPSO(void);

//public:
//	float& Get_Theta(void) { return mTheta; }
//	float& Get_Phi(void) { return mPhi; }
//	float& Get_Radius(void) { return mRadius; }

public:
	static std::shared_ptr<BoxScene> Create(void);
};

typedef std::shared_ptr<BoxScene> PBOX;