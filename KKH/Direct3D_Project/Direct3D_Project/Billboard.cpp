#include "stdafx.h"
#include "Billboard.h"

Billboard::Billboard(void)
{
}

Billboard::Billboard(Object::COM_TYPE _type, std::string _name, std::string _submeshname, std::string _texname, std::string _matname)
{
}

Billboard::~Billboard(void)
{
}

bool Billboard::Ready(void)
{
	BuildDescriptorHeaps();
	BuildGeometry();
	BuildMaterials();
	BuildRenderItem();

	return TRUE;
}

bool Billboard::Update(const CTimer& mt)
{
	return TRUE;
}

bool Billboard::Render(const CTimer& mt)
{
	return TRUE;
}

void Billboard::BuildDescriptorHeaps(void)
{
}

void Billboard::BuildMaterials(void)
{
}

void Billboard::BuildRenderItem(void)
{
}

void Billboard::BuildGeometry(void)
{
}

std::shared_ptr<Billboard> Billboard::Create(Object::COM_TYPE _type, std::string _name, std::string _submeshname, std::string _texname, std::string _matname)
{
	BILLBOARD pBillboard = std::make_shared<Billboard>(_type, _name, _submeshname, _texname, _matname);

	if (!pBillboard) return nullptr;

	return pBillboard; 
}
