#pragma once

struct SubmeshGeometry
{
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	INT	 BaseVertexLocation = 0;

	//Bounding box of the geometry deinfed by this submesh.
	DirectX::BoundingBox Bounds;
};

struct MeshGeometry
{
	std::string Name;	//Give it a name so we can look it up by name.

	//System memory copies.
	//Use Blobs because the vertex / index format can be generic.
	//It is up to the client to cast appropriately.
	Microsoft::WRL::ComPtr<ID3DBlob> VertexBufferCPU = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> IndexBufferCPU = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferGPU = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferUploader = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferUploader = nullptr;

	//Data about the buffer.
	UINT VertexByteStride = 0;
	UINT VertexBufferByteSize = 0;
	DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
	UINT IndexBufferByteSize = 0;

	//A MeshGeometry mat store multiple geometires in one vertex / index buffer.
	//Use this container to define the Submesh geometries so we can draw the submeshes individually.
	std::unordered_map<std::string, SubmeshGeometry> DrawArgs;

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView(void) const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
		vbv.StrideInBytes = VertexByteStride;
		vbv.SizeInBytes = VertexBufferByteSize;

		return vbv;
	}

	D3D12_INDEX_BUFFER_VIEW IndexBufferView(void) const
	{
		D3D12_INDEX_BUFFER_VIEW ibv;
		ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
		ibv.Format = IndexFormat;
		ibv.SizeInBytes = IndexBufferByteSize;

		return ibv;
	}

	void DisposeUploaders(void)
	{
		VertexBufferUploader = nullptr;
		IndexBufferUploader = nullptr;
	}
};

struct Light
{
	DirectX::XMFLOAT3 Strength = { 0.5f, 0.5f, 0.5f };
	float FalloffStart = 1.0f;
	DirectX::XMFLOAT3 Direction = { 0.0f, -1.0f, 0.0f };
	float FalloffEnd = 10.0f;
	DirectX::XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f };
	float SpotPower = 64.0f;
};

struct MaterialConstants
{
	DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
	float Roughness = 0.25f;
	DirectX::XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();
};

typedef struct Material
{
	std::string Name = "";

	int MatCBIndex = -1;
	int DiffuseSrvHeapIndex = -1;
	int NormalSrvHeapIndex = -1;
	int NumFrameDirty = NumFrameResources;

	DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
	float Roughness = 0.25f;
	DirectX::XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();
}MATERIAL;

struct MaterialData
{
	XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
	XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
	float Roughness = 64.0f;

	//using Texture mapping
	XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();

	UINT DiffuseMapIndex = 0;
	UINT MaterialPad0;
	UINT MaterialPad1;
	UINT MaterialPad2;
};

struct InstanceData
{
	XMFLOAT4X4 World = MathHelper::Identity4x4();
	XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();
	UINT MaterialIndex = 0;
	UINT InstancePad0 = 0;
	UINT InstancePad1 = 0;
	UINT InstancePad2 = 0;

	InstanceData(void)
		: World(MathHelper::Identity4x4()), TexTransform(MathHelper::Identity4x4())
		, MaterialIndex(0), InstancePad0(0), InstancePad1(0), InstancePad2(0) {}

	void Init(void)
	{
		World = MathHelper::Identity4x4();
		TexTransform = MathHelper::Identity4x4();
		MaterialIndex = 0;
		InstancePad0 = 0;
		InstancePad1 = 0;
		InstancePad2 = 0;
	}

	void operator=(InstanceData& _data)
	{
		World = _data.World;
		TexTransform = _data.TexTransform;
		MaterialIndex = _data.MaterialIndex;
		InstancePad0 = _data.InstancePad0;
		InstancePad1 = _data.InstancePad1;
		InstancePad2 = _data.InstancePad2;
	}
};

struct RenderItem
{
	RenderItem(void) = default;
	RenderItem(const RenderItem&) = delete;

	bool Visible = TRUE;

	DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();

	DirectX::XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();

	int NumFramesDirty = NumFrameResources;
	
	UINT objCBIndex = -1;

	Material* Mat = nullptr;
	MeshGeometry* Geo = nullptr;

	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	BoundingBox Bounds;
	std::vector<InstanceData> Instances;

	UINT IndexCount = 0;
	UINT InstanceCount = 0;
	UINT StartIndexLocation = 0;
	int BaseVertexLocation = 0;
};

struct Texture
{
	std::string Name;
	std::wstring Filename;

	UINT srvHeapCount = -1;
	UINT matCBCount = -1;
	bool bRegister = FALSE;

	Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;
};

enum class TimerType
{
	TIMER_MAIN,
	TIMER_END
};

struct ObjectConstants
{
	//DirectX::XMFLOAT4X4	WorldViewPorj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4	World = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();
	UINT	Materialndex;
	UINT	ObjPad0;
	UINT	ObjPad1;
	UINT	ObjPad2;
};

struct PassConstants
{
	DirectX::XMFLOAT4X4 View = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 InvView = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 Proj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 InvProj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 ViewProj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 InvViewProj = MathHelper::Identity4x4();

	DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };

	float cbPerObjectPad1 = 0.0f;

	DirectX::XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
	DirectX::XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };

	float NearZ = 0.0f;
	float FarZ = 0.0f;
	float TotalTime = 0.0f;
	float DeltaTime = 0.0f;

	DirectX::XMFLOAT4 AmbientLight = { 0.0f, 0.0f, 0.0f, 1.0f };

	//DirectX::XMFLOAT4 FogColor = { 0.7f, 0.7f, 0.7f, 1.0f };
	//float gFogStart = 5.0f;
	//float gFogRange = 150.0f;
	//DirectX::XMFLOAT2 cbPerObjectPad2;

	Light Lights[MaxLights];
};

typedef struct _Vertex
{
	_Vertex(void) {};
	_Vertex(float x, float y, float z, float nx, float ny, float nz, float u, float v)
		: Pos(x, y, z), Normal(nx, ny, nz), TexC(u, v) {}

	void operator()(float x, float y, float z, float nx, float ny, float nz, float u, float v)
	{
		Pos = DirectX::XMFLOAT3(x, y, z);
		Normal = DirectX::XMFLOAT3(nx, ny, nz);
		TexC = DirectX::XMFLOAT2(u, v);
	}

	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 TexC;
} VERTEX;