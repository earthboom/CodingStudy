#include "stdafx.h"
#include "d3dutil_Manager.h"
#include "Graphic_Manager.h"
#include "Function.h"
#include "Struct.h"

d3dutil_Mananger::d3dutil_Mananger(void)
{
}

d3dutil_Mananger::~d3dutil_Mananger(void)
{
}

Microsoft::WRL::ComPtr<ID3D12Resource> d3dutil_Mananger::CreateDefaultBufffer(ID3D12Device * device, ID3D12GraphicsCommandList * cmdList, const void * initData, UINT64 byteSize, Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer)
{
	Microsoft::WRL::ComPtr<ID3D12Resource> defaultBuffer;

	//Create the actual default buffer resource.
	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(defaultBuffer.GetAddressOf())));

	//In orfer to copy CPU memory data into our default buffer, 
	//we need to create an intermediate upload heap.
	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(uploadBuffer.GetAddressOf())));

	//Describe the data we want to copy into the default buffer.
	D3D12_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pData = initData;
	subResourceData.RowPitch = byteSize;
	subResourceData.SlicePitch = subResourceData.RowPitch;

	//Schedule to copy the data to the default buffer resource.
	//At high level, the helper function UpdateSubResources will copy the CPU memory into the intermediate upload heap.
	//Then, using ID3D12CommandList::CopySubresourceRegion, the intermediate upload heap data will be copied to mBuffer.
	cmdList->ResourceBarrier(1,
		&CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(),
			D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_STATE_COPY_DEST));

	UpdateSubresources<1>(cmdList, defaultBuffer.Get(), uploadBuffer.Get(), 0, 0, 1, &subResourceData);
	
	cmdList->ResourceBarrier(1,
		&CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_GENERIC_READ));

	//Note 
	//uploadBuffer has to be kept alive after the above function calls because 
	//the commnad list has not been executed yet that performs the actual copy.
	//The caller can Release the uploadBuffer after it knows the copy has been executed.
		
	return defaultBuffer;
}

void d3dutil_Mananger::BuildBoxGeometry(void)
{
	std::array<Vertex, 8> vertices = 
	{
		Vertex({ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(DirectX::Colors::White)	}),
		Vertex({ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(DirectX::Colors::Black)	}),
		Vertex({ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(DirectX::Colors::Red)		}),
		Vertex({ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(DirectX::Colors::Green)	}),
		Vertex({ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(DirectX::Colors::Blue)		}),
		Vertex({ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(DirectX::Colors::Yellow)	}),
		Vertex({ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(DirectX::Colors::Cyan)		}),
		Vertex({ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(DirectX::Colors::Magenta)	}),
	};
	
	std::array<uint16_t, 36> indices =
	{
		0, 1, 2,	//front face 
		0, 2, 3,

		4, 6, 5,	//back face
		4, 7, 6,

		4, 5, 1,	//left face
		4, 7, 6,

		4, 5, 1,	//right face
		4, 1, 0,

		3, 2, 6,	//top face
		3, 6, 7,

		4, 0, 3,	//bottom face
		4, 3, 7,
	};

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(uint16_t);

	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferUploader = nullptr;
	
	//VertexBufferGPU = CreateDefaultBufffer(GRAPHIC_MGR.Get_Graphic()->Get_Device().Get(), GRAPHIC_MGR.Get_Graphic()->Get_CommandList().Get(), vertices, vbByteSize, VertexBufferUploader);
}
