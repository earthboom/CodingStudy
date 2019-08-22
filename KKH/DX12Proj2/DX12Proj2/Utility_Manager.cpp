#include "pch.h"
#include "Utility_Manager.h"
#include "Timer_Manager.h"
#include "Texture_Manger.h"
#include "Camera_Manager.h"
#include "Mouse_Manager.h"

Utility_Manager::Utility_Manager(void)
	: mRootSignature(nullptr), mPostProcessRootSignature(nullptr)
	, mSrvDescriptorHeap(nullptr)
	, Obj_static_map(new OBJMAP)
	, Obj_dynamic_map(new OBJMAP)
	//, mTheta(1.5f * PI), mPhi(0.2f * PI), mRadius(15.0f)
	, mView(MathHelper::Identity4x4())
	//, mProj(MathHelper::Identity4x4())
	, mCurrFrameResource(nullptr), mCurrFrameResourceIndex(0)
	, mCbvSrvDescriptorSize(0), mLightRotetionAngle(0.0f)
	, mCurrState(OS_END)
	, mLightNearZ(0.0f), mLightFarZ(0.0f)
	, mLightPosW(XMFLOAT3(0.0f, 0.0f, 0.0f))
	, mLightView(MathHelper::Identity4x4())
	, mLightProj(MathHelper::Identity4x4())
	, mShadowTransform(MathHelper::Identity4x4())
	, mSkyTexHeapIndex(0.0f), mShadowMapHeapIndex(0.0f), mNullCubeSrvIndex(0.0f), mNullTexSrvIndex(0.0f)
{
	allObj_Update_vec.push_back(Obj_static_map);
	allObj_Update_vec.push_back(Obj_dynamic_map);

	mBaseLightDirections[0] = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);
	mBaseLightDirections[1] = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);
	mBaseLightDirections[2] = XMFLOAT3(0.0f, -0.707f, -0.707f);
}

Utility_Manager::~Utility_Manager(void)
{

}

void Utility_Manager::UtilityInitialize(void)
{
	mBlurFilter = std::make_unique<BlurFilter>(DEVICE.Get(), WINSIZE_X, WINSIZE_Y, DXGI_FORMAT_R8G8B8A8_UNORM);
}

void Utility_Manager::BuildRootSignature(void)
{
	CD3DX12_DESCRIPTOR_RANGE texTable0;
	texTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 0, 0);

	CD3DX12_DESCRIPTOR_RANGE texTable1;
	texTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 10, 2, 0);

	CD3DX12_ROOT_PARAMETER slotRootParameter[5];

	slotRootParameter[0].InitAsConstantBufferView(0);
	slotRootParameter[1].InitAsConstantBufferView(1);
	slotRootParameter[2].InitAsShaderResourceView(0, 1);
	slotRootParameter[3].InitAsDescriptorTable(1, &texTable0, D3D12_SHADER_VISIBILITY_PIXEL);
	slotRootParameter[4].InitAsDescriptorTable(1, &texTable1, D3D12_SHADER_VISIBILITY_PIXEL);

	auto staticSamplers = GetStaticSamplers();

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(5, slotRootParameter, 
		(UINT)staticSamplers.size(), staticSamplers.data(), 
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, 
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());

	ThrowIfFailed(hr);

	ThrowIfFailed(DEVICE->CreateRootSignature(
		0, serializedRootSig->GetBufferPointer(), 
		serializedRootSig->GetBufferSize(), IID_PPV_ARGS(mRootSignature.GetAddressOf())));
}

