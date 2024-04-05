#include "EngineCore.h"
#include "SoundAsset.h"

IMPLEMENT_DYNAMIC_CLASS(SoundAsset)

void SoundAsset::Destroy()
{
	Asset::Destroy();
	Mix_FreeChunk(sound);
	sound = nullptr;
}

void SoundAsset::Load(json::JSON& json)
{
	Asset::Load(json);
	sound = Mix_LoadWAV(filepath.c_str());
}