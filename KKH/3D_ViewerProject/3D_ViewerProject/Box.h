#pragma once

#include "Object.h"

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
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* cmdList,
		const void* initData,
		UINT64 byteSize,
		Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer);
};