#ifndef ENUM_H
#define ENUM_H


enum class RenderLayer : int
{
	Opaque = 0,
	Mirrors,
	Reflected,
	Transparent,
	Shadow,
	Count
};

#endif