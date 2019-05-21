#ifndef Struct_h__
#define Struct_h__

#include "MathHelper.h"

struct ObjectConstants
{
	DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();
};

#endif // Struct_h__