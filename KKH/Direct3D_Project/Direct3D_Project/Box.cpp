#include "stdafx.h"
#include "Box.h"
#include "GeometryGenerator.h"
#include "FrameResource.h"
#include "d3dutil_Manager.h"
#include "Utility_Manager.h"
#include "Struct.h"

Box::Box(void)
	: mGeoName(""), mWidth(0.0f), mHeight(0.0f), mDepth(0.0f), mSubdiv(0U)
{
}

Box::~Box(void)
{
}

bool Box::Ready(void)
{
	BuildBox();
	BuildRenderItem();

	return TRUE;
}

bool Box::Update(const float & dt)
{
	return TRUE;
}

bool Box::Render(const float & dt)
{
	return TRUE;
}

void Box::BuildBox(void)
{
	GeometryGenerator geoBox;
	GeometryGenerator::MeshData box = geoBox.CreateBox(1.5f, 0.5f, 1.5f, 3);

	UINT VertexOffset = 0;
	UINT IndexOffset = 0;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)box.Indices32.size();
	submesh.StartIndexLocation = VertexOffset;
	submesh.BaseVertexLocation = IndexOffset;

	auto vertexCount = box.Vertices.size();

	std::vector<Vertex> vertices(vertexCount);

	for (size_t i=0; i<box.Vertices.size(); ++i)
	{
		vertices[i].Pos = box.Vertices[i].Position;
		//vertices[i].Color = DirectX::XMFLOAT4(DirectX::Colors::DarkGreen);
	}

	std::vector<std::uint16_t> indices;
	indices.insert(indices.end(), std::begin(box.GetIndices16()), std::end(box.GetIndices16()));

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = mGeoName;

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = D3DUTIL.CreateDefaultBuffer(GRAPHIC->Get_Device().Get(), GRAPHIC->Get_CommandList().Get(), vertices.data(), vbByteSize, geo->VertexBufferUploader);
	geo->IndexBufferGPU = D3DUTIL.CreateDefaultBuffer(GRAPHIC->Get_Device().Get(), GRAPHIC->Get_CommandList().Get(), indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Vertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	geo->DrawArgs["Box"] = submesh;

	UTIL.Get_Geomesh()[geo->Name] = std::move(geo);
}

void Box::BuildRenderItem(void)
{
	auto boxRitem = std::make_unique<RenderItem>();
	DirectX::XMStoreFloat4x4(&boxRitem->World, DirectX::XMMatrixScaling(2.0f, 2.0f, 2.0f) * DirectX::XMMatrixTranslation(0.0f, 0.5f, 0.0f));
	boxRitem->objCBIndex = 0;
	boxRitem->Geo = UTIL.Get_Geomesh()[mGeoName].get();
	boxRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	boxRitem->IndexCount = boxRitem->Geo->DrawArgs["Box"].IndexCount;
	boxRitem->StartIndexLocation = boxRitem->Geo->DrawArgs["Box"].BaseVertexLocation;
	boxRitem->BaseVertexLocation = boxRitem->Geo->DrawArgs["Box"].BaseVertexLocation;

	UTIL.Get_Drawlayer((int)DrawLayer::DL_OPAUQE).push_back(boxRitem.get());

	UTIL.Get_Ritemvec().push_back(std::move(boxRitem));
}

void Box::Set_BoxAtt(const std::string& _name, const float & w, const float & h, const float & d, const uint32_t & s)
{
	mGeoName	= _name;
	mWidth		= w;
	mHeight		= h;
	mDepth		= d;
	mSubdiv		= s;
}

std::shared_ptr<Box> Box::Create(std::string _name, Object::COM_TYPE _type, std::string _geoname, float _w, float _h, float _d, float _subdiv)
{
	BOX pBox = std::make_shared<Box>();
	if (!pBox)	return nullptr;

	pBox->Get_Objname() = _name;
	pBox->Get_Comtype() = _type;
	pBox->Set_BoxAtt(_geoname, _w, _h, _d, _subdiv);

	return pBox;
}
