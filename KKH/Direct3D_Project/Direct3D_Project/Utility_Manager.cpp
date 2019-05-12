#include "stdafx.h"
#include "Utility_Manager.h"
#include "Graphic_Manager.h"
#include "Timer_Manager.h"
#include "Function.h"

Utility_Manager::Utility_Manager(void)
	: mRootSignature(nullptr)
	, mSrvDescriptorHeap(nullptr)
	, Obj_static_map(new OBJMAP)
	, Obj_dynamic_map(new OBJMAP)
	//, mTheta(1.5f * PI), mPhi(0.2f * PI), mRadius(15.0f)
	, mView(MathHelper::Identity4x4())
	//, mProj(MathHelper::Identity4x4())
	, mCurrFrameResource(nullptr), mCurrFrameResourceIndex(0)
	, mCbvSrvDescriptorSize(0)
{
	allObj_Update_vec.push_back(Obj_static_map);
	allObj_Update_vec.push_back(Obj_dynamic_map);
}

Utility_Manager::~Utility_Manager(void)
{
}

void Utility_Manager::BuildRootSignature(void)
{
	CD3DX12_DESCRIPTOR_RANGE texTable;
	texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	CD3DX12_ROOT_PARAMETER slotRootParameter[4];

	slotRootParameter[0].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);
	slotRootParameter[1].InitAsConstantBufferView(0);
	slotRootParameter[2].InitAsConstantBufferView(1);
	slotRootParameter[3].InitAsConstantBufferView(2);

	auto staticSamplers = GetStaticSamplers();

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(4, slotRootParameter, 
		(UINT)staticSamplers.size(), staticSamplers.data(), 
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, 
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());

	ThrowIfFailed(hr);

	ThrowIfFailed(GRAPHIC->Get_Device()->CreateRootSignature(
		0, serializedRootSig->GetBufferPointer(), 
		serializedRootSig->GetBufferSize(), IID_PPV_ARGS(mRootSignature.GetAddressOf())));
}

void Utility_Manager::BuildDescriptorHeaps(void)
{
	//Create the SRV heap
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 3;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(GRAPHIC->Get_Device()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&mSrvDescriptorHeap)));
}

void Utility_Manager::BuildShadersAndInputLayer(void)
{
	const D3D_SHADER_MACRO alphaTestDefines[] = { "ALPHA_TEST", "1", NULL, NULL };

	mShaders["standardVS"] = d3dutil_Mananger::CompileShader(L"../Shaders/Default.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["opaquePS"] = d3dutil_Mananger::CompileShader(L"../Shaders/Default.hlsl", nullptr, "PS", "ps_5_1");

	mInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
}

void Utility_Manager::BuildFrameResources(void)
{
	for (int i = 0; i < NumFrameResources; ++i)
	{
		mFrameResources.push_back(std::make_unique<FrameResource>(GRAPHIC->Get_Device().Get(), 1, (UINT)mAllRitem.size(), (UINT)mMaterials.size()));
	}
}

void Utility_Manager::BuildPSOs(void)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	psoDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };
	psoDesc.pRootSignature = mRootSignature.Get();

	psoDesc.VS =
	{ reinterpret_cast<BYTE*>(mShaders["standardVS"]->GetBufferPointer()), mShaders["standardVS"]->GetBufferSize() };

	psoDesc.PS =
	{ reinterpret_cast<BYTE*>(mShaders["opaquePS"]->GetBufferPointer()), mShaders["opaquePS"]->GetBufferSize() };

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

	ThrowIfFailed(GRAPHIC->Get_Device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPSOs["opaque"])));

	//D3D12_GRAPHICS_PIPELINE_STATE_DESC opaqueWireframePsoDesc = psoDesc;
	//opaqueWireframePsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	//ThrowIfFailed(GRAPHIC->Get_Device()->CreateGraphicsPipelineState(&opaqueWireframePsoDesc, IID_PPV_ARGS(&mPSOs["opaque_wireframe"])));
}

