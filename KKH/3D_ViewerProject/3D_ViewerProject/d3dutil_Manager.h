#pragma once

#include "Unique_Singleton.h"

#define D3DUTIL d3dutil_Mananger::GetInstnace()

struct SubmeshGeometry
{
	UINT IndexCount = 0;
	UINT StartIndexLocaiton = 0;
	INT	 BaseVertexLocation = 0;

	//Bounding box of the geometry deinfed by this submesh.
	DirectX::BoundingBox Bounds;
};

struct MeshGeometry
{
	std::string Name;	//Give it a name so we can look it up by name.

	//System memory copies.
	//Use Blobs because the vertex / index format can be generic.
	//It is up to the client to cast appropriately.
	Microsoft::WRL::ComPtr<ID3DBlob> VertexBufferCPU = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> IndexBufferCPU = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferGPU = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferUploader = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferUploader = nullptr;

	//Data about the buffer.
	UINT VertexByteStride = 0;
	UINT VertexBufferByteSize = 0;
	DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
	UINT IndexBufferByteSize = 0;

	//A MeshGeometry mat store multiple geometires in one vertex / index buffer.
	//Use this container to define the Submesh geometries so we can draw the submeshes individually.
	std::unordered_map < std::string, SubmeshGeometry> DrawArgs;

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
		vbv.StrideInBytes = VertexByteStride;
		vbv.SizeInBytes = VertexBufferByteSize;

		return vbv;
	}

	D3D12_INDEX_BUFFER_VIEW IndexBufferView() const
	{
		D3D12_INDEX_BUFFER_VIEW ibv;
		ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
		ibv.Format = IndexFormat;
		ibv.SizeInBytes = IndexBufferByteSize;

		return ibv;
	}

	void DisposeUploaders(void)
	{
		VertexBufferUploader = nullptr;
		IndexBufferUploader = nullptr;
	}
};

class d3dutil_Mananger : public CSingleton<d3dutil_Mananger>
{
public:
	explicit d3dutil_Mananger(void);
	d3dutil_Mananger(const d3dutil_Mananger&) = delete;
	d3dutil_Mananger& operator = (const d3dutil_Mananger&) = delete;
	~d3dutil_Mananger(void);

public:
	static UINT CalcConstantBufferByteSize(UINT byteSize){	return (byteSize + 255) & ~255;	}
	
	static Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* cmdList,
		const void* initData,
		UINT64 byteSize,
		Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer);

	static Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
		const std::wstring& filename,
		const D3D_SHADER_MACRO* defines,
		const std::string& entrypoint,
		const std::string& target);


public:
	void OnResize(void);

private:
	
};