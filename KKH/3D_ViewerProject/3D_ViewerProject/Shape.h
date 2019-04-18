#pragma once

#include "Object.h"
#include "Struct.h"

class Shape : public Object
{
public:
	explicit Shape(void);
	Shape(const Shape&) = delete;
	Shape& operator = (const Shape&) = delete;
	~Shape(void);

public:
	virtual bool Ready(void);
	virtual bool Update(const float& dt);
	virtual bool Render(const float& dt);

	void OnResize(void);

private:
	void BuildDescriptorHeaps(void);
	void BuildConstantBufferViews(void);
	void BuildRootSignature(void);
	void BuildShadersAndInputLayout(void);
	void BuildShapeGeometry(void);
	void BuildPSO(void);
	void BuildFrameResources(void);
	void BuildRenderItems(void);
	
	void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems);

	void OnKeyboardInput(const float& dt);
	void UpdateCamera(const float& dt);

	void UpdateObjectCBs(const float& dt);
	void UpdateMainPassCB(const float& dt);

private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;
	
	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeometries;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> mShaders;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> mPSOs;

	//float mTheta;	// = 1.5f * PI;
	//float mPhi;		// = DirectX::XM_PIDIV4;
	//float mRadius;	// = 5.0f;


	std::vector<std::unique_ptr<FrameResource>> mFrameResources;
	FrameResource* mCurrFrameResource = nullptr;
	int mCurrFrameResourceIndex;	// = 0;

	std::vector<std::unique_ptr<RenderItem>> mAllRitems;

	std::vector<RenderItem*> mOpaqueRitems;
	std::vector<RenderItem> mTransparentRitems;

	PassConstants mMainPassCB;
	UINT mPassCbvOffset;

public:
	static std::shared_ptr<Shape> Create(void);
};

typedef std::shared_ptr<Shape> SHAPE;