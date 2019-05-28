#pragma once

#include "MathHelper.h"

#define D3DUTIL d3dutil_Manager::GetInstnace()

class d3dutil_Manager : public CSingleton<d3dutil_Manager>
{
public:
	explicit d3dutil_Manager(void);
	d3dutil_Manager(const d3dutil_Manager&) = delete;
	d3dutil_Manager& operator = (const d3dutil_Manager&) = delete;
	~d3dutil_Manager(void);

public:
	static UINT CalcConstantBufferByteSize(UINT byteSize){	return (byteSize + 255) & ~255;	}

	static Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* cmdList,
		const void* initData,
		UINT64 byteSize,
		Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer);

	static Microsoft::WRL::ComPtr<ID3DBlob> LoadBinary(const std::wstring& filename);

	static Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
		const std::wstring& filename,
		const D3D_SHADER_MACRO* defines,
		const std::string& entrypoint,
		const std::string& target);
};