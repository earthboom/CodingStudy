#pragma once

#include "Object.h"
#include "Struct.h"

class LitColumn : public Object
{
public:
	explicit LitColumn(void);
	LitColumn(const LitColumn&) = delete;
	LitColumn& operator=(const LitColumn&) = delete;
	~LitColumn(void);

public:
	virtual bool Ready(void);
	virtual bool Update(const float& dt);
	virtual bool Render(const float& dt);
	
protected:
	void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems);
	virtual void BuildDescriptorHeaps(void) {}

	virtual void AnimateMaterials(const float& dt);
	virtual void UpdateObjectCBs(const float& dt);
	virtual void UpdateMaterialCBs(const float& dt);
	virtual void UpdateMainPassCB(const float& dt);

private:
	void BuildShapeGeometry(void);
	void BuildSkullGeometry(void);
	void BuildMaterials(void);
	void BuildRenderItem(void);

public:
	static std::shared_ptr<LitColumn> Create(std::string _name, Object::COM_TYPE _type);
};

typedef std::shared_ptr<LitColumn> LIT;