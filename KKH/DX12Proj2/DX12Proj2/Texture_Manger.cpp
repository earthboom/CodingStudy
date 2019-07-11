#include "pch.h"
#include "Texture_Manger.h"
#include "DDSTextureLoader.h"

Texture_Manger::Texture_Manger(void)
{
}

Texture_Manger::~Texture_Manger(void)
{
}

bool Texture_Manger::onDDSLoad(std::string _key, std::wstring _fileadress)
{
	if (mTexture.find(_key) != mTexture.end())
		return FALSE;

	auto ResTex = std::make_unique<Texture>();
	ResTex->Name = _key;
	ResTex->Filename = _fileadress;
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(DEVICE.Get(),
		COM_LIST.Get(), ResTex->Filename.c_str(),
		ResTex->Resource, ResTex->UploadHeap));
	
	mTexture[ResTex->Name] = std::move(ResTex);
	return TRUE;
}
