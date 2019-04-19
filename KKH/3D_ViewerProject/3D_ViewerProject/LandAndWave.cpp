#include "stdafx.h"
#include "LandAndWave.h"
#include "GeometryGenerator.h"
#include "Timer_Manager.h"

LandAndWave::LandAndWave(void)
	: Object()
	, mCurrFrameResource(nullptr)
	, mRootSignature(nullptr)
	, mWavesRtime(nullptr)
	, mCurrFrameResourceIndex(0)
	, mCbvSrvDescriptorSize(0)
	, mSunTheta(1.25f * PI)
	, mSunPhi(PI / 4.0f)
{
}

LandAndWave::~LandAndWave(void)
{
}

bool LandAndWave::Ready(void)
{
	TIME_MGR.Ready_Timer(L"WaveTimer");
	TIME_MGR.Get_Start(L"WaveTimer");

	mWave = std::make_unique<Waves>(128, 128, 1.0f, 0.03f, 4.0f, 0.2f);

	BuildRootSignature();
	BuildShadersAndInputLayer();
	BuildLAndGeometry();
	BuildWavesGeometryBuffers();
	BuildRenderItems();
	BuildRenderItems();
	BuildFrameResources();
	BuildPSOs();
	

	return TRUE;
}

bool LandAndWave::Update(const float & dt)
{
	TIME_MGR.Get_Tick(L"WaveTimer");

	OnKeyboardInput(dt);
	UpdateCamera(dt);

	mCurrFrameResourceIndex = (mCurrFrameResourceIndex + 1) % NumFrameResources;
	mCurrFrameResource = mFrameResources[mCurrFrameResourceIndex].get();

	if (mCurrFrameResource->Fence != 0 && GRAPHIC->Get_Fence()->GetCompletedValue() < mCurrFrameResource->Fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
		ThrowIfFailed(GRAPHIC->Get_Fence()->SetEventOnCompletion(mCurrFrameResource->Fence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	UpdateObjectCBs(dt);
	UpdateMainPassCBs(dt);
	UpdateWave(dt);

	return TRUE;
}

bool LandAndWave::Render(const float & dt)
{
	auto cmdListAlloc = mCurrFrameResource->CmdListAlloc;

	ThrowIfFailed(cmdListAlloc->Reset());

	if (mIsWireframe)
	{
		ThrowIfFailed(GRAPHIC->Get_CommandList()->Reset(cmdListAlloc.Get(), mPSOs["opaque_wireframe"].Get()));
	}
	else
	{
		ThrowIfFailed(GRAPHIC->Get_CommandList()->Reset(cmdListAlloc.Get(), mPSOs["opaque"].Get()));
	}

	GRAPHIC->Get_CommandList()->RSSetViewports(1, &GRAPHIC->Get_ScreenViewport());
	GRAPHIC->Get_CommandList()->RSSetScissorRects(1, &GRAPHIC->Get_ScissorRect());

	GRAPHIC->Get_CommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		GRAPHIC->Get_CurrentBackBuffer_Resource(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	GRAPHIC->Get_CommandList()->ClearRenderTargetView(GRAPHIC->Get_CurrentBackBufferView_Handle(), DirectX::Colors::LightSteelBlue, 0, nullptr);
	GRAPHIC->Get_CommandList()->ClearDepthStencilView(GRAPHIC->Get_DepthStencilView_Handle(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	GRAPHIC->Get_CommandList()->OMSetRenderTargets(1, &GRAPHIC->Get_CurrentBackBufferView_Handle(), TRUE, &GRAPHIC->Get_DepthStencilView_Handle());
	
	GRAPHIC->Get_CommandList()->SetGraphicsRootSignature(mRootSignature.Get());

	auto passCB = mCurrFrameResource->PassCB->Resource();
	GRAPHIC->Get_CommandList()->SetGraphicsRootConstantBufferView(1, passCB->GetGPUVirtualAddress());

	DrawRenderItems(GRAPHIC->Get_CommandList().Get(), mRitemLayer[(int)RenderLayer::Opaque]);

	GRAPHIC->Get_CommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(GRAPHIC->Get_CurrentBackBuffer_Resource(), 
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	ThrowIfFailed(GRAPHIC->Get_CommandList()->Close());

	ID3D12CommandList* cmdsLists[] = { GRAPHIC->Get_CommandList().Get() };
	GRAPHIC->Get_CommandQueue()->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	ThrowIfFailed(GRAPHIC->Get_SwapChain()->Present(0, 0));
	GRAPHIC->Get_Set_CurrBackBuffer() = (GRAPHIC->Get_Set_CurrBackBuffer() + 1) % GRAPHIC->Get_SwapChainBufferCount();

	mCurrFrameResource->Fence = ++GRAPHIC->Get_CurrentFence();

	GRAPHIC->Get_CommandQueue()->Signal(GRAPHIC->Get_Fence().Get(), GRAPHIC->Get_CurrentFence());

	return TRUE;
}

void LandAndWave::OnResize(void)
{
	DirectX::XMMATRIX p = DirectX::XMMatrixPerspectiveFovLH(0.25f * PI, AspectRatio(), 1.0f, 1000.0f);
	DirectX::XMStoreFloat4x4(&mProj, p);
}

void LandAndWave::BuildRootSignature(void)
{
	CD3DX12_ROOT_PARAMETER slotRootParameter[2];

	slotRootParameter[0].InitAsConstantBufferView(0);
	slotRootParameter[1].InitAsConstantBufferView(1);

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(2, slotRootParameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	ThrowIfFailed(hr);

	ThrowIfFailed(GRAPHIC->Get_Device()->CreateRootSignature(0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(), IID_PPV_ARGS(mRootSignature.GetAddressOf())));
}

void LandAndWave::BuildShadersAndInputLayer(void)
{
	HRESULT hr = S_OK;

	mShaders["standardVS"] = d3dutil_Mananger::CompileShader(L"..\\Shaders\\color.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["opaquePS"] = d3dutil_Mananger::CompileShader(L"..\\Shaders\\color.hlsl", nullptr, "PS", "ps_5_1");

	mInputLayout =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};
}

void LandAndWave::BuildLAndGeometry(void)
{
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData grid = geoGen.CreateGrid(160.0f, 160.0f, 50, 50);

	std::vector<Vertex> vertices(grid.Vertices.size());
	for (size_t i = 0; i < grid.Vertices.size(); ++i)
	{
		auto& p = grid.Vertices[i].Position;
		vertices[i].Pos = p;
		vertices[i].Pos.y = GetHillsHeight(p.x, p.z);

		if (vertices[i].Pos.y < -10.0f)
			vertices[i].Color = DirectX::XMFLOAT4(1.0f, 0.96f, 0.62f, 1.0f);
		else if (vertices[i].Pos.y < 5.0f)
			vertices[i].Color = DirectX::XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
		else if (vertices[i].Pos.y < 12.0f)
			vertices[i].Color = DirectX::XMFLOAT4(0.1f, 0.48f, 0.19f, 1.0f);
		else if (vertices[i].Pos.y < 20.0f)
			vertices[i].Color = DirectX::XMFLOAT4(0.45f, 0.39f, 0.34f, 1.0f);
		else
			vertices[i].Color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);

	std::vector<std::uint16_t> indices = grid.GetIndices16();
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = "landGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = D3DUTIL.CreateDefaultBuffer(GRAPHIC->Get_Device().Get(), GRAPHIC->Get_CommandList().Get(), vertices.data(), vbByteSize, geo->VertexBufferUploader);
	geo->IndexBufferGPU  = D3DUTIL.CreateDefaultBuffer(GRAPHIC->Get_Device().Get(), GRAPHIC->Get_CommandList().Get(), indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Vertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocaiton = 0;
	submesh.BaseVertexLocation = 0;

	geo->DrawArgs["grid"] = submesh;

	mGeometries["landGeo"] = std::move(geo);
}

void LandAndWave::BuildWavesGeometryBuffers(void)
{
	std::vector<std::uint16_t> indices(3 * mWave->TriangleCount());
	assert(mWave->VertexCount() < 0x0000ffff);

	int m = mWave->RowCount();
	int n = mWave->ColumnCount();
	int k = 0;
	for (int i = 0; i < m - 1; ++i)
	{
		for (int j = 0; j < n - 1; ++j)
		{
			indices[k]		= i * n + j;
			indices[k + 1]	= i * n + j + 1;
			indices[k + 2]	= (i + 1) * n + j;

			indices[k + 3]	= (i + 1) * n + j;
			indices[k + 4]	= i * n + j + 1;
			indices[k + 5]	= (i + 1) * n + j + 1;

			k += 6;
		}
	}

	UINT vbByteSize = mWave->VertexCount() * sizeof(Vertex);
	UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = "waterGeo";

	geo->VertexBufferCPU = nullptr;
	geo->VertexBufferGPU = nullptr;

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->IndexBufferGPU = D3DUTIL.CreateDefaultBuffer(GRAPHIC->Get_Device().Get(), GRAPHIC->Get_CommandList().Get(), indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Vertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocaiton = 0;
	submesh.BaseVertexLocation = 0;

	geo->DrawArgs["grid"] = submesh;

	mGeometries["waterGeo"] = std::move(geo);
}

void LandAndWave::BuildPSOs(void)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePsoDesc;

	ZeroMemory(&opaquePsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	opaquePsoDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };
	opaquePsoDesc.pRootSignature = mRootSignature.Get();

	opaquePsoDesc.VS =
	{ reinterpret_cast<BYTE*>(mShaders["standardVS"]->GetBufferPointer()), mShaders["standardVS"]->GetBufferSize() };

	opaquePsoDesc.PS =
	{ reinterpret_cast<BYTE*>(mShaders["opaquePS"]->GetBufferPointer()), mShaders["opaquePS"]->GetBufferSize() };

	opaquePsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	opaquePsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	opaquePsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	opaquePsoDesc.SampleMask = UINT_MAX;
	opaquePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	opaquePsoDesc.NumRenderTargets = 1;
	opaquePsoDesc.RTVFormats[0] = GRAPHIC->Get_BackBufferFormat();
	opaquePsoDesc.SampleDesc.Count = GRAPHIC->Get_4xMsaaState() ? 4 : 1;
	opaquePsoDesc.SampleDesc.Quality = GRAPHIC->Get_4xMsaaState() ? (GRAPHIC->Get_4xMsaaQuality() - 1) : 0;
	opaquePsoDesc.DSVFormat = GRAPHIC->Get_DepthStencilFormat();

	ThrowIfFailed(GRAPHIC->Get_Device()->CreateGraphicsPipelineState(&opaquePsoDesc, IID_PPV_ARGS(&mPSOs["opaque"])));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaqueWireframePsoDesc = opaquePsoDesc;
	opaqueWireframePsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	ThrowIfFailed(GRAPHIC->Get_Device()->CreateGraphicsPipelineState(&opaqueWireframePsoDesc, IID_PPV_ARGS(&mPSOs["opaque_wireframe"])));
}

void LandAndWave::BuildFrameResources(void)
{
	for (int i = 0; i < NumFrameResources; ++i)
		mFrameResources.push_back(std::make_unique<FrameResource>(GRAPHIC->Get_Device().Get(), 1, (UINT)mAllRitems.size(), mWave->VertexCount()));
}

void LandAndWave::BuildRenderItems(void)
{
	auto wavesRitem = std::make_unique<RenderItem>();
	wavesRitem->World = MathHelper::Indentity4x4();
	wavesRitem->objCBIndex = 0;
	wavesRitem->Geo = mGeometries["waterGeo"].get();
	wavesRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	wavesRitem->IndexCount = wavesRitem->Geo->DrawArgs["grid"].IndexCount;
	wavesRitem->StartIndexLocation = wavesRitem->Geo->DrawArgs["grid"].StartIndexLocaiton;
	wavesRitem->BaseVertexLocation = wavesRitem->Geo->DrawArgs["grid"].BaseVertexLocation;
	
	mWavesRtime = wavesRitem.get();
	
	mRitemLayer[(int)RenderLayer::Opaque].push_back(wavesRitem.get());
	
	auto gridRitem = std::make_unique<RenderItem>();
	gridRitem->World = MathHelper::Indentity4x4();
	gridRitem->objCBIndex = 1;
	gridRitem->Geo = mGeometries["landGeo"].get();
	gridRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	gridRitem->IndexCount = gridRitem->Geo->DrawArgs["grid"].IndexCount;
	gridRitem->StartIndexLocation = gridRitem->Geo->DrawArgs["grid"].StartIndexLocaiton;
	gridRitem->BaseVertexLocation = gridRitem->Geo->DrawArgs["grid"].BaseVertexLocation;

	mRitemLayer[(int)RenderLayer::Opaque].push_back(gridRitem.get());

	mAllRitems.push_back(std::move(wavesRitem));
	mAllRitems.push_back(std::move(gridRitem));
}

void LandAndWave::DrawRenderItems(ID3D12GraphicsCommandList * cmdList, const std::vector<RenderItem*>& ritems)
{
	UINT objCBByteSize = D3DUTIL.CalcConstantBufferByteSize(sizeof(ObjectConstants));

	auto objectCB = mCurrFrameResource->ObjectCB->Resource();

	for (size_t i = 0; i < ritems.size(); ++i)
	{
		auto ri = ritems[i];

		cmdList->IASetVertexBuffers(0, 1, &ri->Geo->VertexBufferView());
		cmdList->IASetIndexBuffer(&ri->Geo->IndexBufferView());
		cmdList->IASetPrimitiveTopology(ri->PrimitiveType);

		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress();
		objCBAddress += ri->objCBIndex * objCBByteSize;

		cmdList->SetGraphicsRootConstantBufferView(0, objCBAddress);

		cmdList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
	}
}

void LandAndWave::OnKeyboardInput(const float & dt)
{
	if (GetAsyncKeyState('1') & 0x8000)
		mIsWireframe = TRUE;
	else
		mIsWireframe = FALSE;
}

void LandAndWave::UpdateCamera(const float & dt)
{
	g_EyePos.x = mRadius * sinf(mPhi) * cosf(mTheta);
	g_EyePos.z = mRadius * sinf(mPhi) * sinf(mTheta);
	g_EyePos.y = mRadius * cosf(mPhi);

	//Build the view matrix.
	DirectX::XMVECTOR pos = DirectX::XMVectorSet(g_EyePos.x, g_EyePos.y, g_EyePos.z, 1.0f);
	DirectX::XMVECTOR target = DirectX::XMVectorZero();
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(pos, target, up);
	DirectX::XMStoreFloat4x4(&mView, view);
}

void LandAndWave::UpdateObjectCBs(const float & dt)
{
	auto currObjectCB = mCurrFrameResource->ObjectCB.get();

	for (auto& e : mAllRitems)
	{
		if (e->NumFramesDirty > 0)
		{
			DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&e->World);

			ObjectConstants objConstants;
			DirectX::XMStoreFloat4x4(&objConstants.World, DirectX::XMMatrixTranspose(world));

			currObjectCB->CopyData(e->objCBIndex, objConstants);

			e->NumFramesDirty--;
		}
	}
}

void LandAndWave::UpdateMainPassCBs(const float & dt)
{
	DirectX::XMMATRIX view = DirectX::XMLoadFloat4x4(&mView);
	DirectX::XMMATRIX proj = DirectX::XMLoadFloat4x4(&mProj);

	DirectX::XMMATRIX ViewProj = DirectX::XMMatrixMultiply(view, proj);
	DirectX::XMMATRIX invView = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(view), view);
	DirectX::XMMATRIX invProj = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(proj), proj);
	DirectX::XMMATRIX invViewProj = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(ViewProj), ViewProj);

	DirectX::XMStoreFloat4x4(&mMainPassCB.View, DirectX::XMMatrixTranspose(view));
	DirectX::XMStoreFloat4x4(&mMainPassCB.InvView, DirectX::XMMatrixTranspose(invView));
	DirectX::XMStoreFloat4x4(&mMainPassCB.Proj, DirectX::XMMatrixTranspose(proj));
	DirectX::XMStoreFloat4x4(&mMainPassCB.InvProj, DirectX::XMMatrixTranspose(invProj));
	DirectX::XMStoreFloat4x4(&mMainPassCB.ViewProj, DirectX::XMMatrixTranspose(ViewProj));
	DirectX::XMStoreFloat4x4(&mMainPassCB.InvViewProj, DirectX::XMMatrixTranspose(invViewProj));

	mMainPassCB.EyePosW = g_EyePos;
	mMainPassCB.RenderTargetSize = DirectX::XMFLOAT2((float)WINSIZE_X, (float)WINSIZE_Y);
	mMainPassCB.InvRenderTargetSize = DirectX::XMFLOAT2(1.0f / WINSIZE_X, 1.0f / WINSIZE_Y);
	mMainPassCB.NearZ = 1.0f;
	mMainPassCB.FarZ = 1000.0f;
	mMainPassCB.TotalTime = TIME_MGR.Get_TotalTime(L"MainTimer");
	mMainPassCB.DeltaTime = TIME_MGR.Get_TimeDelta(L"MainTimer");

	auto currPassCB = mCurrFrameResource->PassCB.get();
	currPassCB->CopyData(0, mMainPassCB);
}

void LandAndWave::UpdateWave(const float & dt)
{
	static float t_base = 0.0f;
	if ((TIME_MGR.Get_TotalTime(L"WaveTimer") - t_base) >= 0.25f)
	{
		t_base += 0.25;

		int i = MathHelper::Rand(4, mWave->RowCount() - 5);
		int j = MathHelper::Rand(4, mWave->ColumnCount() - 5);

		float r = MathHelper::RandF(0.2f, 0.5f);

		mWave->Disturb(i, j, r);
	}

	mWave->Update(TIME_MGR.Get_TimeDelta(L"MainTimer"));
	
	auto currWavesVB = mCurrFrameResource->WavesVB.get();
	for (int i = 0; i < mWave->VertexCount(); ++i)
	{
		Vertex v;

		v.Pos = mWave->Position(i);
		v.Color = DirectX::XMFLOAT4(DirectX::Colors::Blue);

		currWavesVB->CopyData(i, v);
	}

	mWavesRtime->Geo->VertexBufferGPU = currWavesVB->Resource();
}

float LandAndWave::GetHillsHeight(float x, float z) const
{
	return 0.3f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));
}

DirectX::XMFLOAT3 LandAndWave::GetHillsNormal(float x, float z) const
{
	DirectX::XMFLOAT3 n(-0.03f * z * cosf(0.1f * x) - 0.3f * cosf(0.1f * z), 1.0f, -0.3f * sinf(0.1f * x) - 0.03f * x * sinf(0.1f * z));

	DirectX::XMVECTOR unitNormal = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&n));
	DirectX::XMStoreFloat3(&n, unitNormal);
	return n;
}

std::shared_ptr<LandAndWave> LandAndWave::Create(void)
{
	LAW pLAW = std::make_shared<LandAndWave>();
	if (!pLAW)	return nullptr;

	pLAW->Ready();
	return pLAW;
}
