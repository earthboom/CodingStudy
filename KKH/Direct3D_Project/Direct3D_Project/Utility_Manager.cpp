#include "stdafx.h"
#include "Utility_Manager.h"
#include "Graphic_Manager.h"
#include "Function.h"

Utility_Manager::Utility_Manager(void)
	: mRootSignature(nullptr)
	, Obj_static_map(new OBJMAP)
	, Obj_dynamic_map(new OBJMAP)
{
	allObj_Update_vec.push_back(Obj_static_map);
	allObj_Update_vec.push_back(Obj_dynamic_map);
}

Utility_Manager::~Utility_Manager(void)
{
}

void Utility_Manager::BuildRootSignature(void)
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

void Utility_Manager::BuildShadersAndInputLayer(void)
{
	HRESULT hr = S_OK;

	mShaders["standardVS"] = d3dutil_Mananger::CompileShader(L"..\\Shaders\\color.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["opaquePS"] = d3dutil_Mananger::CompileShader(L"..\\Shaders\\color.hlsl", nullptr, "PS", "ps_5_1");

	mInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
}

void Utility_Manager::BuildFrameResources(void)
{
	for (int i = 0; i < NumFrameResources; ++i)
	{
		mFrameResources.push_back(std::make_unique<FrameResource>(GRAPHIC->Get_Device().Get()));//, 1, (UINT)mAllRitem.size(), ))		
		mFrameResources[i]->Set_Pass(GRAPHIC->Get_Device().Get(), 1);
		mFrameResources[i]->Set_Object(GRAPHIC->Get_Device().Get(), (UINT)mAllRitem.size());
	}
}

void Utility_Manager::BuildPSOs(void)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	psoDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };
	psoDesc.pRootSignature = mRootSignature.Get();

	psoDesc.VS =
	{ reinterpret_cast<BYTE*>(mShaders["standardVS"]->GetBufferPointer(), mShaders["standardVS"]->GetBufferSize()) };

	psoDesc.PS =
	{ reinterpret_cast<BYTE*>(mShaders["opaquePS"]->GetBufferPointer(), mShaders["opaquePS"]->GetBufferSize()) };

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

	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaqueWireframePsoDesc = psoDesc;
	opaqueWireframePsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	ThrowIfFailed(GRAPHIC->Get_Device()->CreateGraphicsPipelineState(&opaqueWireframePsoDesc, IID_PPV_ARGS(&mPSOs["opaque_wireframe"])));
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
