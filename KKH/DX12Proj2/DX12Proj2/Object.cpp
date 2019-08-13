#include "pch.h"
#include "Object.h"
#include "Utility_Manager.h"
#include "GeometryGenerator.h"

Object::Object(void)
	: Component()
	, m_Comtype(Object::COM_TYPE::CT_STATIC)
	, m_Name(""), m_submeshName(""), m_texName(""), m_matName(""), m_normalTex("")
	, mWorld(MathHelper::Identity4x4())
	//, mView(MathHelper::Identity4x4())
	, mProj(MathHelper::Identity4x4())
	, mTheta(1.5f * PI)
	, mPhi(DirectX::XM_PIDIV4)
	, mRadius(5.0f)
	, m_DrawLayer(DrawLayer::DL_END)
	, mPosition(0.0f, 0.0f, 0.0f), mScaling(0.0f, 0.0f, 0.0f), mRotation(0.0f, 0.0f, 0.0f)
	, m_matCount(0), m_eShapeType(ST_END)
{
}

Object::Object(COM_TYPE _type, std::string _geoname, std::string _submeshname, 
	std::string _texname, std::string _matname, std::string _normalmap, ShapeType eType)
	: Component()
	, m_Comtype(_type)
	, m_Name(_geoname), m_submeshName(_submeshname)
	, m_texName(_texname), m_matName(_matname), m_normalTex(_normalmap)
	, mWorld(MathHelper::Identity4x4())
	, mProj(MathHelper::Identity4x4())
	, mTheta(1.5f * PI)
	, mPhi(DirectX::XM_PIDIV4)
	, mRadius(5.0f)
	, m_DrawLayer(DrawLayer::DL_END)
	, mPosition(0.0f, 0.0f, 0.0f), mScaling(0.0f, 0.0f, 0.0f), mRotation(0.0f, 0.0f, 0.0f)
	, m_matCount(0), m_eShapeType(eType)
{
}

Object::~Object(void)
{
}

bool Object::Ready(void)
{
	return TRUE;
}

bool Object::Update(const CTimer& mt)
{
	return TRUE;
}

bool Object::Render(const CTimer& mt)
{
	return TRUE;
}

bool Object::BuildDescriptorHeaps(void)
{
	return TRUE;
}

void Object::BuildGeometry(void)
{
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData mesh;

	SubmeshGeometry	SubMesh;

	switch (m_eShapeType)
	{
	case ST_BOX:
		mesh = geoGen.CreateBox(1.0f, 1.0f, 1.0f, 3);
		break;

	case ST_GRID:
		mesh = geoGen.CreateGrid(20.0f, 30.0f, 60, 40);
		break;

	case ST_SHPERE:
		mesh = geoGen.CreateSphere(0.5f, 20, 20);
		break;

	case ST_CYLINEDER:
		mesh = geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20);
		break;

	default:
		return;
	}

	SubMesh.IndexCount = (UINT)mesh.Indices32.size();
	SubMesh.StartIndexLocation = 0;// g_totalIndexOffset;
	SubMesh.BaseVertexLocation = 0;// g_totalVertexOffset;

	//g_totalIndexOffset = (UINT)mesh.Indices32.size();
	//g_totalVertexOffset = (UINT)mesh.Vertices.size();

	XMFLOAT3 vMinf3(Infinity, Infinity, Infinity);
	XMFLOAT3 vMaxf3(-Infinity, -Infinity, -Infinity);

	XMVECTOR vMin = XMLoadFloat3(&vMinf3);
	XMVECTOR vMax = XMLoadFloat3(&vMaxf3);

	std::vector<VERTEX> vertices(mesh.Vertices.size());

	for (size_t i = 0; i < vertices.size(); ++i)
	{
		vertices[i].Pos = mesh.Vertices[i].Position;
		vertices[i].Normal = mesh.Vertices[i].Normal;
		vertices[i].TexC = mesh.Vertices[i].TexC;

		XMVECTOR P = XMLoadFloat3(&vertices[i].Pos);
		vMin = XMVectorMin(vMin, P);
		vMax = XMVectorMax(vMax, P);
	}

	BoundingBox bounds;
	XMStoreFloat3(&bounds.Center, 0.5f * (vMin + vMax));
	XMStoreFloat3(&bounds.Extents, 0.5f * (vMax - vMin));
	SubMesh.Bounds = bounds;

	std::vector<std::uint16_t> indices(3 * mesh.Indices32.size());

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(VERTEX);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);


	if (UTIL.Get_Geomesh().find(m_Name) == UTIL.Get_Geomesh().end())
	{
		auto geo = std::make_unique<MeshGeometry>();
		geo->Name = m_Name;

		ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
		CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

		ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
		CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

		geo->VertexBufferGPU = d3dutil::CreateDefaultBuffer(DEVICE.Get(), COM_LIST.Get(), vertices.data(), vbByteSize, geo->VertexBufferUploader);
		geo->IndexBufferGPU = d3dutil::CreateDefaultBuffer(DEVICE.Get(), COM_LIST.Get(), indices.data(), ibByteSize, geo->IndexBufferUploader);

		geo->VertexByteStride = sizeof(VERTEX);
		geo->VertexBufferByteSize = vbByteSize;
		geo->IndexFormat = DXGI_FORMAT_R16_UINT;
		geo->IndexBufferByteSize = ibByteSize;

		geo->DrawArgs[m_submeshName] = SubMesh;
		UTIL.Get_Geomesh()[geo->Name] = std::move(geo);
	}
	else
	{
		UTIL.Get_Geomesh()[m_Name]->DrawArgs[m_submeshName] = SubMesh;
	}
}