void Utility_Manager::BuildPostProcessRootSignature(void)
{
	CD3DX12_DESCRIPTOR_RANGE srvTable;
	srvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	CD3DX12_DESCRIPTOR_RANGE uavTable;
	uavTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 0);
	//uavTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);

	CD3DX12_ROOT_PARAMETER slotRootParameter[5];

	//slotRootParameter[0].InitAsConstants(12, 0);
	slotRootParameter[0].InitAsConstantBufferView(0);
	slotRootParameter[1].InitAsConstantBufferView(1);
	slotRootParameter[2].InitAsShaderResourceView(0, 1);
	slotRootParameter[3].InitAsDescriptorTable(1, &srvTable, D3D12_SHADER_VISIBILITY_PIXEL);
	slotRootParameter[4].InitAsDescriptorTable(1, &uavTable, D3D12_SHADER_VISIBILITY_PIXEL);

	auto staticSampler = GetStaticSamplers();

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(3, slotRootParameter
		, (UINT)staticSampler.size(), staticSampler.data(),
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());

	ThrowIfFailed(hr);

	ThrowIfFailed(DEVICE->CreateRootSignature(
		0, serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(), IID_PPV_ARGS(mPostProcessRootSignature.GetAddressOf())));
}

void Utility_Manager::BuildDescriptorHeaps(void)
{
	const int textureDescriptorCount = 4;
	const int blurDescriptorCount = 4;

	//Create the SRV heap
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 14;// textureDescriptorCount + blurDescriptorCount;//7;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(DEVICE->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&mSrvDescriptorHeap)));
}

