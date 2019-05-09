#include "stdafx.h"
#include "d3dutil_Manager.h"
#include "Graphic_Manager.h"
#include "Function.h"
#include "FrameResource.h"

d3dutil_Mananger::d3dutil_Mananger(void)	
{

}

d3dutil_Mananger::~d3dutil_Mananger(void)
{
}

Microsoft::WRL::ComPtr<ID3D12Resource> d3dutil_Mananger::CreateDefaultBuffer(
	ID3D12Device * device, 
	ID3D12GraphicsCommandList * cmdList, 
	const void * initData, 
	UINT64 byteSize, 
	Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer)
{
	Microsoft::WRL::ComPtr<ID3D12Resource> defaultBuffer;

	//Create the actual default buffer resource.
	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(byteSize),
		D3D12_RESOURCE_STATE_COMMON, nullptr,
		IID_PPV_ARGS(defaultBuffer.GetAddressOf())));

	//In orfer to copy CPU memory data into our default buffer, 
	//we need to create an intermediate upload heap.
	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(byteSize),
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(uploadBuffer.GetAddressOf())));

	//Describe the data we want to copy into the default buffer.
	D3D12_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pData = initData;
	subResourceData.RowPitch = byteSize;
	subResourceData.SlicePitch = subResourceData.RowPitch;

	//Schedule to copy the data to the default buffer resource.
	//At high level, the helper function UpdateSubResources will copy the CPU memory into the intermediate upload heap.
	//Then, using ID3D12CommandList::CopySubresourceRegion, the intermediate upload heap data will be copied to mBuffer.
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(),
			D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));

	UpdateSubresources<1>(cmdList, defaultBuffer.Get(), uploadBuffer.Get(), 0, 0, 1, &subResourceData);

	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));

	//Note 
	//uploadBuffer has to be kept alive after the above function calls because 
	//the commnad list has not been executed yet that performs the actual copy.
	//The caller can Release the uploadBuffer after it knows the copy has been executed.

	return defaultBuffer;
}

Microsoft::WRL::ComPtr<ID3DBlob> d3dutil_Mananger::CompileShader(const std::wstring & filename, const D3D_SHADER_MACRO * defines, const std::string & entrypoint, const std::string & target)
{
	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	HRESULT hr = S_OK;

	Microsoft::WRL::ComPtr<ID3DBlob> byteCode = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errors = nullptr;

	hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entrypoint.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors);

	if (errors != nullptr)
		OutputDebugStringA((char*)errors->GetBufferPointer());

	ThrowIfFailed(hr);

	return byteCode;
}

//bool d3dutil_Mananger::Object_Ready(void)
//{
//	for (auto& objmap : allObj_Update_vec)
//	{
//		for (auto& obj : *objmap)
//		{
//			if (!obj.second->Ready())
//				return FALSE;
//		}
//	}
//
//	return TRUE;
//}
//
//bool d3dutil_Mananger::Object_Update(const float& dt)
//{
//	for (auto& objmap : allObj_Update_vec)
//	{
//		for (auto& obj : *objmap)
//		{
//			if (!obj.second->Update(dt))
//				return FALSE;
//		}
//	}
//
//	return TRUE;
//}
//
//bool d3dutil_Mananger::Object_Render(const float& dt)
//{
//	for (auto& objmap : allObj_Update_vec)
//	{
//		for (auto& obj : *objmap)
//		{
//			if (!obj.second->Render(dt))
//				return FALSE;
//		}
//	}
//
//	return TRUE;
//}
