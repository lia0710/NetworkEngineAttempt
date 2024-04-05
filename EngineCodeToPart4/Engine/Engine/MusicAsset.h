#pragma once

#ifndef _MUSIC_ASSET_H_
#define _MUSIC_ASSET_H_

#include "Asset.h"
#include "SDL_mixer.h"

class MusicAsset final : public Asset 
{
    DECLARE_DYNAMIC_DERIVED_CLASS(MusicAsset, Asset)

public:
    Mix_Music* GetMusic() { return music; }

protected:
    void Load(json::JSON&) override;
    void Destroy() override;

private:
    Mix_Music* music;
};

#endif // !_MUSIC_ASSET_H_