void Utility_Manager::BuildShadersAndInputLayer(void)
{
	//const D3D_SHADER_MACRO defines[] = { "FOG", "1", NULL, NULL };
	const D3D_SHADER_MACRO alphaTestDefines[] = { "ALPHA_TEST", "1", NULL, NULL };

	mShaders["standardVS"]			= d3dutil::CompileShader(L"Shaders\\Default.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["opaquePS"]			= d3dutil::CompileShader(L"Shaders\\Default.hlsl", nullptr, "PS", "ps_5_1");

	mShaders["shadowVS"]			= d3dutil::CompileShader(L"Shaders\\Shadows.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["shadowOpaquePS"]		= d3dutil::CompileShader(L"Shaders\\Shadows.hlsl", nullptr, "PS", "ps_5_1");
	mShaders["shadowAlphaTestedPS"]	= d3dutil::CompileShader(L"Shaders\\Shadows.hlsl", alphaTestDefines, "PS", "ps_5_1");

	mShaders["debugVS"]				= d3dutil::CompileShader(L"Shaders\\ShadowDebug.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["debugPS"]				= d3dutil::CompileShader(L"Shaders\\ShadowDebug.hlsl", nullptr, "PS", "ps_5_1");

	mShaders["skyVS"]				= d3dutil::CompileShader(L"Shaders\\Sky.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["skyPS"]				= d3dutil::CompileShader(L"Shaders\\Sky.hlsl", nullptr, "PS", "ps_5_1");

	mInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};
}

void Utility_Manager::UtilityDecriptor(void)
{
	mBlurFilter->BuildDescriptors(
		CD3DX12_CPU_DESCRIPTOR_HANDLE(mSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), g_MatCBcount, mCbvSrvDescriptorSize),
		CD3DX12_GPU_DESCRIPTOR_HANDLE(mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart(), g_MatCBcount, mCbvSrvDescriptorSize),
		mCbvSrvDescriptorSize);
}

void Utility_Manager::BuildFrameResources(void)
{
	for (int i = 0; i < NumFrameResources; ++i)
	{
		mFrameResources.push_back(std::make_unique<FrameResource>(DEVICE.Get(), 2, (UINT)mAllRitem.size(), (UINT)mMaterials.size()));
	}
}

void Utility_Manager::BuildPSOs(void)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	psoDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };
	psoDesc.pRootSignature = mRootSignature.Get();

	psoDesc.VS = { reinterpret_cast<BYTE*>(mShaders["standardVS"]->GetBufferPointer()), mShaders["standardVS"]->GetBufferSize() };
	psoDesc.PS = { reinterpret_cast<BYTE*>(mShaders["opaquePS"]->GetBufferPointer()), mShaders["opaquePS"]->GetBufferSize() };

	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = g_Graphics->Get_BackBufferFormat();
	psoDesc.SampleDesc.Count = g_Graphics->Get_4xMsaaState() ? 4 : 1;
	psoDesc.SampleDesc.Quality = g_Graphics->Get_4xMsaaState() ? (g_Graphics->Get_4xMsaaQuality() - 1) : 0;
	psoDesc.DSVFormat = g_Graphics->Get_DepthStencilFormat();

	ThrowIfFailed(DEVICE->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPSOs["opaque"])));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC samPsoDesc = psoDesc;
	samPsoDesc.RasterizerState.DepthBias = 100000;
	samPsoDesc.RasterizerState.DepthBiasClamp = 0.0f;
	samPsoDesc.RasterizerState.SlopeScaledDepthBias = 1.0f;
	samPsoDesc.pRootSignature = mRootSignature.Get();
	samPsoDesc.VS = { reinterpret_cast<BYTE*>(mShaders["shadowVS"]->GetBufferPointer()), mShaders["shadowVS"]->GetBufferSize() };
	samPsoDesc.PS = { reinterpret_cast<BYTE*>(mShaders["shadowOpaquePS"]->GetBufferPointer()), mShaders["shadowOpaquePS"]->GetBufferSize() };
	samPsoDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
	samPsoDesc.NumRenderTargets = 0;
	ThrowIfFailed(DEVICE->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPSOs["shadow_opaque"])));



	D3D12_GRAPHICS_PIPELINE_STATE_DESC debugDesc = psoDesc;
	debugDesc.pRootSignature = mRootSignature.Get();
	debugDesc.VS = { reinterpret_cast<BYTE*>(mShaders["debugVS"]->GetBufferPointer()), mShaders["debugVS"]->GetBufferSize() };
	debugDesc.PS = { reinterpret_cast<BYTE*>(mShaders["debugPS"]->GetBufferPointer()), mShaders["debugPS"]->GetBufferSize() };
	ThrowIfFailed(DEVICE->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPSOs["debug"])));



	D3D12_GRAPHICS_PIPELINE_STATE_DESC skyPsoDesc = psoDesc;

	skyPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

	skyPsoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	skyPsoDesc.pRootSignature = mRootSignature.Get();
	skyPsoDesc.VS = { reinterpret_cast<BYTE*>(mShaders["skyVS"]->GetBufferPointer()), mShaders["skyVS"]->GetBufferSize() };
	skyPsoDesc.PS = { reinterpret_cast<BYTE*>(mShaders["skyPS"]->GetBufferPointer()), mShaders["skyPS"]->GetBufferSize() };
	ThrowIfFailed(DEVICE->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPSOs["sky"])));


	// hightlight object

	//D3D12_GRAPHICS_PIPELINE_STATE_DESC highlightPsoDesc = psoDesc;

	//highlightPsoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	//D3D12_RENDER_TARGET_BLEND_DESC transparencyBlendDesc;
	//transparencyBlendDesc.BlendEnable = TRUE;
	//transparencyBlendDesc.LogicOpEnable = FALSE;
	//transparencyBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	//transparencyBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	//transparencyBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
	//transparencyBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	//transparencyBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	//transparencyBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	//transparencyBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
	//transparencyBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//highlightPsoDesc.BlendState.RenderTarget[0] = transparencyBlendDesc;
	//ThrowIfFailed(DEVICE->CreateGraphicsPipelineState(&highlightPsoDesc, IID_PPV_ARGS(&mPSOs["highlight"])));
}

void Utility_Manager::OnResize(void)
{
	CURR_CAM->SetLens(0.25f * PI, AspectRatio(), 1.0f, 1000.0f);



	if (g_ScreenBlur)
	{
		if (mBlurFilter != nullptr)
			mBlurFilter->OnResize(WINSIZE_X, WINSIZE_Y);
	}

	BoundingFrustum::CreateFromMatrix(mCamFrustum, XMLoadFloat4x4(&g_Proj));
}

//void Utility_Manager::BuildPublicMaterials(void)
//{
//	auto highlight0 = std::make_unique<Material>();
//	highlight0->Name = "highlight0";
//	highlight0->MatCBIndex = g_MatCBcount++;
//	highlight0->DiffuseSrvHeapIndex = 0;
//	highlight0->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f);
//	highlight0->FresnelR0 = XMFLOAT3(0.06f, 0.06f, 0.06f);
//	highlight0->Roughness = 0.0f;
//
//	mMaterials["highlight0"] = std::move(highlight0);
//}

