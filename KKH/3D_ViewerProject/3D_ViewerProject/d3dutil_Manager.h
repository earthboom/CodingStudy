#pragma once

#include "Unique_Singleton.h"

#define D3DUTIL d3dutil_Mananger::GetInstnace()

class d3dutil_Mananger : public CSingleton<d3dutil_Mananger>
{
public:
	explicit d3dutil_Mananger(void);
	d3dutil_Mananger(const d3dutil_Mananger&) = delete;
	d3dutil_Mananger& operator = (const d3dutil_Mananger&) = delete;
	~d3dutil_Mananger(void);

public:
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBufffer(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* cmdList,
		const void* initData,
		UINT64 byteSize,
		Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer);
};