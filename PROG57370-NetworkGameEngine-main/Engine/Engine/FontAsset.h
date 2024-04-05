#pragma once
#ifndef _FONT_ASSET_H_
#define _FONT_ASSET_H_

#include "Asset.h"
#include "SDL_ttf.h"

class FontAsset final : public Asset
{
    DECLARE_DYNAMIC_DERIVED_CLASS(FontAsset, Asset)

public:
    void SetFontSize(int size) const;
    TTF_Font* GetFont() const { return font; };

protected:
    void Initialize() override;
    void Destroy() override;
    void Load(json::JSON&) override;

private:
    TTF_Font* font = nullptr;
    int font_size = 12;
};

#endif // !_FONT_ASSET_H_
