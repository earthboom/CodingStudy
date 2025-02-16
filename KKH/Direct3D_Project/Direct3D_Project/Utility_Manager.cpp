#include "stdafx.h"
#include "Utility_Manager.h"
#include "Graphic_Manager.h"
#include "Timer_Manager.h"
#include "Texture_Manger.h"
#include "Camera_Manager.h"

Utility_Manager::Utility_Manager(void)
	: mRootSignature(nullptr), mPostProcessRootSignature(nullptr)
	, mSrvDescriptorHeap(nullptr)
	, Obj_static_map(new OBJMAP)
	, Obj_dynamic_map(new OBJMAP)
	//, mTheta(1.5f * PI), mPhi(0.2f * PI), mRadius(15.0f)
	, mView(MathHelper::Identity4x4())
	//, mProj(MathHelper::Identity4x4())
	, mCurrFrameResource(nullptr), mCurrFrameResourceIndex(0)
	, mCbvSrvDescriptorSize(0)
	, mCurrState(OS_END)
{
	allObj_Update_vec.push_back(Obj_static_map);
	allObj_Update_vec.push_back(Obj_dynamic_map);
}

Utility_Manager::~Utility_Manager(void)
{

}

void Utility_Manager::UtilityInitialize(void)
{
	mBlurFilter = std::make_unique<BlurFilter>(GRAPHIC_DEV.Get(), WINSIZE_X, WINSIZE_Y, DXGI_FORMAT_R8G8B8A8_UNORM);
}

void Utility_Manager::BuildRootSignature(void)
{
	CD3DX12_DESCRIPTOR_RANGE texTable;
	//texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 4, 0, 0);

	CD3DX12_ROOT_PARAMETER slotRootParameter[4];

	//slotRootParameter[0].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);
	//slotRootParameter[1].InitAsConstantBufferView(0);
	//slotRootParameter[2].InitAsConstantBufferView(1);
	//slotRootParameter[3].InitAsConstantBufferView(2);

	slotRootParameter[0].InitAsConstantBufferView(0);
	slotRootParameter[1].InitAsConstantBufferView(1);
	slotRootParameter[2].InitAsShaderResourceView(0, 1);
	slotRootParameter[3].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);

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

	ThrowIfFailed(GRAPHIC_DEV->CreateRootSignature(
		0, serializedRootSig->GetBufferPointer(), 
		serializedRootSig->GetBufferSize(), IID_PPV_ARGS(mRootSignature.GetAddressOf())));
}

void Utility_Manager::BuildPostProcessRootSignature(void)
{
	CD3DX12_DESCRIPTOR_RANGE srvTable;
	srvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	CD3DX12_DESCRIPTOR_RANGE uavTable;
	uavTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);

	CD3DX12_ROOT_PARAMETER slotRootParameter[3];

	slotRootParameter[0].InitAsConstants(12, 0);
	slotRootParameter[1].InitAsDescriptorTable(1, &srvTable);
	slotRootParameter[2].InitAsDescriptorTable(1, &uavTable);

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(3, slotRootParameter, 0, nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());

	ThrowIfFailed(hr);

	ThrowIfFailed(GRAPHIC_DEV->CreateRootSignature(
		0, serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(), IID_PPV_ARGS(mPostProcessRootSignature.GetAddressOf())));
}

void Utility_Manager::BuildDescriptorHeaps(void)
{
	const int textureDescriptorCount = 4;
	const int blurDescriptorCount = 4;

	//Create the SRV heap
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = textureDescriptorCount + blurDescriptorCount;//7;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(GRAPHIC_DEV->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&mSrvDescriptorHeap)));
}

