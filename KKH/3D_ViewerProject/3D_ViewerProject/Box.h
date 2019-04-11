#pragma once

#include "Object.h"
#include "UploadBuffer.h"
#include "Struct.h"

class Box : public Object
{
protected:
	explicit Box(void);
	virtual ~Box(void);

public:
	virtual bool Ready(void);
	virtual bool Update(const float& dt);
	virtual bool Render(const float& dt);

private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;

	std::unique_ptr<UploadBuffer<ObjectConstants>> mObjectCB = nullptr;
	std::unique_ptr<MeshGeometry> mBoxGeo = nullptr;

	Microsoft::WRL::ComPtr<ID3DBlob> mvsByteCode = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> mpsByteCode = nullptr;

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayour;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> mPSO = nullptr;

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* cmdList,
		const void* initData,
		UINT64 byteSize,
		Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer);

	void BuildDescriptorHeaps(void);
	void BuildConstantBuffers(void);
	void BuildRootSignature(void);
	void BuildShadersAndInputLayout(void);
	void BuildBoxGeometry(void);
	void BuildPSO(void);
};