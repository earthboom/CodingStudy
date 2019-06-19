#pragma once

class BlurFilter
{
public:
	explicit BlurFilter(void);
	explicit BlurFilter(ID3D12Device* device, UINT width, UINT height, DXGI_FORMAT format);
	BlurFilter(const BlurFilter&) = delete;
	BlurFilter& operator=(const BlurFilter&) = delete;
	~BlurFilter(void);

	ID3D12Resource* Output(void);

	void BuildDescriptors(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDps, CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuDps, UINT dpsSize);
	
	void OnResize(UINT newWidth, UINT newHeight);
	
	void Execute(ID3D12GraphicsCommandList* cmdList,
		ID3D12RootSignature* rootSig,
		ID3D12PipelineState* horzBlurPSO,
		ID3D12PipelineState* vertBlurPSO,
		ID3D12Resource* input,
		int blurCnt);

private:
	std::vector<float> CalcGaussWights(float sigma);

	void BuildDescriptors(void);
	void BuildResource(void);

private:
	const int MaxBlurRadius;

	ID3D12Device* m_pDevice;

	UINT mWidth;
	UINT mHeight;
	
	DXGI_FORMAT mFormat;

	CD3DX12_CPU_DESCRIPTOR_HANDLE mBlur0CpuSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mBlur0CpuUav;

	CD3DX12_CPU_DESCRIPTOR_HANDLE mBlur1CpuSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mBlur1CpuUav;

	CD3DX12_GPU_DESCRIPTOR_HANDLE mBlur0GpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mBlur0GpuUav;

	CD3DX12_GPU_DESCRIPTOR_HANDLE mBlur1GpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mBlur1GpuUav;

	Microsoft::WRL::ComPtr<ID3D12Resource> mBlurMap0;
	Microsoft::WRL::ComPtr<ID3D12Resource> mBlurMap1;
};