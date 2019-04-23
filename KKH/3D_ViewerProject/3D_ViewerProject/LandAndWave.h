#pragma once

#include "Object.h"
#include "Struct.h"
#include "Waves.h"
#include "Timer.h"
#include "FrameResource.h"

enum class RenderLayer : int 
{
	Opaque = 0,
	Count
};

class LandAndWave : public Object
{
public:
	explicit LandAndWave(void);
	LandAndWave(const LandAndWave&) = delete;
	LandAndWave& operator=(const LandAndWave&) = delete;
	~LandAndWave(void);

public:
	virtual bool Ready(void);
	virtual bool Update(const float& dt);
	virtual bool Render(const float& dt);

	void OnResize(void);

private:
	void BuildRootSignature(void);
	void BuildShadersAndInputLayer(void);
	void BuildLAndGeometry(void);
	void BuildWavesGeometryBuffers(void);
	void BuildPSOs(void);
	void BuildFrameResources(void);
	void BuildRenderItems(void);

	void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems);

	void OnKeyboardInput(const float& dt);
	void UpdateCamera(const float& dt);
	void UpdateObjectCBs(const float& dt);
	void UpdateMainPassCBs(const float& dt);
	void UpdateWave(const float& dt);

	float GetHillsHeight(float x, float z) const;
	DirectX::XMFLOAT3 GetHillsNormal(float x, float z) const;

private:
	std::vector<std::unique_ptr<FrameResource>> mFrameResources;
	FrameResource* mCurrFrameResource;
	int mCurrFrameResourceIndex;

	UINT mCbvSrvDescriptorSize;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;

	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeometries;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> mShaders;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> mPSOs;

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

	RenderItem* mWavesRtime;

	std::vector<std::unique_ptr<RenderItem>> mAllRitems;

	std::vector<RenderItem*> mRitemLayer[(int)RenderLayer::Count];

	std::unique_ptr<Waves> mWave;

	PassConstants mMainPassCB;
	
	float mSunTheta;
	float mSunPhi;

public:
	static std::shared_ptr<LandAndWave> Create(void);
};

typedef std::shared_ptr<LandAndWave> LAW;