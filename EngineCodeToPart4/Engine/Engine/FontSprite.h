#pragma once

#ifndef _FONT_SPRITE_
#define _FONT_SPRITE_

#include "Renderable.h"
#include "SDL.h"

class FontAsset;

class FontSprite : public Renderable
{
	DECLARE_DYNAMIC_DERIVED_CLASS(FontSprite, Component)

public:
	void SetText(std::string text);
	void SetFont(FontAsset* font);
	void SetFontColor(int r, int g, int b, int a);

protected:
	void Initialize() override;
	void Destroy() override;
	void Render() override;

	void Save(json::JSON&) const;
	void Load(json::JSON&) override;
	void Update() override;

private:
	void RegenerateOutput();

private:
	IVec2 outputSizing = { 0, 0 };
	std::string text = "DEFAULT TEXT";
	FontAsset* font = nullptr;
	SDL_Rect fontRect = { 0,0,250,250 };
	SDL_Texture* output = nullptr;
	SDL_Color fontColor = { 255,255,255,255 };
};

#endif