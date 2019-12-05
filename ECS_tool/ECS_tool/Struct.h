#pragma once

using namespace DirectX;

namespace ECS_tool
{
	struct RenderItem
	{
		RenderItem(void) = default;
		RenderItem(const RenderItem&) = delete;

		D3D12_VERTEX_BUFFER_VIEW	VB_View;
		D3D12_INDEX_BUFFER_VIEW		IB_View;
	};

	struct BlendingIndexWeightPair
	{
		int BlendingIndex;
		double BlendingWeight;

		BlendingIndexWeightPair(void)
			: BlendingIndex(-1)
			, BlendingWeight(0.0f) {}
	};

	struct ControlPoint
	{
		XMFLOAT3 vPos;
		std::vector<BlendingIndexWeightPair*>* Weights;

		ControlPoint(void)
		{
			Weights = new std::vector<BlendingIndexWeightPair*>();
		}
	};

	typedef struct VertexBone
	{
		XMFLOAT3	vPos;
		XMFLOAT3	vNormal;
		XMFLOAT2	vTexUV;
		XMFLOAT3	vBinormal;
		XMFLOAT3	vTangent;

		std::vector<BlendingIndexWeightPair*>* Weights;

		VertexBone(void)
		{
			vPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			vNormal = XMFLOAT3(0.0f, 0.0f, 0.0f);
			vTexUV = XMFLOAT2(0.0f, 0.0f);
			vBinormal = XMFLOAT3(0.0f, 0.0f, 0.0f);
			vTangent = XMFLOAT3(0.0f, 0.0f, 0.0f);

			Weights = new std::vector<BlendingIndexWeightPair*>();
		}

		VertexBone(const XMFLOAT3& _pos, const XMFLOAT3& _normal, const XMFLOAT2& _uv, const XMFLOAT3& _binormal, const XMFLOAT3& _tangent)
		{
			vPos = _pos;
			vNormal = _normal;
			vTexUV = _uv;
			vBinormal = _binormal;
			vTangent = _tangent;
		}

		BOOL operator==(const VertexBone& _vb) const
		{
			if (!(vPos.x == _vb.vPos.x) && !(vPos.y == _vb.vPos.y) && !(vPos.z == _vb.vPos.z))
				return FALSE;

			if (!(vNormal.x == _vb.vNormal.x) && !(vNormal.y == _vb.vNormal.y) && !(vNormal.z == _vb.vNormal.z))
				return FALSE;

			if (!(vTexUV.x == _vb.vTexUV.x) && !(vTexUV.y == _vb.vTexUV.y))
				return FALSE;

			if (!(vBinormal.x == _vb.vBinormal.x) && !(vBinormal.y == _vb.vBinormal.y) && !(vBinormal.z == _vb.vBinormal.z))
				return FALSE;

			if (!(vTangent.x == _vb.vTangent.x) && !(vTangent.y == _vb.vTangent.y) && !(vTangent.z == _vb.vTangent.z))
				return FALSE;

			return TRUE;
		}

		size_t operator()(const VertexBone& _vb) const
		{
			//return std::hash<VertexBone>()(_vb);
			return sizeof(_vb);
		}
	}VTXBONE;

	struct Material
	{
		std::string* NodeName;
		std::string* MaterialName;
		std::string* TextureName;
		int TriangleCount;
		int OffsetIndex;
	};

	struct Mesh
	{
		std::string Name;
		Material* Material;
	};

	enum class NodeType
	{
		eNullNode = FbxNodeAttribute::eNull,
		eSkeletonNode = FbxNodeAttribute::eSkeleton,
		eMeshNode = FbxNodeAttribute::eMesh,
	};

	struct Node
	{
		int Index;
		int ParentIndex;
		NodeType Type;
		std::string* Name;
		XMMATRIX GlobalBindposeInverse;
		XMFLOAT3 LclTranslate;
		XMFLOAT3 LclRotation;
		XMFLOAT3 LclScaling;
		std::map<int, XMFLOAT3>* LclTransFrames;
		std::map<int, XMFLOAT3>* LclRotFrames;
		std::map<int, XMFLOAT3>* LclScalingFrames;

		Node(void)
		{
			LclTransFrames		= new std::map<int, XMFLOAT3>();
			LclRotFrames		= new std::map<int, XMFLOAT3>();
			LclScalingFrames	= new std::map<int, XMFLOAT3>();
		}
	};
}