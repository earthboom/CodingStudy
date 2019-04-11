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

void d3dutil_Mananger::BuildBoxGeometry(void)
{
	std::array<Vertex, 8> vertices = 
	{
		Vertex({ DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), DirectX::XMFLOAT4(DirectX::Colors::White)	}),
		Vertex({ DirectX::XMFLOAT3(-1.0f, +1.0f, -1.0f), DirectX::XMFLOAT4(DirectX::Colors::Black)	}),
		Vertex({ DirectX::XMFLOAT3(+1.0f, +1.0f, -1.0f), DirectX::XMFLOAT4(DirectX::Colors::Red)	}),
		Vertex({ DirectX::XMFLOAT3(+1.0f, -1.0f, -1.0f), DirectX::XMFLOAT4(DirectX::Colors::Green)	}),
		Vertex({ DirectX::XMFLOAT3(-1.0f, -1.0f, +1.0f), DirectX::XMFLOAT4(DirectX::Colors::Blue)	}),
		Vertex({ DirectX::XMFLOAT3(-1.0f, +1.0f, +1.0f), DirectX::XMFLOAT4(DirectX::Colors::Yellow)	}),
		Vertex({ DirectX::XMFLOAT3(+1.0f, +1.0f, +1.0f), DirectX::XMFLOAT4(DirectX::Colors::Cyan)	}),
		Vertex({ DirectX::XMFLOAT3(+1.0f, -1.0f, +1.0f), DirectX::XMFLOAT4(DirectX::Colors::Magenta)}),
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
	
	//VertexBufferGPU = CreateDefaultBuffer(GRAPHIC_MGR.Get_Graphic()->Get_Device().Get(), GRAPHIC_MGR.Get_Graphic()->Get_CommandList().Get(), vertices, vbByteSize, VertexBufferUploader);
}
