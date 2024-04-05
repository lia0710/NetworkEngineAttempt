#include "EngineCore.h"
#include "FontAsset.h"
#include "RenderSystem.h"

IMPLEMENT_DYNAMIC_CLASS(FontAsset)

void FontAsset::Initialize()
{
	Asset::Initialize();

	TTF_Init();
	font = TTF_OpenFont(filepath.c_str(), font_size);
}

void FontAsset::Destroy()
{
	Asset::Destroy();

	TTF_CloseFont(font);
	font = nullptr;
	TTF_Quit();
}

void FontAsset::Load(json::JSON& node)
{
	Asset::Load(node);
	
	if (node.hasKey("FontSize"))
	{
		font_size = node.at("FontSize").ToInt();
	}
}

void FontAsset::SetFontSize(int size) const
{
	if (font == nullptr)
	{
		LOG("ERROR: Font not set");
		return;
	}
	TTF_SetFontSize(font, size);
};
