#pragma once

#include "Object.h"

class LitColumn : public Object
{
public:
	explicit LitColumn(void);
	LitColumn(const LitColumn&) = delete;
	LitColumn& operator=(const LitColumn&) = delete;
	~LitColumn(void);
};