void Utility_Manager::OnResize(void)
{
	DirectX::XMMATRIX p = DirectX::XMMatrixPerspectiveFovLH(0.25f * PI, AspectRatio(), 1.0f, 1000.0f);
	DirectX::XMStoreFloat4x4(&g_Proj, p);
}

bool Utility_Manager::Object_Create(OBJECT obj)
{
	if (obj == nullptr) return FALSE;

	if (obj->Get_Comtype() == Object::COM_TYPE::CT_STATIC)
		Obj_static_map->insert(OBJMAP::value_type(obj->Get_Objname(), obj));
	else if (obj->Get_Comtype() == Object::COM_TYPE::CT_DYNAMIC)
		Obj_dynamic_map->insert(OBJMAP::value_type(obj->Get_Objname(), obj));

	return TRUE;
}

bool Utility_Manager::Object_Cycle(const float & dt, ObjState _state)
{
	for (auto& objmap : allObj_Update_vec)
	{
		for (auto& obj : *objmap)
		{
			switch (_state)
			{
			case ObjState::OS_READY:
				if (!obj.second->Ready())
					return FALSE;
				break;

			case ObjState::OS_UPDATE:
				//if (!obj.second->Update(dt))
				if(Object_Update(dt, obj.second))
					return FALSE;
				break;

			case ObjState::OS_RENDER:
				//if (!obj.second->Render(dt))
				if (Object_Render(dt, obj.second))
					return FALSE;
				break;
			}
		}
	}

	return TRUE;
}

