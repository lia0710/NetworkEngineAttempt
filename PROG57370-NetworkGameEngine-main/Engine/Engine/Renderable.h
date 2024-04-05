#pragma once

#ifndef _RENDERABLE_H_
#define _RENDERABLE_H_

#include "Component.h"
#include "SDL.h"

class Renderable : public Component
{
protected:
	Renderable();
	~Renderable();

private:
	void SetFilterColor(int r, int g, int b, int a);
	virtual void Render() = 0;

protected:
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	SDL_Color filterColor = { 255, 255, 255 };

	friend class RenderSystem;
};

#endif

