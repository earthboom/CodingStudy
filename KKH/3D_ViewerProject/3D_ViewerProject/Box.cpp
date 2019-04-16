#include "stdafx.h"
#include "Box.h"
#include "Graphic_Manager.h"
#include "Function.h"

Box::Box(void)
	: Object()
{
}

Box::~Box(void)
{
}

bool Box::Ready(void)
{
	BuildDescriptorHeaps();
	BuildConstantBuffers();
	BuildRootSignature();
	BuildShadersAndInputLayout();
	BuildBoxGeometry();
	BuildPSO();

	return TRUE;
}

bool Box::Update(const float & dt)
{
	//Convert Shperical to Cartesian coordinates.
	float x = mRadius * sinf(mPhi) * cosf(mTheta);
	float z = mRadius * sinf(mPhi) * sinf(mTheta);
	float y = mRadius * cosf(mPhi);

	//Build the view matrix.
	DirectX::XMVECTOR pos = DirectX::XMVectorSet(x, y, z, 1.0f);
	DirectX::XMVECTOR target = DirectX::XMVectorZero();
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(pos, target, up);
	DirectX::XMStoreFloat4x4(&mView, view);

	DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&mWorld);
	DirectX::XMMATRIX proj = DirectX::XMLoadFloat4x4(&mProj);
	DirectX::XMMATRIX worldViewProj = world * view * proj;

	//Update the constant buffer with the lastest worldViewProj matrix.
	ObjectConstants objConstants;
	DirectX::XMStoreFloat4x4(&objConstants.World, DirectX::XMMatrixTranspose(worldViewProj));
	mObjectCB->CopyData(0, objConstants);

	return TRUE;
}

bool Box::Render(const float & dt)
{
	ThrowIfFailed(GRAPHIC->Get_CommandAllocator()->Reset());

	ThrowIfFailed(GRAPHIC->Get_CommandList()->Reset(GRAPHIC->Get_CommandAllocator().Get(), mPSO.Get()));

	GRAPHIC->Get_CommandList()->RSSetViewports(1, &GRAPHIC->Get_ScreenViewport());
	GRAPHIC->Get_CommandList()->RSSetScissorRects(1, &GRAPHIC->Get_ScissorRect());

	//Indicate a state transition on the resource usage.
	GRAPHIC->Get_CommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		GRAPHIC->Get_CurrentBackBuffer_Resource(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	//Clear the back buffer and depth buffer.
	GRAPHIC->Get_CommandList()->ClearRenderTargetView(GRAPHIC->Get_CurrentBackBufferView_Handle(), DirectX::Colors::LightSteelBlue, 0, nullptr);
	GRAPHIC->Get_CommandList()->ClearDepthStencilView(GRAPHIC->Get_DepthStencilView_Handle(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	//Specify the buffers we are going to render to.
	GRAPHIC->Get_CommandList()->OMSetRenderTargets(1, &GRAPHIC->Get_CurrentBackBufferView_Handle(), TRUE, &GRAPHIC->Get_DepthStencilView_Handle());

	ID3D12DescriptorHeap* descriptorHeaps[] = { mCbvHeap.Get() };
	GRAPHIC->Get_CommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	GRAPHIC->Get_CommandList()->SetGraphicsRootSignature(mRootSignature.Get());

	GRAPHIC->Get_CommandList()->IASetVertexBuffers(0, 1, &mBoxGeo->VertexBufferView());
	GRAPHIC->Get_CommandList()->IASetIndexBuffer(&mBoxGeo->IndexBufferView());
	GRAPHIC->Get_CommandList()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	GRAPHIC->Get_CommandList()->SetGraphicsRootDescriptorTable(0, mCbvHeap->GetGPUDescriptorHandleForHeapStart());

	GRAPHIC->Get_CommandList()->DrawIndexedInstanced(mBoxGeo->DrawArgs["box"].IndexCount, 1, 0, 0, 0);

	//Indicate a state transition on the resource usage.
	GRAPHIC->Get_CommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		GRAPHIC->Get_CurrentBackBuffer_Resource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	//Done recording commands.
	ThrowIfFailed(GRAPHIC->Get_CommandList()->Close());

	//Add the command list to the queue for execution.
	ID3D12CommandList* cmdsLists[] = { GRAPHIC->Get_CommandList().Get() };
	GRAPHIC->Get_CommandQueue()->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	//swap the back and front buffer.
	ThrowIfFailed(GRAPHIC->Get_SwapChain()->Present(0, 0));

	GRAPHIC->Get_Set_CurrBackBuffer() = (GRAPHIC->Get_Set_CurrBackBuffer() + 1) % GRAPHIC->Get_SwapChainBufferCount();

	GRAPHIC->FlushCommandQueue();

	return TRUE;
}

void Box::OnResize(void)
{
	DirectX::XMMATRIX p = DirectX::XMMatrixPerspectiveFovLH(0.25f * PI, AspectRatio(), 1.0f, 1000.0f);
	DirectX::XMStoreFloat4x4(&mProj, p);
}

void Box::BuildDescriptorHeaps(void)
{
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors = 1;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;
	ThrowIfFailed(GRAPHIC->Get_Device()->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&mCbvHeap)));
}

void Box::BuildConstantBuffers(void)
{
	mObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(GRAPHIC->Get_Device().Get(), 1, TRUE);
	
	UINT objCBByteSize = d3dutil_Mananger::CalcConstantBufferByteSize(sizeof(ObjectConstants));

	D3D12_GPU_VIRTUAL_ADDRESS cbAddress = mObjectCB->Resource()->GetGPUVirtualAddress();

	//Offset to the ith object constant buffer in the buffer
	int boxCBufIndex = 0;
	cbAddress += boxCBufIndex * objCBByteSize;

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = d3dutil_Mananger::CalcConstantBufferByteSize(sizeof(ObjectConstants));

	GRAPHIC->Get_Device()->CreateConstantBufferView(&cbvDesc, mCbvHeap->GetCPUDescriptorHandleForHeapStart());
}

void Box::BuildRootSignature(void)
{
	CD3DX12_ROOT_PARAMETER slotRootParameter[1];

	CD3DX12_DESCRIPTOR_RANGE cbvTable;
	cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable);

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(1, slotRootParameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	ThrowIfFailed(hr);

	ThrowIfFailed(GRAPHIC->Get_Device()->CreateRootSignature(0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(), IID_PPV_ARGS(&mRootSignature)));
}

void Box::BuildShadersAndInputLayout(void)
{
	HRESULT hr = S_OK;

	mvsByteCode = d3dutil_Mananger::CompileShader(L"..\\Shaders\\color.hlsl", nullptr, "VS", "vs_5_0");
	mpsByteCode = d3dutil_Mananger::CompileShader(L"..\\Shaders\\color.hlsl", nullptr, "PS", "ps_5_0");

	mInputLayout =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};
}

