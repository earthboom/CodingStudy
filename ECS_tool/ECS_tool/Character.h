#pragma once

#include "Object.h"

class Character : public Object
{
public:
	Character(void);
	Character(const Character&) = delete;
	Character& operator=(const Character&) = delete;
	virtual ~Character(void);

public:
	virtual bool Initialize(void);
	virtual bool Update(DX::StepTimer const& timer);
	virtual bool Render(DX::StepTimer const& timer);
};

typedef std::shared_ptr<Character>	PCHARACTER;