#include "EngineCore.h"
#include "MusicAsset.h"

IMPLEMENT_DYNAMIC_CLASS(MusicAsset)

void MusicAsset::Destroy()
{
	Asset::Destroy();

	Mix_FreeMusic(music);
	music = nullptr;
}

void MusicAsset::Load(json::JSON& json)
{
	Asset::Load(json);
	music = Mix_LoadMUS(filepath.c_str());
};