void Box::BuildBoxGeometry(void)
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
		4, 1, 0,

		3, 2, 6,	//right face
		3, 6, 7,

		1, 5, 6,	//top face
		1, 6, 2,

		4, 0, 3,	//bottom face
		4, 3, 7,
	};

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(uint16_t);

	mBoxGeo = std::make_unique<MeshGeometry>();
	mBoxGeo->Name = "boxGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &mBoxGeo->VertexBufferCPU));
	CopyMemory(mBoxGeo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &mBoxGeo->IndexBufferCPU));
	CopyMemory(mBoxGeo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	mBoxGeo->VertexBufferGPU = d3dutil_Mananger::CreateDefaultBuffer(GRAPHIC->Get_Device().Get(), GRAPHIC->Get_CommandList().Get(), vertices.data(), vbByteSize, mBoxGeo->VertexBufferUploader);
	mBoxGeo->IndexBufferGPU = d3dutil_Mananger::CreateDefaultBuffer(GRAPHIC->Get_Device().Get(), GRAPHIC->Get_CommandList().Get(), indices.data(), ibByteSize, mBoxGeo->IndexBufferUploader);

	mBoxGeo->VertexByteStride = sizeof(Vertex);
	mBoxGeo->VertexBufferByteSize = vbByteSize;
	mBoxGeo->IndexFormat = DXGI_FORMAT_R16_UINT;
	mBoxGeo->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocaiton = 0;
	submesh.BaseVertexLocation = 0;

	mBoxGeo->DrawArgs["box"] = submesh;
}

void Box::BuildPSO(void)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };
	psoDesc.pRootSignature = mRootSignature.Get();
	
	psoDesc.VS = { reinterpret_cast<BYTE*>(mvsByteCode->GetBufferPointer()), mvsByteCode->GetBufferSize() };
	psoDesc.PS = { reinterpret_cast<BYTE*>(mpsByteCode->GetBufferPointer()), mpsByteCode->GetBufferSize() };

	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = GRAPHIC->Get_BackBufferFormat();
	psoDesc.SampleDesc.Count = GRAPHIC->Get_4xMsaaState() ? 4 : 1;
	psoDesc.SampleDesc.Quality = GRAPHIC->Get_4xMsaaState() ? (GRAPHIC->Get_4xMsaaQuality() - 1) : 0;
	psoDesc.DSVFormat = GRAPHIC->Get_DepthStencilFormat();

	ThrowIfFailed(GRAPHIC->Get_Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPSO)));
}

std::shared_ptr<Box> Box::Create(void)
{
	auto pbox = std::make_shared<Box>();
	if (!pbox)	return nullptr;

	pbox->Ready();
	return pbox;
}

