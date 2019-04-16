#include "stdafx.h"
#include "GeometryGenerator.h"
#include "Const.h"

GeometryGenerator::MeshData GeometryGenerator::CreateBox(float w, float h, float depth, uint32 numSubdivisions)
{
	return MeshData();
}

GeometryGenerator::MeshData GeometryGenerator::CreateCylinder(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount)
{
	MeshData meshData;

	float stackHeight = height / stackCount;

	float radiusStep = (topRadius - bottomRadius) / stackCount;

	uint32 ringCount = stackCount + 1;

	for (uint32 i = 0; i < ringCount; ++i)
	{
		float y = -0.5f * height + i * stackHeight;
		float r = bottomRadius + i * radiusStep;

		float dTheta = 2.0f * PI / sliceCount;
		for (uint32 j = 0; j <= sliceCount; ++j)
		{
			GeometryGenerator::Vertex vertex;

			float c = cosf(j * dTheta);
			float s = sinf(j * dTheta);

			vertex.Position = DirectX::XMFLOAT3(r * c, y, r * s);

			vertex.TexC.x = (float)j / sliceCount;
			vertex.TexC.y = 1.0f - (float)i / stackCount;

			vertex.TangentU = DirectX::XMFLOAT3(-s, 0.0f, c);

			float dr = bottomRadius - topRadius;
			DirectX::XMFLOAT3 bitangent(dr * c, -height, dr * s);

			DirectX::XMVECTOR T = DirectX::XMLoadFloat3(&vertex.TangentU);
			DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&bitangent);
			DirectX::XMVECTOR N = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(T, B));
			DirectX::XMStoreFloat3(&vertex.Normal, N);

			meshData.Vertices.push_back(vertex);
		}
	}

	uint32 ringVertexCount = sliceCount + 1;

	for (uint32 i = 0; i < stackCount; ++i)
	{
		for (uint32 j = 0; j < sliceCount; ++j)
		{
			meshData.Indices32.push_back(i * ringVertexCount + j);
			meshData.Indices32.push_back((i + 1) * ringVertexCount + j);
			meshData.Indices32.push_back((i + 1) * ringVertexCount + j + 1);

			meshData.Indices32.push_back(i * ringVertexCount + j);
			meshData.Indices32.push_back((i + 1) * ringVertexCount + j + 1);
			meshData.Indices32.push_back(i * ringVertexCount + j + 1);
		}
	}

	BuildCylinderTopCap(bottomRadius, topRadius, height, sliceCount, stackCount, meshData);
	BuildCylinderBottomCap(bottomRadius, topRadius, height, sliceCount, stackCount, meshData);

	return meshData;
}

void GeometryGenerator::BuildCylinderTopCap(float bottomRadisu, float topRadius, float height, uint32 sliceCount, uint32 stackCount, MeshData & meshData)
{
	uint32 baseIndex = (uint32)meshData.Vertices.size();

	float y = 0.5f * height;
	float dTheta = 2.0f * PI / sliceCount;

	for (uint32 i = 0; i < sliceCount; ++i)
	{
		float x = topRadius * cosf(i * dTheta);
		float z = topRadius * sinf(i * dTheta);

		float u = x / height + 0.5f;
		float v = z / height + 0.5f;

		meshData.Vertices.push_back(GeometryGenerator::Vertex(x, y, z, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, u, v));
	}

	meshData.Vertices.push_back(GeometryGenerator::Vertex(0.0f, y, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f));

	uint32 centerIndex = (uint32)meshData.Vertices.size() - 1;

	for (uint32 i = 0; i < sliceCount; ++i)
	{
		meshData.Indices32.push_back(centerIndex);
		meshData.Indices32.push_back(baseIndex + i + 1);
		meshData.Indices32.push_back(baseIndex + i);
	}
}

void GeometryGenerator::BuildCylinderBottomCap(float bottomRadisu, float topRadius, float height, uint32 sliceCount, uint32 stackCount, MeshData & meshData)
{
	uint32 baseIndex = (uint32)meshData.Vertices.size();

	float y = -0.5f * height;
	float dTheta = 2.0f * PI / sliceCount;

	for (uint32 i = 0; i < sliceCount; ++i)
	{
		float x = topRadius * cosf(i * dTheta);
		float z = topRadius * sinf(i * dTheta);

		float u = x / height + 0.5f;
		float v = z / height + 0.5f;

		meshData.Vertices.push_back(GeometryGenerator::Vertex(x, y, z, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, u, v));
	}

	meshData.Vertices.push_back(GeometryGenerator::Vertex(0.0f, y, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f));

	uint32 centerIndex = (uint32)meshData.Vertices.size() - 1;

	for (uint32 i = 0; i < sliceCount; ++i)
	{
		meshData.Indices32.push_back(centerIndex);
		meshData.Indices32.push_back(baseIndex + i + 1);
		meshData.Indices32.push_back(baseIndex + i);
	}
}
