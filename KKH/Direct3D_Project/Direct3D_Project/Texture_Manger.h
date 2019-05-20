#pragma once

#include "Unique_Singleton.h"
#include "Struct.h"

#define TEX Texture_Manger::GetInstnace()

class Texture_Manger : public CSingleton<Texture_Manger>
{
public:
	explicit Texture_Manger(void);
	Texture_Manger(const Texture_Manger&) = delete;
	Texture_Manger& operator=(const Texture_Manger&) = delete;
	~Texture_Manger(void);

public:
	bool onDDSLoad(std::string _key, std::wstring _fileadress);

private:
	typedef std::unordered_map<std::string, std::unique_ptr<Texture>> MAP_TEX;
	MAP_TEX mTexture;

public:
	//std::function<MAP_TEX&()> Get_Textures = [&]()->MAP_TEX& {return mTexture; };
	MAP_TEX& Get_Textures(void){return mTexture; }
};


