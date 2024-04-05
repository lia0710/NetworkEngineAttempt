#pragma once

#ifndef _RENDERSYSTEM_H_
#define _RENDERSYSTEM_H_

#include <iostream>
#include <list>
#include "SDL.h"

class Renderable;

class RenderSystem
{
	DECLARE_SINGLETON(RenderSystem)

public:
	SDL_Window& GetWindow();
	SDL_Renderer& GetRenderer();

	void AddRenderable(Renderable* renderable);
	void RemoveRenderable(Renderable* renderable);
	void WindowBackgroundColor(int r, int g, int b, int a);
	void WindowSize(int width, int height);
	IVec2 GetWindowSize() const;

protected:
	void Load();
	void Initialize();
	void Update();
	void Destroy();

private:
	std::string _name = "DEFAULT NAME";
	int _width = 1280;
	unsigned int _height = 720;
	bool _fullScreen = false;
	SDL_Color _backgroundColor = { 0,0,0,255 };
	std::list<Renderable*> _renderables;
	SDL_Window* _window = nullptr;
	SDL_Renderer* _renderer = nullptr;

	friend class Engine;
};

#endif