#pragma once

template<typename T>
class UploadBuffer
{
public:
	UploadBuffer(ID3D12Device* device, UINT elementCount, bool isConstantBuffer)
		: mIsConstantBuffer(isConstantBuffer)
	{
		mElementByteSize = sizeof(T);

		//Constant buffer elements needs to be multiples of 256 bytes.
		//This is because the hardware can only view constant data at m*256 byte offsets and of n*256 byte lengths.
		if (isConstantBuffer)
			mElementByteSize = d3dutil::CalcConstantBufferByteSize(sizeof(T));

		ThrowIfFailed(DEVICE->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(mElementByteSize * elementCount),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&mUploadBuffer)));

		ThrowIfFailed(mUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mMappedData)));
	}

	UploadBuffer(const UploadBuffer&) = delete;
	UploadBuffer& operator = (const UploadBuffer&) = delete;
	~UploadBuffer(void)
	{
		if (mUploadBuffer != nullptr)
			mUploadBuffer->Unmap(0, nullptr);

		mMappedData = nullptr;
	}

	ID3D12Resource* Resource(void) const {	return mUploadBuffer.Get();	}
	void CopyData(int elementIndex, const T& data) { memcpy(&mMappedData[elementIndex * mElementByteSize], &data, sizeof(T)); }

private:
	Microsoft::WRL::ComPtr<ID3D12Resource>	mUploadBuffer;
	BYTE* mMappedData = nullptr;
	UINT mElementByteSize = 0;
	bool mIsConstantBuffer = FALSE;
};