#include "pch.h"
#include "Character.h"

Character::Character(void)
	: Object()
{
}

Character::~Character(void)
{
}

bool Character::Initialize(void)
{
	return TRUE;
}

bool Character::Update(DX::StepTimer const& timer)
{
	return TRUE;
}

bool Character::Render(DX::StepTimer const& timer)
{
	return TRUE;
}
