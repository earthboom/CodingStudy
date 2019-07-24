#pragma once

#define TEX Texture_Manger::GetInstnace()

typedef std::unordered_map<std::string, std::unique_ptr<Texture>> MAP_TEX;

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
	MAP_TEX mTexture;

public:
	//std::function<MAP_TEX&()> Get_Textures = [&]()->MAP_TEX& {return mTexture; };
	MAP_TEX& Get_Textures(void){return mTexture; }
};


