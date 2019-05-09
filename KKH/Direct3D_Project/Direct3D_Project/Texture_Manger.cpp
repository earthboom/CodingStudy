#include "stdafx.h"
#include "Texture_Manger.h"
#include "Graphic_Manager.h"
#include "DDSTextureLoader.h"
#include "Function.h"

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
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(GRAPHIC->Get_Device().Get(),
		GRAPHIC->Get_CommandList().Get(), ResTex->Filename.c_str(),
		ResTex->Resource, ResTex->UploadHeap));
	
	mTexture[ResTex->Name] = std::move(ResTex);
	return TRUE;
}
