#include "EngineCore.h"
#include "TextureAsset.h"
#include "RenderSystem.h"

IMPLEMENT_DYNAMIC_CLASS(TextureAsset);

void TextureAsset::Initialize()
{
	Asset::Initialize();

	SDL_Surface* image = IMG_Load(filepath.c_str());
	texture = SDL_CreateTextureFromSurface(&RenderSystem::Instance().GetRenderer(), image);
	printf("%s\n", SDL_GetError()); //Returns "Invalid Renderer".
	SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
	SDL_FreeSurface(image);
}

void TextureAsset::Destroy()
{
	Asset::Destroy();

	SDL_DestroyTexture(texture);
	texture = nullptr;
}