bool Utility_Manager::Object_Update(const float & dt, OBJECT& obj)
{
	OnKeyboardInput(dt);
	UpdateCamera(dt);

	UTIL.Get_CurrFrameResourceIndex() = (UTIL.Get_CurrFrameResourceIndex() + 1) % NumFrameResources;
	UTIL.Get_CurrFrameResource() = UTIL.Get_Frameres()[UTIL.Get_CurrFrameResourceIndex()].get();

	if (UTIL.Get_CurrFrameResource()->Fence != 0 && GRAPHIC->Get_Fence()->GetCompletedValue() < UTIL.Get_CurrFrameResource()->Fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
		ThrowIfFailed(GRAPHIC->Get_Fence()->SetEventOnCompletion(UTIL.Get_CurrFrameResource()->Fence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	obj->Update(dt);

	AnimateMaterials(dt);
	UpdateObjectCBs(dt);
	UpdateMaterialCBs(dt);
	UpdateMainPassCB(dt);

	return TRUE;
}

bool Utility_Manager::Object_Render(const float & dt, OBJECT& obj)
{
	auto cmdListAlloc = UTIL.Get_CurrFrameResource()->CmdListAlloc;

	ThrowIfFailed(cmdListAlloc->Reset());
	ThrowIfFailed(GRAPHIC->Get_CommandList()->Reset(cmdListAlloc.Get(), UTIL.Get_PSOs("opaque").Get()));

	GRAPHIC->Get_CommandList()->RSSetViewports(1, &GRAPHIC->Get_ScreenViewport());
	GRAPHIC->Get_CommandList()->RSSetScissorRects(1, &GRAPHIC->Get_ScissorRect());

	GRAPHIC->Get_CommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(GRAPHIC->Get_CurrentBackBuffer_Resource(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	GRAPHIC->Get_CommandList()->ClearRenderTargetView(GRAPHIC->Get_CurrentBackBufferView_Handle(), Colors::LightSteelBlue, 0, nullptr);
	GRAPHIC->Get_CommandList()->ClearDepthStencilView(GRAPHIC->Get_DepthStencilView_Handle(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0.0f, 0.0f, nullptr);

	GRAPHIC->Get_CommandList()->OMSetRenderTargets(1, &GRAPHIC->Get_CurrentBackBufferView_Handle(), TRUE, &GRAPHIC->Get_DepthStencilView_Handle());

	GRAPHIC->Get_CommandList()->SetGraphicsRootSignature(UTIL.Get_RootSignature().Get());

	auto passCB = UTIL.Get_CurrFrameResource()->PassCB->Resource();
	GRAPHIC->Get_CommandList()->SetGraphicsRootConstantBufferView(2, passCB->GetGPUVirtualAddress());

	DrawRenderItems(GRAPHIC->Get_CommandList().Get(), UTIL.Get_Drawlayer((int)DrawLayer::DL_OPAUQE));

	GRAPHIC->Get_CommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(GRAPHIC->Get_CurrentBackBuffer_Resource(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	ThrowIfFailed(GRAPHIC->Get_CommandList()->Close());

	ID3D12CommandList* cmdsLists[] = { GRAPHIC->Get_CommandList().Get() };
	GRAPHIC->Get_CommandQueue()->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	ThrowIfFailed(GRAPHIC->Get_SwapChain()->Present(0, 0));
	GRAPHIC->Get_Set_CurrBackBuffer() = (GRAPHIC->Get_Set_CurrBackBuffer() + 1) % GRAPHIC->Get_SwapChainBufferCount();

	UTIL.Get_CurrFrameResource()->Fence = ++GRAPHIC->Get_CurrentFence();

	GRAPHIC->Get_CommandQueue()->Signal(GRAPHIC->Get_Fence().Get(), GRAPHIC->Get_CurrentFence());

	return TRUE;
}

void Utility_Manager::AnimateMaterials(const float & dt)
{
}

void Utility_Manager::UpdateObjectCBs(const float & dt)
{
	auto currObjectCB = UTIL.Get_CurrFrameResource()->ObjectCB.get();
	for (auto& e : UTIL.Get_Ritemvec())
	{
		if (e->NumFramesDirty > 0)
		{
			DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&e->World);
			DirectX::XMMATRIX texTransform = DirectX::XMLoadFloat4x4(&e->TexTransform);

			ObjectConstants objConstants;
			DirectX::XMStoreFloat4x4(&objConstants.World, DirectX::XMMatrixTranspose(world));
			DirectX::XMStoreFloat4x4(&objConstants.TexTransform, DirectX::XMMatrixTranspose(texTransform));

			currObjectCB->CopyData(e->objCBIndex, objConstants);

			e->NumFramesDirty--;
		}
	}
}

void Utility_Manager::UpdateMaterialCBs(const float & dt)
{
	auto currMaterialCB = UTIL.Get_CurrFrameResource()->MaterialCB.get();
	for (auto& e : UTIL.Get_Materials())
	{
		Material* mat = e.second.get();
		if (mat->NumFrameDirty > 0)
		{
			DirectX::XMMATRIX matTransform = DirectX::XMLoadFloat4x4(&mat->MatTransform);

			MaterialConstants matConstants;
			matConstants.DiffuseAlbedo = mat->DiffuseAlbedo;
			matConstants.FresnelR0 = mat->FresnelR0;
			matConstants.Roughness = mat->Roughness;
			DirectX::XMStoreFloat4x4(&matConstants.MatTransform, DirectX::XMMatrixTranspose(matTransform));

			currMaterialCB->CopyData(mat->MatCBIndex, matConstants);

			mat->NumFrameDirty--;
		}
	}
}

void Utility_Manager::UpdateMainPassCB(const float & dt)
{
	XMMATRIX view = XMLoadFloat4x4(&UTIL.Get_ViewMat());
	XMMATRIX proj = XMLoadFloat4x4(&g_Proj);

	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
	XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

	XMStoreFloat4x4(&UTIL.Get_MainPassCB().View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&UTIL.Get_MainPassCB().InvView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&UTIL.Get_MainPassCB().Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&UTIL.Get_MainPassCB().InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&UTIL.Get_MainPassCB().ViewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&UTIL.Get_MainPassCB().InvViewProj, XMMatrixTranspose(invViewProj));
	UTIL.Get_MainPassCB().EyePosW = g_EyePos;
	UTIL.Get_MainPassCB().RenderTargetSize = XMFLOAT2((float)WINSIZE_X, (float)WINSIZE_Y);
	UTIL.Get_MainPassCB().InvRenderTargetSize = XMFLOAT2(1.0f / WINSIZE_X, 1.0f / WINSIZE_Y);
	UTIL.Get_MainPassCB().NearZ = 1.0f;
	UTIL.Get_MainPassCB().FarZ = 1000.0f;
	UTIL.Get_MainPassCB().TotalTime = dt;
	UTIL.Get_MainPassCB().DeltaTime = dt;
	UTIL.Get_MainPassCB().AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };
	UTIL.Get_MainPassCB().Lights[0].Direction = { 0.57735f, -0.57735f, 0.57735f };
	UTIL.Get_MainPassCB().Lights[0].Strength = { 0.6f, 0.6f, 0.6f };
	UTIL.Get_MainPassCB().Lights[1].Direction = { -0.57735f, -0.57735f, 0.57735f };
	UTIL.Get_MainPassCB().Lights[1].Strength = { 0.3f, 0.3f, 0.3f };
	UTIL.Get_MainPassCB().Lights[2].Direction = { 0.0f, -0.707f, -0.707f };
	UTIL.Get_MainPassCB().Lights[2].Strength = { 0.15f, 0.15f, 0.15f };

	auto currPassCB = UTIL.Get_CurrFrameResource()->PassCB.get();
	currPassCB->CopyData(0, UTIL.Get_MainPassCB());
}

void Utility_Manager::DrawRenderItems(ID3D12GraphicsCommandList * cmdList, const std::vector<RenderItem*>& ritems)
{
	UINT objCBByteSize = d3dutil_Mananger::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	UINT matCBByteSize = d3dutil_Mananger::CalcConstantBufferByteSize(sizeof(MaterialConstants));

	auto objectCB = UTIL.Get_CurrFrameResource()->ObjectCB->Resource();
	auto matCB = UTIL.Get_CurrFrameResource()->MaterialCB->Resource();

	// For each render item...
	for (size_t i = 0; i < ritems.size(); ++i)
	{
		auto ri = ritems[i];

		cmdList->IASetVertexBuffers(0, 1, &ri->Geo->VertexBufferView());
		cmdList->IASetIndexBuffer(&ri->Geo->IndexBufferView());
		cmdList->IASetPrimitiveTopology(ri->PrimitiveType);

		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + ri->objCBIndex*objCBByteSize;
		D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + ri->Mat->MatCBIndex*matCBByteSize;

		cmdList->SetGraphicsRootConstantBufferView(0, objCBAddress);
		cmdList->SetGraphicsRootConstantBufferView(1, matCBAddress);

		cmdList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
	}
}

void Utility_Manager::OnKeyboardInput(const float & dt)
{
}

void Utility_Manager::UpdateCamera(const float & dt)
{
	for (auto& all : UTIL.Get_Allobjvec())
	{
		for (auto& objvec : *all)
		{
			g_EyePos.x = objvec.second->Get_Radius() * sinf(objvec.second->Get_Phi()) * cosf(objvec.second->Get_Theta());
			g_EyePos.z = objvec.second->Get_Radius() * sinf(objvec.second->Get_Phi()) * sinf(objvec.second->Get_Theta());
			g_EyePos.y = objvec.second->Get_Radius() * cosf(objvec.second->Get_Phi());

			DirectX::XMVECTOR pos = XMVectorSet(g_EyePos.x, g_EyePos.y, g_EyePos.z, 1.0f);
			DirectX::XMVECTOR target = DirectX::XMVectorZero();
			DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

			DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(pos, target, up);
			DirectX::XMStoreFloat4x4(&mView, view);
		}
	}
}