void Utility_Manager::BuildShadersAndInputLayer(void)
{
	const D3D_SHADER_MACRO defines[] = { "FOG", "1", NULL, NULL };
	const D3D_SHADER_MACRO alphaTestDefines[] = { "FOG", "1", "ALPHA_TEST", "1", NULL, NULL };

	////===== [ Standard ] ======
	mShaders["standardVS"] = d3dutil_Manager::CompileShader(L"..\\Shaders\\Default.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["opaquePS"] = d3dutil_Manager::CompileShader(L"..\\Shaders\\Default.hlsl", nullptr, "PS", "ps_5_1");
	//mShaders["alphaTestedPS"] = d3dutil_Manager::CompileShader(L"../Shaders/Default.hlsl", alphaTestDefines, "PS", "ps_5_0");
	////=========================

	////==== [ GS ] ================
	//mShaders["treeSpriteVS"] = d3dutil_Manager::CompileShader(L"../Shaders/TreeSprite.hlsl", nullptr, "VS", "vs_5_0");
	//mShaders["treeSpriteGS"] = d3dutil_Manager::CompileShader(L"../Shaders/TreeSprite.hlsl", nullptr, "GS", "gs_5_0");
	//mShaders["treeSpritePS"] = d3dutil_Manager::CompileShader(L"../Shaders/TreeSprite.hlsl", alphaTestDefines, "PS", "ps_5_0");
	////========================

	//mShaders["standardVS"] = d3dutil_Manager::LoadBinary(L"../Shaders/Default_vs.cso");
	//mShaders["opaquePS"] = d3dutil_Mananger::LoadBinary(L"../Shaders/Default_ps.cso");
	//mShaders["alphaTestedPS"] = d3dutil_Mananger::LoadBinary(L"../Shaders/Default_ps.cso");

	////==== [ CS (Blur) ] =================
	//mShaders["horzBlurCS"] = d3dutil_Manager::CompileShader(L"../Shaders/Blur.hlsl", nullptr, "HorzBlurCS", "cs_5_0");
	//mShaders["vertBlurCS"] = d3dutil_Manager::CompileShader(L"../Shaders/Blur.hlsl", nullptr, "VertBlurCS", "cs_5_0");
	////====================================

	//==== [ Tessellation Stage ] =======
	//mShaders["tessVS"] = d3dutil_Manager::CompileShader(L"../Shaders/Tessellation.hlsl", nullptr, "VS", "vs_5_0");
	//mShaders["tessHS"] = d3dutil_Manager::CompileShader(L"../Shaders/Tessellation.hlsl", nullptr, "HS", "hs_5_0");
	//mShaders["tessDS"] = d3dutil_Manager::CompileShader(L"../Shaders/Tessellation.hlsl", nullptr, "DS", "ds_5_0");
	//mShaders["tessPS"] = d3dutil_Manager::CompileShader(L"../Shaders/Tessellation.hlsl", nullptr, "PS", "ps_5_0");

	//Bazier
	//mShaders["tessVS"] = d3dutil_Manager::CompileShader(L"../Shaders/BezierTessellation.hlsl", nullptr, "VS", "vs_5_0");
	//mShaders["tessHS"] = d3dutil_Manager::CompileShader(L"../Shaders/BezierTessellation.hlsl", nullptr, "HS", "hs_5_0");
	//mShaders["tessDS"] = d3dutil_Manager::CompileShader(L"../Shaders/BezierTessellation.hlsl", nullptr, "DS", "ds_5_0");
	//mShaders["tessPS"] = d3dutil_Manager::CompileShader(L"../Shaders/BezierTessellation.hlsl", nullptr, "PS", "ps_5_0");
	//===================================

	mInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	//==== [ Tessellation Stage ] ======
	//mInputLayout =
	//{
	//	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	//};
	//==================================

	//mTreeSpriteInputLayout =
	//{
	//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	//	{ "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	//};
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
		mFrameResources.push_back(std::make_unique<FrameResource>(GRAPHIC_DEV.Get(), 2, (UINT)mAllRitem.size(), (UINT)mMaterials.size()));
		//mFrameResources.push_back(std::make_unique<FrameResource>(GRAPHIC_DEV.Get(), 2, (UINT)mAllRitem.size(), (UINT)mMaterials.size(), mCPWave->VertexCount()));
	}
}

