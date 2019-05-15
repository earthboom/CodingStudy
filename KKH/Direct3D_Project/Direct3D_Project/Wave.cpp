#include "stdafx.h"
#include "Wave.h"

Wave::Wave(void)
{
}

Wave::~Wave(void)
{
}

bool Wave::Ready(void)
{
	return false;
}

bool Wave::Update(const float & dt)
{
	return false;
}

bool Wave::Render(const float & dt)
{
	return false;
}

void Wave::BuildDescriptorHeaps(void)
{
}

void Wave::BuildMaterials(void)
{
}

void Wave::BuildRenderItem(void)
{
}

void Wave::BuildGeometry(void)
{
}

std::shared_ptr<Wave> Wave::Create(Object::COM_TYPE _type, std::string _name, std::string _submeshname, std::string _texname, std::string _matname)
{
	return std::shared_ptr<Wave>();
}
