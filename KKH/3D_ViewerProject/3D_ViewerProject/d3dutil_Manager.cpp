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
	
	//VertexBufferGPU = CreateDefaultBuffer(GRAPHIC_MGR.Get_Graphic()->Get_Device().Get(), GRAPHIC_MGR.Get_Graphic()->Get_CommandList().Get(), vertices, vbByteSize, VertexBufferUploader);
}