void Utility_Manager::BuildPSOs(void)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	psoDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };
	psoDesc.pRootSignature = mRootSignature.Get();

	////===== [ Standard ] ======
	psoDesc.VS = { reinterpret_cast<BYTE*>(mShaders["standardVS"]->GetBufferPointer()), mShaders["standardVS"]->GetBufferSize() };
	psoDesc.PS = { reinterpret_cast<BYTE*>(mShaders["opaquePS"]->GetBufferPointer()), mShaders["opaquePS"]->GetBufferSize() };
	////=========================


	//==== [ Tessellation Stage ] =======
	//psoDesc.VS = { reinterpret_cast<BYTE*>(mShaders["tessVS"]->GetBufferPointer()), mShaders["tessVS"]->GetBufferSize() };
	//psoDesc.HS = { reinterpret_cast<BYTE*>(mShaders["tessHS"]->GetBufferPointer()), mShaders["tessHS"]->GetBufferSize() };
	//psoDesc.DS = { reinterpret_cast<BYTE*>(mShaders["tessDS"]->GetBufferPointer()), mShaders["tessDS"]->GetBufferSize() };
	//psoDesc.PS = { reinterpret_cast<BYTE*>(mShaders["tessPS"]->GetBufferPointer()), mShaders["tessPS"]->GetBufferSize() };
	//===================================


	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	//psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;	//(tessellation option)
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;	//(tessellation option)
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = GRAPHIC->Get_BackBufferFormat();
	psoDesc.SampleDesc.Count = GRAPHIC->Get_4xMsaaState() ? 4 : 1;
	psoDesc.SampleDesc.Quality = GRAPHIC->Get_4xMsaaState() ? (GRAPHIC->Get_4xMsaaQuality() - 1) : 0;
	psoDesc.DSVFormat = GRAPHIC->Get_DepthStencilFormat();

	ThrowIfFailed(GRAPHIC_DEV->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPSOs["opaque"])));


	////PSO for transparent objects
	//D3D12_GRAPHICS_PIPELINE_STATE_DESC transparentPsoDesc = psoDesc;

	//D3D12_RENDER_TARGET_BLEND_DESC transparencyBlenDesc;
	//transparencyBlenDesc.BlendEnable = TRUE;
	//transparencyBlenDesc.LogicOpEnable = FALSE;
	//transparencyBlenDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	//transparencyBlenDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	//transparencyBlenDesc.BlendOp = D3D12_BLEND_OP_ADD;
	//transparencyBlenDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	//transparencyBlenDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	//transparencyBlenDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	//transparencyBlenDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
	//transparencyBlenDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//transparentPsoDesc.BlendState.RenderTarget[0] = transparencyBlenDesc;
	//ThrowIfFailed(GRAPHIC_DEV->CreateGraphicsPipelineState(&transparentPsoDesc, IID_PPV_ARGS(&mPSOs["transparent"])));

	////PSO for marking stencil mirros.
	//CD3DX12_BLEND_DESC mirrorBlendState(D3D12_DEFAULT);
	//mirrorBlendState.RenderTarget[0].RenderTargetWriteMask = 0;

	//D3D12_DEPTH_STENCIL_DESC mirrorDSS;
	//mirrorDSS.DepthEnable = TRUE;
	//mirrorDSS.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	//mirrorDSS.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	//mirrorDSS.StencilEnable = TRUE;
	//mirrorDSS.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
	//mirrorDSS.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;

	//mirrorDSS.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	//mirrorDSS.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	//mirrorDSS.FrontFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
	//mirrorDSS.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	//mirrorDSS.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	//mirrorDSS.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	//mirrorDSS.BackFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
	//mirrorDSS.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	//D3D12_GRAPHICS_PIPELINE_STATE_DESC markMirrorsPosDesc = psoDesc;
	//markMirrorsPosDesc.BlendState = mirrorBlendState;
	//markMirrorsPosDesc.DepthStencilState = mirrorDSS;
	//ThrowIfFailed(GRAPHIC_DEV->CreateGraphicsPipelineState(&markMirrorsPosDesc, IID_PPV_ARGS(&mPSOs["markStencilMirrors"])));

	////PSO for stencil reflections.
	//D3D12_DEPTH_STENCIL_DESC reflectionsDDS;
	//reflectionsDDS.DepthEnable = TRUE;
	//reflectionsDDS.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//reflectionsDDS.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	//reflectionsDDS.StencilEnable = TRUE;
	//reflectionsDDS.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
	//reflectionsDDS.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;

	//reflectionsDDS.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	//reflectionsDDS.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	//reflectionsDDS.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	//reflectionsDDS.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;

	//reflectionsDDS.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	//reflectionsDDS.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	//reflectionsDDS.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	//reflectionsDDS.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;

	//D3D12_GRAPHICS_PIPELINE_STATE_DESC drawReflectionsPsoDesc = psoDesc;
	//drawReflectionsPsoDesc.DepthStencilState = reflectionsDDS;
	//drawReflectionsPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	//drawReflectionsPsoDesc.RasterizerState.FrontCounterClockwise = TRUE;
	//ThrowIfFailed(GRAPHIC_DEV->CreateGraphicsPipelineState(&drawReflectionsPsoDesc, IID_PPV_ARGS(&mPSOs["drawStencilReflections"])));
	//
	////PSO for shodow objects
	//D3D12_DEPTH_STENCIL_DESC shadowDSS;
	//shadowDSS.DepthEnable = TRUE;
	//shadowDSS.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//shadowDSS.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	//shadowDSS.StencilEnable = TRUE;
	//shadowDSS.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
	//shadowDSS.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;

	//shadowDSS.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	//shadowDSS.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	//shadowDSS.FrontFace.StencilPassOp = D3D12_STENCIL_OP_INCR;
	//shadowDSS.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;

	//shadowDSS.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	//shadowDSS.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	//shadowDSS.BackFace.StencilPassOp = D3D12_STENCIL_OP_INCR;
	//shadowDSS.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;

	//D3D12_GRAPHICS_PIPELINE_STATE_DESC shadowPsoDesc = transparentPsoDesc;
	//shadowPsoDesc.DepthStencilState = shadowDSS;
	//ThrowIfFailed(GRAPHIC_DEV->CreateGraphicsPipelineState(&shadowPsoDesc, IID_PPV_ARGS(&mPSOs["shadow"])));

	//PSO for alpha tested objects
	//D3D12_GRAPHICS_PIPELINE_STATE_DESC alphaTestedPSoDesc = psoDesc;

	//alphaTestedPSoDesc.PS = 
	//{	reinterpret_cast<BYTE*>(mShaders["alphaTestedPS"]->GetBufferPointer()), mShaders["alphaTestedPS"]->GetBufferSize()	};

	//alphaTestedPSoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//ThrowIfFailed(GRAPHIC_DEV->CreateGraphicsPipelineState(&alphaTestedPSoDesc, IID_PPV_ARGS(&mPSOs["alphaTested"])));



	//PSO for tree sprite
	//D3D12_GRAPHICS_PIPELINE_STATE_DESC treeSpritePsoDesc = psoDesc;
	//treeSpritePsoDesc.VS =
	//{	reinterpret_cast<BYTE*>(mShaders["treeSpriteVS"]->GetBufferPointer()), mShaders["treeSpriteVS"]->GetBufferSize()	};

	//treeSpritePsoDesc.GS =
	//{	reinterpret_cast<BYTE*>(mShaders["treeSpriteGS"]->GetBufferPointer()), mShaders["treeSpriteGS"]->GetBufferSize()	};

	//treeSpritePsoDesc.PS =
	//{	reinterpret_cast<BYTE*>(mShaders["treeSpritePS"]->GetBufferPointer()), mShaders["treeSpritePS"]->GetBufferSize()	};

	//treeSpritePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	//treeSpritePsoDesc.InputLayout = { mTreeSpriteInputLayout.data(), (UINT)mTreeSpriteInputLayout.size() };
	//treeSpritePsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

	//ThrowIfFailed(GRAPHIC_DEV->CreateGraphicsPipelineState(&treeSpritePsoDesc, IID_PPV_ARGS(&mPSOs["treeSprite"])));


	//blur================
	//D3D12_COMPUTE_PIPELINE_STATE_DESC horzBlurPSO = {};
	//horzBlurPSO.pRootSignature = mPostProcessRootSignature.Get();
	//horzBlurPSO.CS =
	//{ reinterpret_cast<BYTE*>(mShaders["horzBlurCS"]->GetBufferPointer()), mShaders["horzBlurCS"]->GetBufferSize() };
	//horzBlurPSO.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	//ThrowIfFailed(GRAPHIC_DEV->CreateComputePipelineState(&horzBlurPSO, IID_PPV_ARGS(&mPSOs["horzBlur"])));


	//D3D12_COMPUTE_PIPELINE_STATE_DESC vertBlurPSO = {};
	//vertBlurPSO.pRootSignature = mPostProcessRootSignature.Get();
	//vertBlurPSO.CS =
	//{ reinterpret_cast<BYTE*>(mShaders["vertBlurCS"]->GetBufferPointer()), mShaders["vertBlurCS"]->GetBufferSize() };
	//vertBlurPSO.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	//ThrowIfFailed(GRAPHIC_DEV->CreateComputePipelineState(&vertBlurPSO, IID_PPV_ARGS(&mPSOs["vertBlur"])));




	//D3D12_GRAPHICS_PIPELINE_STATE_DESC opaqueWireframePsoDesc = psoDesc;
	//opaqueWireframePsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	//ThrowIfFailed(GRAPHIC->Get_Device()->CreateGraphicsPipelineState(&opaqueWireframePsoDesc, IID_PPV_ARGS(&mPSOs["opaque_wireframe"])));
}

