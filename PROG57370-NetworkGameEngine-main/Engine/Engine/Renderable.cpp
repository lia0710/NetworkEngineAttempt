#include "EngineCore.h"
#include "Renderable.h"
#include "RenderSystem.h"

Renderable::Renderable()
{
	RenderSystem::Instance().AddRenderable(this);
}

Renderable::~Renderable()
{
	RenderSystem::Instance().RemoveRenderable(this);
}

void Renderable::SetFilterColor(int r, int g, int b, int a)
{
	filterColor.r = static_cast<Uint8>(r);
	filterColor.g = static_cast<Uint8>(g);
	filterColor.b = static_cast<Uint8>(b);
	filterColor.a = static_cast<Uint8>(a);
}
