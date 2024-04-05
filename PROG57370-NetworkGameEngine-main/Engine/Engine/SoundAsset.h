#pragma once

#ifndef _SOUND_ASSET_H_
#define _SOUND_ASSET_H_

#include "Asset.h"
#include "SDL_mixer.h"

class SoundAsset final : public Asset
{
    DECLARE_DYNAMIC_DERIVED_CLASS(SoundAsset, Asset)

public:
    Mix_Chunk* GetSound() const { return sound; };

protected:
    void Destroy() override;
    void Load(json::JSON&) override;

private:
    Mix_Chunk* sound = nullptr;
};

#endif // !_SOUND_ASSET_H_