void Utility_Manager::OnResize(void)
{
	//DirectX::XMMATRIX p = DirectX::XMMatrixPerspectiveFovLH(0.25f * PI, AspectRatio(), 1.0f, 1000.0f);
	//DirectX::XMStoreFloat4x4(&g_Proj, p);

	CURR_CAM->SetLens(0.25f * PI, AspectRatio(), 1.0f, 1000.0f);

	if (g_ScreenBlur)
	{
		if (mBlurFilter != nullptr)
			mBlurFilter->OnResize(WINSIZE_X, WINSIZE_Y);
	}
}

bool Utility_Manager::Object_Create(OBJECT& obj)
{
	if (obj == nullptr) return FALSE;

	if (obj->Get_Comtype() == Object::COM_TYPE::CT_STATIC)
		Obj_static_map->insert(OBJMAP::value_type(obj->Get_Objname(), obj));
	else if (obj->Get_Comtype() == Object::COM_TYPE::CT_DYNAMIC)
		Obj_dynamic_map->insert(OBJMAP::value_type(obj->Get_Objname(), obj));

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

	if (mCurrFrameResource->Fence != 0 && GRAPHIC->Get_Fence()->GetCompletedValue() < mCurrFrameResource->Fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
		ThrowIfFailed(GRAPHIC->Get_Fence()->SetEventOnCompletion(mCurrFrameResource->Fence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
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
	UpdateMaterialCBs(mt);
	UpdateMainPassCB(mt);
	UpdateReflectedPassCB(mt);

	return TRUE;
}

bool Utility_Manager::Object_Render(const CTimer& mt)//, OBJMAP& _objmap)
{
	const COMMANDLIST& _commandlist = COM_LIST;
	const PGRAPHIC& _graphic = GRAPHIC;

	auto cmdListAlloc = mCurrFrameResource->CmdListAlloc;

	ThrowIfFailed(cmdListAlloc->Reset());
	ThrowIfFailed(_commandlist->Reset(cmdListAlloc.Get(), mPSOs["opaque"].Get()));
	
	_commandlist->RSSetViewports(1, &_graphic->Get_ScreenViewport());
	_commandlist->RSSetScissorRects(1, &_graphic->Get_ScissorRect());

	_commandlist->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_graphic->Get_CurrentBackBuffer_Resource(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	//_commandlist->ClearRenderTargetView(_graphic->Get_CurrentBackBufferView_Handle(), (float*)&mMainPassCB.FogColor, 0, nullptr);
	_commandlist->ClearRenderTargetView(_graphic->Get_CurrentBackBufferView_Handle(), Colors::LightSteelBlue, 0, nullptr);
	_commandlist->ClearDepthStencilView(_graphic->Get_DepthStencilView_Handle(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	_commandlist->OMSetRenderTargets(1, &_graphic->Get_CurrentBackBufferView_Handle(), TRUE, &_graphic->Get_DepthStencilView_Handle());

	ID3D12DescriptorHeap* descriptorHeaps[] = { mSrvDescriptorHeap.Get() };
	_commandlist->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	_commandlist->SetGraphicsRootSignature(mRootSignature.Get());

	UINT passCBByteSize = d3dutil_Manager::CalcConstantBufferByteSize(sizeof(PassConstants));

	auto passCB = mCurrFrameResource->PassCB->Resource();
	_commandlist->SetGraphicsRootConstantBufferView(1, passCB->GetGPUVirtualAddress());

	auto matBuffer = mCurrFrameResource->MaterialBuffer->Resource();
	_commandlist->SetGraphicsRootConstantBufferView(2, matBuffer->GetGPUVirtualAddress());

	_commandlist->SetGraphicsRootDescriptorTable(3, mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

	DrawRenderItems(_commandlist.Get(), mDrawLayer[(int)DrawLayer::DL_OPAUQE]);

	//_commandlist->OMSetStencilRef(1);
	//_commandlist->SetPipelineState(mPSOs["markStencilMirrors"].Get());
	//DrawRenderItems(_commandlist.Get(), mDrawLayer[(int)DrawLayer::DL_MIRROR]);

	//_commandlist->SetGraphicsRootConstantBufferView(2, passCB->GetGPUVirtualAddress() + 1 * passCBByteSize);
	//_commandlist->SetPipelineState(mPSOs["drawStencilReflections"].Get());
	//DrawRenderItems(_commandlist.Get(), mDrawLayer[(int)DrawLayer::DL_REFLECTED]);

	//_commandlist->SetGraphicsRootConstantBufferView(2, passCB->GetGPUVirtualAddress());
	//_commandlist->OMSetStencilRef(0);

	//_commandlist->SetPipelineState(mPSOs["alphaTested"].Get());
	//DrawRenderItems(_commandlist.Get(), mDrawLayer[(int)DrawLayer::DL_ALPHATESTED]);
	//
	//_commandlist->SetPipelineState(mPSOs["treeSprite"].Get());
	//DrawRenderItems(_commandlist.Get(), mDrawLayer[(int)DrawLayer::DL_ALPHATESTED_TREESPRITE]);

	//_commandlist->SetPipelineState(mPSOs["transparent"].Get());
	//DrawRenderItems(_commandlist.Get(), mDrawLayer[(int)DrawLayer::DL_TRANSPARENT]);

	//_commandlist->SetPipelineState(mPSOs["shadow"].Get());
	//DrawRenderItems(_commandlist.Get(), mDrawLayer[(int)DrawLayer::DL_SHADOW]);


	//mBlurFilter->Execute(_commandlist.Get(), mPostProcessRootSignature.Get(),
	//	mPSOs["horzBlur"].Get(), mPSOs["vertBlur"].Get(), _graphic->Get_CurrentBackBuffer_Resource(), 4);

	//_commandlist->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_graphic->Get_CurrentBackBuffer_Resource(),
	//	D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_COPY_DEST));

	//_commandlist->CopyResource(_graphic->Get_CurrentBackBuffer_Resource(), mBlurFilter->Output());

	//_commandlist->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_graphic->Get_CurrentBackBuffer_Resource(),
	//	D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PRESENT));

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

	XMStoreFloat4x4(&mMainPassCB.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&mMainPassCB.InvView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&mMainPassCB.Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&mMainPassCB.InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&mMainPassCB.ViewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&mMainPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
	mMainPassCB.EyePosW = CURR_CAM->GetPosition3f();//g_EyePos;
	mMainPassCB.RenderTargetSize = XMFLOAT2((float)WINSIZE_X, (float)WINSIZE_Y);
	mMainPassCB.InvRenderTargetSize = XMFLOAT2(1.0f / WINSIZE_X, 1.0f / WINSIZE_Y);
	mMainPassCB.NearZ = 1.0f;
	mMainPassCB.FarZ = 1000.0f;
	mMainPassCB.TotalTime = mt.TotalTime();
	mMainPassCB.DeltaTime = mt.DeltaTime();
	mMainPassCB.AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };
	mMainPassCB.Lights[0].Direction = { 0.57735f, -0.57735f, 0.57735f };
	mMainPassCB.Lights[0].Strength = { 0.9f, 0.9f, 0.8f };
	mMainPassCB.Lights[1].Direction = { -0.57735f, -0.57735f, 0.57735f };
	mMainPassCB.Lights[1].Strength = { 0.3f, 0.3f, 0.3f };
	mMainPassCB.Lights[2].Direction = { 0.0f, -0.707f, -0.707f };
	mMainPassCB.Lights[2].Strength = { 0.15f, 0.15f, 0.15f };

	auto currPassCB = mCurrFrameResource->PassCB.get();
	currPassCB->CopyData(0, mMainPassCB);
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
	UINT objCBByteSize = d3dutil_Manager::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	UINT matCBByteSize = d3dutil_Manager::CalcConstantBufferByteSize(sizeof(MaterialConstants));

	auto objectCB = mCurrFrameResource->ObjectCB->Resource();
	auto matCB = mCurrFrameResource->MaterialBuffer->Resource();//MaterialCB->Resource();

	// For each render item...
	for (size_t i = 0; i < ritems.size(); ++i)
	{
		auto ri = ritems[i];

		cmdList->IASetVertexBuffers(0, 1, &ri->Geo->VertexBufferView());
		cmdList->IASetIndexBuffer(&ri->Geo->IndexBufferView());
		cmdList->IASetPrimitiveTopology(ri->PrimitiveType);

		CD3DX12_GPU_DESCRIPTOR_HANDLE tex(mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		tex.Offset(ri->Mat->DiffuseSrvHeapIndex, mCbvSrvDescriptorSize);

		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + ri->objCBIndex * objCBByteSize;
		D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + ri->Mat->MatCBIndex * matCBByteSize;

		cmdList->SetGraphicsRootDescriptorTable(0, tex);
		cmdList->SetGraphicsRootConstantBufferView(1, objCBAddress);
		cmdList->SetGraphicsRootConstantBufferView(3, matCBAddress);

		cmdList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
	}
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