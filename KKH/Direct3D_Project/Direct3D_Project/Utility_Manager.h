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

private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;

	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> mShaders;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> mPSOs;

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>	mGeometries;
	typedef std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> GEOMESH;

	std::vector<std::unique_ptr<RenderItem>> mAllRitem;
	typedef std::vector<std::unique_ptr<RenderItem>> RITEMVEC;

	std::vector<RenderItem*> mDrawLayer[(int)DrawLayer::DL_END];

	std::vector<std::unique_ptr<FrameResource>> mFrameResources;
	typedef std::vector<std::unique_ptr<FrameResource>> FRAMERES;

private:
	std::map<std::string, OBJECT>* Obj_static_map;
	std::map<std::string, OBJECT>* Obj_dynamic_map;
	typedef std::map<std::string, OBJECT> OBJMAP;

	std::vector<OBJMAP*> allObj_Update_vec;
	typedef std::vector<OBJMAP*> ALLOBJVEC;

public:
	bool Object_Create(OBJECT obj);
	bool Object_Cycle(const float& dt, ObjState _state);



	//bool Object_Ready(void);
	//bool Object_Update(const float& dt);
	//bool Object_Render(const float& dt);

public:
	std::function<GEOMESH&()> Get_Geomesh = [&]()->GEOMESH& {return mGeometries; };
	std::function<RITEMVEC&()> Get_Ritemvec = [&]()->RITEMVEC& {return mAllRitem; };
	std::function<FRAMERES&()> Get_Frameres = [&]()->FRAMERES& {return mFrameResources; };
	std::function<std::vector<RenderItem*>&(int)> Get_Drawlayer = [&](int _type)->std::vector<RenderItem*>& {return mDrawLayer[_type]; };
};