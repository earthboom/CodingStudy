#ifndef ENUM_H
#define ENUM_H


enum class RenderLayer : int
{
	Opaque = 0,
	Transparent,
	AlphaTested,
	Count
};

#endif