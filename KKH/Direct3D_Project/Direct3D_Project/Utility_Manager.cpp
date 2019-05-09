#include "stdafx.h"
#include "Utility_Manager.h"
#include "Graphic_Manager.h"
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

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(4, slotRootParameter, (UINT)staticSamplers.size(), staticSamplers.data(), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

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
				if (!obj.second->Update(dt))
					return FALSE;
				break;

			case ObjState::OS_RENDER:
				if (!obj.second->Render(dt))
					return FALSE;
				break;
			}
		}
	}

	return TRUE;
}
