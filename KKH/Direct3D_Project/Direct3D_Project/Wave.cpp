#include "stdafx.h"
#include "Wave.h"
#include "Utility_Manager.h"
#include "Texture_Manger.h"
#include "Timer_Manager.h"

Wave::Wave(void)
	: Object(),
	mWaveRitem(nullptr)
{
}

Wave::Wave(Object::COM_TYPE _type, std::string _name, std::string _submeshname, std::string _texname, std::string _matname)
	: Object(_type, _name, _submeshname, _texname, _matname),
	mWaveRitem(nullptr)
{
}

Wave::~Wave(void)
{
}

bool Wave::Ready(void)
{
	UTIL.Get_CPWave() = std::make_unique<ComputeWaves>(128, 128, 1.0f, 0.03f, 4.0f, 0.2f);

	BuildDescriptorHeaps();
	BuildGeometry();
	BuildMaterials();
	BuildRenderItem();

	return TRUE;
}

bool Wave::Update(const CTimer& mt)
{
	AnimateMaterials(mt);
	UpdateWaves(mt);

	return TRUE;
}

bool Wave::Render(const CTimer& mt)
{
	return TRUE;
}

void Wave::BuildDescriptorHeaps(void)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(UTIL.Get_SrvDiscriptorHeap()->GetCPUDescriptorHandleForHeapStart());

	hDescriptor.Offset(g_MatCBcount, UTIL.Get_CbvSrvDescriptorSize());

	auto tex = TEX.Get_Textures()[m_texName]->Resource;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = tex->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;
	//srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	GRAPHIC_DEV->CreateShaderResourceView(tex.Get(), &srvDesc, hDescriptor);
}

void Wave::BuildMaterials(void)
{
	auto mat = std::make_unique<Material>();
	mat->Name = m_matName;
	mat->MatCBIndex = g_MatCBcount;
	mat->DiffuseSrvHeapIndex = g_MatCBcount;
	mat->DiffuseAlbedo = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	mat->FresnelR0 = DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f);
	mat->Roughness = 0.0f;

	UTIL.Get_Materials()[m_matName] = std::move(mat);

	++g_MatCBcount;
}

void Wave::BuildRenderItem(void)
{
	auto ritem = std::make_unique<RenderItem>();
	ritem->World = MathHelper::Identity4x4();
	DirectX::XMStoreFloat4x4(&ritem->TexTransform, DirectX::XMMatrixScaling(5.0f, 5.0f, 1.0f));
	ritem->objCBIndex = g_ObjCBcount;
	ritem->Mat = UTIL.Get_Materials()[m_matName].get();
	ritem->Geo = UTIL.Get_Geomesh()[m_Name].get();
	ritem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	ritem->IndexCount = ritem->Geo->DrawArgs[m_submeshName].IndexCount;
	ritem->StartIndexLocation = ritem->Geo->DrawArgs[m_submeshName].StartIndexLocation;
	ritem->BaseVertexLocation = ritem->Geo->DrawArgs[m_submeshName].BaseVertexLocation;

	mWaveRitem = ritem.get();

	UTIL.Get_Drawlayer((int)DrawLayer::DL_TRANSPARENT).push_back(ritem.get());

	UTIL.Get_Ritemvec().push_back(std::move(ritem));

	++g_ObjCBcount;
}

void Wave::AnimateMaterials(const CTimer& mt)
{
	//Scroll the water material texture coordinate
	auto waterMat = UTIL.Get_Materials()[m_matName].get();

	float& tu = waterMat->MatTransform(3, 0);
	float& tv = waterMat->MatTransform(3, 1);

	tu += 0.1f * mt.DeltaTime();
	tv += 0.02f * mt.DeltaTime();

	if (tu >= 1.0f)
		tu -= 1.0f;

	if (tv >= 1.0f)
		tv -= 1.0f;

	waterMat->MatTransform(3, 0) = tu;
	waterMat->MatTransform(3, 1) = tv;

	waterMat->NumFrameDirty = NumFrameResources;
}

void Wave::UpdateWaves(const CTimer& mt)
{
	static float t_base = 0.0f;
	if ((mt.TotalTime() - t_base) >= 0.25f)
	{
		t_base += 0.25f;

		int i = MathHelper::Rand(4, UTIL.Get_CPWave()->RowCount() - 5);
		int j = MathHelper::Rand(4, UTIL.Get_CPWave()->ColumnCount() - 5);

		float r = MathHelper::RandF(0.2f, 0.5f);

		UTIL.Get_CPWave()->Disturb(i, j, r);
	}

	UTIL.Get_CPWave()->Update(mt);

	auto currWaveCB = UTIL.Get_CurrFrameResource()->WavesVB.get();
	for (int i = 0; i < UTIL.Get_CPWave()->VertexCount(); ++i)
	{
		Vertex v;

		v.Pos = UTIL.Get_CPWave()->Position(i);
		v.Normal = UTIL.Get_CPWave()->Normal(i);

		v.TexC.x = 0.5f + v.Pos.x / UTIL.Get_CPWave()->Width();
		v.TexC.y = 0.5f - v.Pos.z / UTIL.Get_CPWave()->Depth();

		currWaveCB->CopyData(i, v);
	}

	mWaveRitem->Geo->VertexBufferGPU = currWaveCB->Resource();
}

void Wave::BuildGeometry(void)
{
	std::vector<std::uint16_t> indices(3 * UTIL.Get_CPWave()->TriangleCount());	//3 indices per face
	assert(UTIL.Get_CPWave()->VertexCount() < 0x0000ffff);

	int m = UTIL.Get_CPWave()->RowCount();
	int n = UTIL.Get_CPWave()->ColumnCount();
	int k = 0;
	for (int i = 0; i < m - 1; ++i)
	{
		for (int j = 0; j < n - 1; ++j)
		{
			indices[k] = i * n + j;
			indices[k + 1] = i * n + j + 1;
			indices[k + 2] = (i + 1) * n + j;

			indices[k + 3] = (i + 1) * n + j;
			indices[k + 4] = i * n + j + 1;
			indices[k + 5] = (i + 1) * n + j + 1;

			k += 6;
		}
	}

	UINT vByteSize = UTIL.Get_CPWave()->VertexCount() * sizeof(Vertex);
	UINT iByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = m_Name;

	geo->VertexBufferCPU = nullptr;
	geo->VertexBufferGPU = nullptr;

	ThrowIfFailed(D3DCreateBlob(iByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), iByteSize);
	
	geo->IndexBufferGPU = D3DUTIL.CreateDefaultBuffer(GRAPHIC_DEV.Get(), COM_LIST.Get(), indices.data(), iByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Vertex);
	geo->VertexBufferByteSize = vByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = iByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.BaseVertexLocation = 0;
	submesh.StartIndexLocation = 0;

	geo->DrawArgs[m_submeshName] = submesh;

	UTIL.Get_Geomesh()[geo->Name] = std::move(geo);
}

std::shared_ptr<Wave> Wave::Create(Object::COM_TYPE _type, std::string _name, std::string _submeshname, std::string _texname, std::string _matname)
{
	WAVE pWave = std::make_shared<Wave>(_type, _name, _submeshname, _texname, _matname);
	if (!pWave) return nullptr;

	return pWave;
}
