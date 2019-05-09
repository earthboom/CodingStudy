#pragma once

#include "Unique_Singleton.h"
#include "d3dutil_Manager.h"
#include "FrameResource.h"

#include "Object.h"
#include "Struct.h"

#define UTIL Utility_Manager::GetInstnace()

class Utility_Manager : public CSingleton<Utility_Manager>
{
public:
	enum ObjState { OS_READY, OS_UPDATE, OS_RENDER, OS_END };

public:
	explicit Utility_Manager(void);
	Utility_Manager(const Utility_Manager&) = delete;
	Utility_Manager& operator=(const Utility_Manager&) = delete;
	~Utility_Manager(void);

public:
	void BuildRootSignature(void);
	void BuildShadersAndInputLayer(void);

	void BuildFrameResources(void);
	void BuildPSOs(void);


	void OnResize(void);

	void OnKeyboardInput(const float& dt);
	void UpdateCamera(const float& dt);

private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;

	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> mShaders;

	typedef std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> MAP_PSO;
	MAP_PSO mPSOs;

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

	typedef std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> GEOMESH;
	GEOMESH	mGeometries;

	typedef std::unordered_map<std::string, std::unique_ptr<Material>> MATERIAL;
	MATERIAL mMaterials;

	typedef std::unordered_map<std::string, std::unique_ptr<Texture>> TEXTURE;
	TEXTURE mTextures;

	typedef std::vector<std::unique_ptr<RenderItem>> RITEMVEC;
	RITEMVEC mAllRitem;

	typedef std::vector<std::unique_ptr<FrameResource>> FRAMERES;
	FRAMERES mFrameResources;
	FrameResource* mCurrFrameResource;
	int mCurrFrameResourceIndex;

	PassConstants mMainPassCB;

	std::vector<RenderItem*> mDrawLayer[(int)DrawLayer::DL_END];

private:
	typedef std::map<std::string, OBJECT> OBJMAP;
	OBJMAP* Obj_static_map;
	OBJMAP* Obj_dynamic_map;	

	typedef std::vector<OBJMAP*> ALLOBJVEC;
	ALLOBJVEC allObj_Update_vec;

private:
	DirectX::XMFLOAT4X4 mView;
	//DirectX::XMFLOAT4X4 mProj;

	//float mTheta;
	//float mPhi;
	//float mRadius;

public:
	bool Object_Create(OBJECT obj);
	bool Object_Cycle(const float& dt, ObjState _state);

	//bool Object_Ready(void);
	//bool Object_Update(const float& dt);
	//bool Object_Render(const float& dt);

public:
	std::function<Microsoft::WRL::ComPtr<ID3D12PipelineState>&(std::string)> Get_PSOs = [&](std::string str)->Microsoft::WRL::ComPtr<ID3D12PipelineState>& {return mPSOs[str]; };
	std::function<Microsoft::WRL::ComPtr<ID3D12RootSignature>&()> Get_RootSignature = [&]()->Microsoft::WRL::ComPtr<ID3D12RootSignature>& {return mRootSignature; };

	std::function<GEOMESH&()> Get_Geomesh = [&]()->GEOMESH& {return mGeometries; };
	std::function<RITEMVEC&()> Get_Ritemvec = [&]()->RITEMVEC& {return mAllRitem; };
	std::function<FRAMERES&()> Get_Frameres = [&]()->FRAMERES& {return mFrameResources; };
	std::function<MATERIAL&()> Get_Materials = [&]()->MATERIAL& {return mMaterials; };
	std::function<TEXTURE&()> Get_Textures = [&]()->TEXTURE& {return mTextures; };
	std::function<std::vector<RenderItem*>&(int)> Get_Drawlayer = [&](int _type)->std::vector<RenderItem*>& {return mDrawLayer[_type]; };

	std::function<PassConstants&()> Get_MainPassCB = [&]()->PassConstants& {return mMainPassCB; };

	std::function<FrameResource*&()> Get_CurrFrameResource = [&]()->FrameResource*& {return mCurrFrameResource; };
	std::function<int&()> Get_CurrFrameResourceIndex = [&]()->int& {return mCurrFrameResourceIndex; };

	std::function<ALLOBJVEC&()> Get_Allobjvec = [&]()->ALLOBJVEC& {return allObj_Update_vec; };

	std::function<DirectX::XMFLOAT4X4&()> Get_ViewMat = [&]()->DirectX::XMFLOAT4X4& {return mView; };

	//std::function<float&()> Get_Theta = [&]()->float& {return mTheta; };
	//std::function<float&()> Get_Phi = [&]()->float& {return mPhi; };
	//std::function<float&()> Get_Radius = [&]()->float& {return mRadius; };
};