bool Utility_Manager::Object_Create(OBJECT& obj)
{
	if (obj == nullptr) return FALSE;
	
	if (obj->Get_Comtype() == Object::COM_TYPE::CT_STATIC)
		Obj_static_map->insert(OBJMAP::value_type(obj->Get_submeshName(), obj));
		//Obj_static_map->insert(OBJMAP::value_type(obj->Get_Objname(), obj));
	else if (obj->Get_Comtype() == Object::COM_TYPE::CT_DYNAMIC)
		Obj_dynamic_map->insert(OBJMAP::value_type(obj->Get_submeshName(), obj));
		//Obj_dynamic_map->insert(OBJMAP::value_type(obj->Get_Objname(), obj));

	return TRUE;
}

bool Utility_Manager::Object_Cycle(const CTimer& mt, ObjState _state)
{
	mCurrState = _state;

	switch (_state)
	{
	case ObjState::OS_UPDATE:
		if (!Object_Update(mt))
			return FALSE;
		break;

	case ObjState::OS_RENDER:
		if (!Object_Render(mt))
			return FALSE;
		break;
	}

	return TRUE;
}

OBJECT Utility_Manager::Get_Object(std::string _key, Object::COM_TYPE _type)
{
	auto pObj = allObj_Update_vec[(int)_type]->find(_key);
	if (pObj == allObj_Update_vec[(int)_type]->end()) return nullptr;
	return pObj->second;
}

bool Utility_Manager::Object_Ready()
{
	for (auto& objmap : allObj_Update_vec)
	{
		for (auto& obj : *objmap)
		{
			OBJECT& pObject = obj.second;
			if (pObject == nullptr) return FALSE;
			if (!pObject->Ready())	return FALSE;
		}
	}

	return TRUE;
}

