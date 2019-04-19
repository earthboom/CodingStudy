#include "stdafx.h"
#include "LandAndWave.h"

LandAndWave::LandAndWave(void)
	: Object()
	, mCurrFrameResource(nullptr)
	, mRootSignature(nullptr)
	, mWavesRtime(nullptr)
	, mCurrFrameResourceIndex(0)
	, mCbvSrvDescriptorSize(0)
	, mSunTheta(1.25f * PI)
	, mSunPhi(PI / 4.0f)
{
}

LandAndWave::~LandAndWave(void)
{
}

bool LandAndWave::Ready(void)
{
	return false;
}

bool LandAndWave::Update(const float & dt)
{
	return false;
}

bool LandAndWave::Render(const float & dt)
{
	return false;
}

void LandAndWave::OnResize(void)
{
}

void LandAndWave::BuildRootSignature(void)
{
}

void LandAndWave::BuildShaderAndInputLayer(void)
{
}

void LandAndWave::BuildLAndGeometry(void)
{
}

void LandAndWave::BuildWavesGeometryBuffers(void)
{
}

void LandAndWave::BuildPSOs(void)
{
}

void LandAndWave::BuildFrameResources(void)
{
}

void LandAndWave::BuildRenderItems(void)
{
}

void LandAndWave::DrawRenderItems(ID3D12GraphicsCommandList * cmdList, const std::vector<RenderItem*>& ritems)
{
}

void LandAndWave::OnKeyboardInput(const float & dt)
{
}

void LandAndWave::UpdateCamera(const float & dt)
{
}

void LandAndWave::UpdateObjectCBs(const float & dt)
{
}

void LandAndWave::UpdateMainPassCBs(const float & dt)
{
}

void LandAndWave::UpdateWave(const float & dt)
{
}

float LandAndWave::GetHillsHeight(float x, float z) const
{
	return 0.0f;
}

DirectX::XMFLOAT3 LandAndWave::GetHillsNormal(float x, float z) const
{
	return DirectX::XMFLOAT3();
}
