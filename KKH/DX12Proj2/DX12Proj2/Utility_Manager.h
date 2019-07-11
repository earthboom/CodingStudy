#pragma once

#include "FrameResource.h"
//#include "ComputeWaves.h"
#include "Object.h"
#include "BlurFilter.h"

#define UTIL Utility_Manager::GetInstnace()

class Utility_Manager : public CSingleton<Utility_Manager>
{
public:
	enum ObjState { OS_UPDATE, OS_RENDER, OS_END };

public:
	explicit Utility_Manager(void);
	Utility_Manager(const Utility_Manager&) = delete;
	Utility_Manager& operator=(const Utility_Manager&) = delete;
	~Utility_Manager(void);

public:
	void UtilityInitialize(void);
	void BuildRootSignature(void);
	void BuildPostProcessRootSignature(void);
	void BuildDescriptorHeaps(void);
	void BuildShadersAndInputLayer(void);

	void UtilityDecriptor(void);
	void BuildFrameResources(void);
	void BuildPSOs(void);


	void OnResize(void);

private:
	UINT mCbvSrvDescriptorSize;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> mPostProcessRootSignature;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap;

	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> mShaders;

	typedef std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> MAP_PSO;
	MAP_PSO mPSOs;

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;
	std::vector<D3D12_INPUT_ELEMENT_DESC> mTreeSpriteInputLayout;

	typedef std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> GEOMESH;
	GEOMESH	mGeometries;

	typedef std::unordered_map<std::string, std::unique_ptr<Material>> MATERIAL;
	MATERIAL mMaterials;

	//typedef std::unordered_map<std::string, std::unique_ptr<Texture>> TEXTURE;
	//TEXTURE mTextures;

	typedef std::vector<std::unique_ptr<RenderItem>> RITEMVEC;
	RITEMVEC mAllRitem;

	typedef std::vector<std::unique_ptr<FrameResource>> FRAMERES;
	FRAMERES mFrameResources;
	FrameResource* mCurrFrameResource;
	int mCurrFrameResourceIndex;

	PassConstants mMainPassCB;
	PassConstants mReflectedPassCB;

	std::vector<RenderItem*> mDrawLayer[(int)DrawLayer::DL_END];

	//std::unique_ptr<ComputeWaves> mCPWave;

	std::unique_ptr<BlurFilter> mBlurFilter;

private:
	typedef std::map<std::string, OBJECT> OBJMAP;
	OBJMAP* Obj_static_map;
	OBJMAP* Obj_dynamic_map;	

	typedef std::vector<OBJMAP*> ALLOBJVEC;
	ALLOBJVEC allObj_Update_vec;

private:
	ObjState	mCurrState;

	DirectX::XMFLOAT4X4 mView;

public:
	bool Object_Create(OBJECT& obj);
	bool Object_Ready(void);
	bool Object_Cycle(const CTimer& mt, ObjState _state = OS_END);

	OBJECT Get_Object(std::string _key, Object::COM_TYPE _type);

private:
	bool Object_Update(const CTimer& mt);
	bool Object_Render(const CTimer& mt);//, OBJMAP& _objmap);

	void UpdateObjectCBs(const CTimer& mt);
	void UpdateMaterialCBs(const CTimer& mt);
	void UpdateMainPassCB(const CTimer& mt);
	void UpdateReflectedPassCB(const CTimer& mt);

	void DrawRenderItems(ID3D12GraphicsCommandList * cmdList, const std::vector<RenderItem*>& ritems);

	void OnKeyboardInput(const CTimer& mt);
	void UpdateCamera(const CTimer& mtt);

public:
	ObjState& Get_CurrState(void) { return mCurrState; }

	Microsoft::WRL::ComPtr<ID3D12PipelineState>& Get_PSOs(std::string str){return mPSOs[str]; }
	Microsoft::WRL::ComPtr<ID3D12RootSignature>& Get_RootSignature(void){return mRootSignature; }
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& Get_SrvDiscriptorHeap(void){return mSrvDescriptorHeap; }

	GEOMESH&	Get_Geomesh(void){return mGeometries; }
	RITEMVEC&	Get_Ritemvec(void){return mAllRitem; }
	FRAMERES&	Get_Frameres(void){return mFrameResources; }
	MATERIAL&	Get_Materials(void){return mMaterials; }
	
	std::vector<RenderItem*>&Get_Drawlayer(int _type){return mDrawLayer[_type]; }

	PassConstants&Get_MainPassCB(void){return mMainPassCB; }

	FrameResource*& Get_CurrFrameResource(void){return mCurrFrameResource; }
	int& Get_CurrFrameResourceIndex(void){return mCurrFrameResourceIndex; }

	ALLOBJVEC& Get_Allobjvec(void){return allObj_Update_vec; }

	DirectX::XMFLOAT4X4& Get_ViewMat(void){return mView; }

	UINT& Get_CbvSrvDescriptorSize(void){return mCbvSrvDescriptorSize; }

	//std::function<std::unique_ptr<ComputeWaves>&()> Get_CPWave = [&]()->std::unique_ptr<ComputeWaves>& {return mCPWave; };
	//COMWAVE& Get_CPWave(void){return mCPWave; }
};