bool Utility_Manager::Object_Update(const CTimer& mt)
{
	OnKeyboardInput(mt);
	//UpdateCamera(mt);

	mCurrFrameResourceIndex = (mCurrFrameResourceIndex + 1) % NumFrameResources;
	mCurrFrameResource = UTIL.Get_Frameres()[mCurrFrameResourceIndex].get();

	if (mCurrFrameResource->Fence != 0 && g_Graphics->Get_Fence()->GetCompletedValue() < mCurrFrameResource->Fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
		ThrowIfFailed(g_Graphics->Get_Fence()->SetEventOnCompletion(mCurrFrameResource->Fence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	mLightRotetionAngle += 0.1f * mt.DeltaTime();
	
	XMMATRIX R = XMMatrixRotationY(mLightRotetionAngle);
	for (int i = 0; i < 3; ++i)
	{
		XMVECTOR lightDir = XMLoadFloat3(&mBaseLightDirections[i]);
		lightDir = XMVector3TransformNormal(lightDir, R);
		XMStoreFloat3(&mRotatedLightDirections[i], lightDir);
	}

	for (auto& objmap : allObj_Update_vec)
	{
		for (auto& obj : *objmap)
		{
			OBJECT& pObject = obj.second;
			if (pObject == nullptr)		return FALSE;
			if (!pObject->Update(mt))	return FALSE;
		}
	}

	UpdateObjectCBs(mt);
	//UpdateInstanceData(mt);
	UpdateMaterialCBs(mt);
	UpdateShadowTransform(mt);
	UpdateMainPassCB(mt);
	UpdateShadowPassCB(mt);
	//UpdateReflectedPassCB(mt);

	return TRUE;
}

bool Utility_Manager::Object_Render(const CTimer& mt)//, OBJMAP& _objmap)
{
	const COMMANDLIST& _commandlist = COM_LIST;
	const PGRAPHIC& _graphic = g_Graphics;

	auto cmdListAlloc = mCurrFrameResource->CmdListAlloc;

	ThrowIfFailed(cmdListAlloc->Reset());
	ThrowIfFailed(_commandlist->Reset(cmdListAlloc.Get(), mPSOs["opaque"].Get()));
	
	ID3D12DescriptorHeap* descriptorHeaps[] = { mSrvDescriptorHeap.Get() };
	_commandlist->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	_commandlist->SetGraphicsRootSignature(mRootSignature.Get());

	auto matBuffer = mCurrFrameResource->MaterialBuffer->Resource();
	_commandlist->SetGraphicsRootShaderResourceView(2, matBuffer->GetGPUVirtualAddress());

	_commandlist->SetGraphicsRootDescriptorTable(3, mNullSrv);

	_commandlist->SetGraphicsRootDescriptorTable(4, mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	
	DrawSceneToShadowMap();

	_commandlist->RSSetViewports(1, &_graphic->Get_ScreenViewport());
	_commandlist->RSSetScissorRects(1, &_graphic->Get_ScissorRect());

	_commandlist->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_graphic->Get_CurrentBackBuffer_Resource(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	_commandlist->ClearRenderTargetView(_graphic->Get_CurrentBackBufferView_Handle(), Colors::LightSteelBlue, 0, nullptr);
	_commandlist->ClearDepthStencilView(_graphic->Get_DepthStencilView_Handle(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	_commandlist->OMSetRenderTargets(1, &_graphic->Get_CurrentBackBufferView_Handle(), TRUE, &_graphic->Get_DepthStencilView_Handle());

	auto passCB = mCurrFrameResource->PassCB->Resource();
	_commandlist->SetGraphicsRootConstantBufferView(2, passCB->GetGPUVirtualAddress());

	CD3DX12_GPU_DESCRIPTOR_HANDLE skyTexDip(mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	skyTexDip.Offset(mSkyTexHeapIndex, mCbvSrvDescriptorSize);
	_commandlist->SetGraphicsRootDescriptorTable(3, skyTexDip);

	_commandlist->SetPipelineState(mPSOs["opaque"].Get());
	DrawRenderItems(_commandlist.Get(), mDrawLayer[(int)DrawLayer::DL_OPAUQE]);

	_commandlist->SetPipelineState(mPSOs["debug"].Get());
	DrawRenderItems(_commandlist.Get(), mDrawLayer[(int)DrawLayer::DL_DEUBG]);
	
	_commandlist->SetPipelineState(mPSOs["sky"].Get());
	DrawRenderItems(_commandlist.Get(), mDrawLayer[(int)DrawLayer::DL_SKY]);

	//_commandlist->SetPipelineState(mPSOs["highlight"].Get());
	//DrawRenderItems(_commandlist.Get(), mDrawLayer[(int)DrawLayer::DL_HIGHLIGHT]);

	_commandlist->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_graphic->Get_CurrentBackBuffer_Resource(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	ThrowIfFailed(_commandlist->Close());

	ID3D12CommandList* cmdsLists[] = { _commandlist.Get() };
	_graphic->Get_CommandQueue()->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	ThrowIfFailed(_graphic->Get_SwapChain()->Present(0, 0));
	_graphic->Get_Set_CurrBackBuffer() = (_graphic->Get_Set_CurrBackBuffer() + 1) % _graphic->Get_SwapChainBufferCount();

	mCurrFrameResource->Fence = ++_graphic->Get_CurrentFence();

	_graphic->Get_CommandQueue()->Signal(_graphic->Get_Fence().Get(), _graphic->Get_CurrentFence());

	return TRUE;
}

void Utility_Manager::UpdateObjectCBs(const CTimer& mt)
{
	auto currObjectCB = mCurrFrameResource->ObjectCB.get();
	for (auto& e : mAllRitem)
	{
		if (e->NumFramesDirty > 0)
		{
			DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&e->World);
			DirectX::XMMATRIX texTransform = DirectX::XMLoadFloat4x4(&e->TexTransform);

			ObjectConstants objConstants;
			DirectX::XMStoreFloat4x4(&objConstants.World, DirectX::XMMatrixTranspose(world));
			DirectX::XMStoreFloat4x4(&objConstants.TexTransform, DirectX::XMMatrixTranspose(texTransform));
			objConstants.Materialndex = e->Mat->MatCBIndex;

			currObjectCB->CopyData(e->objCBIndex, objConstants);

			e->NumFramesDirty--;
		}
	}
}

void Utility_Manager::UpdateInstanceData(const CTimer& mt)
{
	//XMMATRIX view = CURR_CAM->GetView();
	//XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);

	//auto currInstanceBuffer = mCurrFrameResource->InstanceBuffer.get();
	//int visibleInstanceCount = 0;
	//for (auto& e : mAllRitem)
	//{
	//	const auto& instanceData = e->Instances;
	//	
	//	UINT matIndex = e->Mat->DiffuseSrvHeapIndex + 1;

	//	for (UINT i = 0; i < (UINT)instanceData.size(); ++i)
	//	{
	//		XMMATRIX world = XMLoadFloat4x4(&instanceData[i].World);
	//		XMMATRIX texTransform = XMLoadFloat4x4(&instanceData[i].TexTransform);

	//		XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(world), world);

	//		XMMATRIX viewToLocal = XMMatrixMultiply(invView, invWorld);

	//		BoundingFrustum localSpaceFrustum;
	//		mCamFrustum.Transform(localSpaceFrustum, viewToLocal);

	//		if ((localSpaceFrustum.Contains(e->Bounds) != DirectX::DISJOINT) || (g_FrustumCullingEnabled == FALSE))
	//		{
	//			InstanceData data;
	//			XMStoreFloat4x4(&data.World, XMMatrixTranspose(world));
	//			XMStoreFloat4x4(&data.TexTransform, XMMatrixTranspose(texTransform));
	//			//data.MaterialIndex = instanceData[i].MaterialIndex;
	//			data.MaterialIndex = matIndex;

	//			currInstanceBuffer->CopyData(visibleInstanceCount++, data);
	//		}
	//	}

	//	//e->InstanceCount = visibleInstanceCount;
	//	//visibleInstanceCount = 0;

	//	std::wostringstream outs;
	//	outs.precision(6);
	//	outs << L"Instancing and Culling" << L"   " << e->InstanceCount << L" objects visible out of " << e->Instances.size();
	//}
}

void Utility_Manager::UpdateMaterialCBs(const CTimer& mt)
{
	auto currMaterialCB = mCurrFrameResource->MaterialBuffer.get();//MaterialCB.get();
	for (auto& e : mMaterials)
	{
		Material* mat = e.second.get();
		if (mat->NumFrameDirty > 0)
		{
			DirectX::XMMATRIX matTransform = DirectX::XMLoadFloat4x4(&mat->MatTransform);

			//MaterialConstants matConstants;
			MaterialData matData;
			matData.DiffuseAlbedo = mat->DiffuseAlbedo;
			matData.FresnelR0 = mat->FresnelR0;
			matData.Roughness = mat->Roughness;
			DirectX::XMStoreFloat4x4(&matData.MatTransform, DirectX::XMMatrixTranspose(matTransform));
			matData.DiffuseMapIndex = mat->DiffuseSrvHeapIndex;
			matData.NormalMapIndex = mat->NormalSrvHeapIndex;

			currMaterialCB->CopyData(mat->MatCBIndex, matData);

			mat->NumFrameDirty--;
		}
	}
}

void Utility_Manager::UpdateMainPassCB(const CTimer& mt)
{
	XMMATRIX view = CURR_CAM->GetView();//XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&g_Proj);

	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
	XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

	XMMATRIX shadowTransform = XMLoadFloat4x4(&mShadowTransform);

	XMStoreFloat4x4(&mMainPassCB.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&mMainPassCB.InvView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&mMainPassCB.Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&mMainPassCB.InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&mMainPassCB.ViewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&mMainPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
	XMStoreFloat4x4(&mMainPassCB.ShadowTransform, XMMatrixTranspose(shadowTransform));
	mMainPassCB.EyePosW = g_EyePos;
	mMainPassCB.RenderTargetSize = XMFLOAT2((float)WINSIZE_X, (float)WINSIZE_Y);
	mMainPassCB.InvRenderTargetSize = XMFLOAT2(1.0f / WINSIZE_X, 1.0f / WINSIZE_Y);
	mMainPassCB.NearZ = 1.0f;
	mMainPassCB.FarZ = 1000.0f;
	mMainPassCB.TotalTime = mt.TotalTime();
	mMainPassCB.DeltaTime = mt.DeltaTime();
	mMainPassCB.AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };
	mMainPassCB.Lights[0].Direction = mRotatedLightDirections[0];//{ 0.57735f, -0.57735f, 0.57735f };
	mMainPassCB.Lights[0].Strength = { 0.9f, 0.9f, 0.8f };
	mMainPassCB.Lights[1].Direction = mRotatedLightDirections[1]; //{ -0.57735f, -0.57735f, 0.57735f };
	mMainPassCB.Lights[1].Strength = { 0.3f, 0.3f, 0.3f };
	mMainPassCB.Lights[2].Direction = mRotatedLightDirections[2]; //{ 0.0f, -0.707f, -0.707f };
	mMainPassCB.Lights[2].Strength = { 0.2f, 0.2f, 0.2f };

	auto currPassCB = mCurrFrameResource->PassCB.get();
	currPassCB->CopyData(0, mMainPassCB);
}

void Utility_Manager::UpdateShadowPassCB(const CTimer& mt)
{
	XMMATRIX view = XMLoadFloat4x4(&mLightView);
	XMMATRIX proj = XMLoadFloat4x4(&mLightProj);

	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
	XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

	UINT w = mShadowMap->Width();
	UINT h = mShadowMap->Height();

	XMStoreFloat4x4(&mShadowPassCB.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&mShadowPassCB.InvView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&mShadowPassCB.Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&mShadowPassCB.InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&mShadowPassCB.ViewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&mShadowPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
	mShadowPassCB.EyePosW = mLightPosW;
	mShadowPassCB.RenderTargetSize = XMFLOAT2((float)w, (float)h);
	mShadowPassCB.InvRenderTargetSize = XMFLOAT2(1.0f / w, 1.0f / h);
	mShadowPassCB.NearZ = mLightNearZ;
	mShadowPassCB.FarZ = mLightFarZ;

	auto currPassCB = mCurrFrameResource->PassCB.get();
	currPassCB->CopyData(1, mShadowPassCB);
}

void Utility_Manager::UpdateReflectedPassCB(const CTimer& mt)
{
	mReflectedPassCB = mMainPassCB;

	DirectX::XMVECTOR _mirror = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);	//xy Plane
	DirectX::XMMATRIX _r = DirectX::XMMatrixReflect(_mirror);

	for (int i = 0; i < 3; ++i)
	{
		DirectX::XMVECTOR lightDir = DirectX::XMLoadFloat3(&mMainPassCB.Lights[i].Direction);
		DirectX::XMVECTOR reflectedLightDir = DirectX::XMVector3TransformNormal(lightDir, _r);
		DirectX::XMStoreFloat3(&mReflectedPassCB.Lights[i].Direction, reflectedLightDir);
	}

	auto currPassCB = mCurrFrameResource->PassCB.get();
	currPassCB->CopyData(1, mReflectedPassCB);
}

void Utility_Manager::DrawRenderItems(ID3D12GraphicsCommandList * cmdList, const std::vector<RenderItem*>& ritems)
{
	UINT objCBByteSize = d3dutil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	//UINT matCBByteSize = d3dutil::CalcConstantBufferByteSize(sizeof(MaterialConstants));

	auto objectCB = mCurrFrameResource->ObjectCB->Resource();
	//auto matCB = mCurrFrameResource->MaterialBuffer->Resource();//MaterialCB->Resource();

	//auto instanceBuffer = mCurrFrameResource->InstanceBuffer->Resource();
	//UINT InstanceBufByteSize = sizeof(InstanceData);
	//UINT instanceAllbufsize = 1;
	// For each render item...
	for (size_t i = 0; i < ritems.size(); ++i)
	{
		auto ri = ritems[i];

		//if(ri->Visible == FALSE)
		//	continue;

		cmdList->IASetVertexBuffers(0, 1, &ri->Geo->VertexBufferView());
		cmdList->IASetIndexBuffer(&ri->Geo->IndexBufferView());
		cmdList->IASetPrimitiveTopology(ri->PrimitiveType);

		//CD3DX12_GPU_DESCRIPTOR_HANDLE tex(mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		//tex.Offset(ri->Mat->DiffuseSrvHeapIndex, mCbvSrvDescriptorSize);

		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + ri->objCBIndex * objCBByteSize;
		//D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + ri->Mat->MatCBIndex * matCBByteSize;

		cmdList->SetGraphicsRootConstantBufferView(0, objCBAddress);
		
		//D3D12_GPU_VIRTUAL_ADDRESS instAddress = instanceBuffer->GetGPUVirtualAddress() + ri->objCBIndex * InstanceBufByteSize;
		//g_Graphics->Get_CommandList()->SetGraphicsRootShaderResourceView(0, instAddress);
		
		cmdList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
		//instanceAllbufsize += ri->InstanceCount;
	}
}

void Utility_Manager::DrawSceneToShadowMap(void)
{
	const COMMANDLIST& _commandlist = COM_LIST;

	_commandlist->RSSetViewports(1, &mShadowMap->Viewport());
	_commandlist->RSSetScissorRects(1, &mShadowMap->ScissorRect());

	_commandlist->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mShadowMap->Resource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE));

	UINT passCBByteSize = d3dutil::CalcConstantBufferByteSize(sizeof(PassConstants));

	_commandlist->ClearDepthStencilView(mShadowMap->Dsv(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	_commandlist->OMSetRenderTargets(0, nullptr, FALSE, &mShadowMap->Dsv());

	auto passCB = mCurrFrameResource->PassCB->Resource();
	D3D12_GPU_VIRTUAL_ADDRESS passCBAddress = passCB->GetGPUVirtualAddress() + 1 * passCBByteSize;
	_commandlist->SetGraphicsRootConstantBufferView(1, passCBAddress);

	_commandlist->SetPipelineState(mPSOs["shadow_opaque"].Get());

	DrawRenderItems(_commandlist.Get(), mDrawLayer[(int)DrawLayer::DL_OPAUQE]);

	_commandlist->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mShadowMap->Resource(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ));
}

void Utility_Manager::UpdateShadowTransform(const CTimer& mt)
{
	XMVECTOR lightDir = XMLoadFloat3(&mRotatedLightDirections[0]);
	XMVECTOR lightPos = -2.0f * mSceneBounds.Radius * lightDir;
	XMVECTOR targetPos = XMLoadFloat3(&mSceneBounds.Center);
	XMVECTOR lightUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX lightView = XMMatrixLookAtLH(lightPos, targetPos, lightUp);

	XMStoreFloat3(&mLightPosW, lightPos);

	XMFLOAT3 sphereCenterLS;
	XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, lightView));

	float l = sphereCenterLS.x - mSceneBounds.Radius;
	float b = sphereCenterLS.y - mSceneBounds.Radius;
	float n = sphereCenterLS.z - mSceneBounds.Radius;
	float r = sphereCenterLS.x + mSceneBounds.Radius;
	float t = sphereCenterLS.y + mSceneBounds.Radius;
	float f = sphereCenterLS.z + mSceneBounds.Radius;

	mLightNearZ = n;
	mLightFarZ = f;
	XMMATRIX lightProj = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);

	XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	XMMATRIX S = lightView * lightProj * T;
	XMStoreFloat4x4(&mLightView, lightView);
	XMStoreFloat4x4(&mLightProj, lightProj);
	XMStoreFloat4x4(&mShadowTransform, S);
}

void Utility_Manager::OnKeyboardInput(const CTimer& mt)
{
}

void Utility_Manager::UpdateCamera(const CTimer& mt)
{
	for (auto& all : allObj_Update_vec)
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