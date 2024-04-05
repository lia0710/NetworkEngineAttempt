#include "EngineCore.h"
#include "RenderSystem.h"
#include "Renderable.h"
#include "Scene.h"

void RenderSystem::Initialize()
{
	//Pulls the window information from the RenderSettings file located in Assets
	std::ifstream inputStream("../Assets/RenderSettings.json");
	std::string str((std::istreambuf_iterator<char>(inputStream)), std::istreambuf_iterator<char>());
	json::JSON document = json::JSON::Load(str);

	//Checks for name in RenderSettings
	if (document.hasKey("name"))
	{
		_name = document["name"].ToString();
	}
	else
	{
		std::cout << "Name wasn't found in RenderSettings. Going with the Default instead.";
	}

	//Checks for width in RenderSettings
	if (document.hasKey("width"))
	{
		_width = document["width"].ToInt();
	}
	else
	{
		std::cout << "Width wasn't found in RenderSettings. Going with the Default instead.";
	}

	//Checks for height in RenderSettings
	if (document.hasKey("height"))
	{
		_height = document["height"].ToInt();
	}
	else
	{
		std::cout << "Height wasn't found in RenderSettings. Going with the Default instead.";
	}

	//Checks for fullscreen in RenderSettings
	if (document.hasKey("fullscreen"))
	{
		_fullScreen = document["fullScreen"].ToBool();
	}
	else
	{
		std::cout << "Fullscreen wasn't found in RenderSettings. Going with the Default instead.";
	}

	_window = SDL_CreateWindow(_name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, _width, _height, _fullScreen);
	_renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
}

void RenderSystem::Destroy()
{
	SDL_DestroyWindow(_window);
	SDL_DestroyRenderer(_renderer);
}

void RenderSystem::Update()
{
	SDL_RenderClear(_renderer);
	SDL_SetRenderDrawColor(_renderer, _backgroundColor.r, _backgroundColor.g, _backgroundColor.b, _backgroundColor.a);

	for (auto renderable : _renderables)
	{
		if (!renderable->owner->GetParentScene()->IsEnabled())
		{
			continue;
		}
		renderable->Render();
	}

	SDL_RenderPresent(_renderer);
}

void RenderSystem::Load()
{

}

SDL_Window& RenderSystem::GetWindow()
{
	return *_window;
}

SDL_Renderer& RenderSystem::GetRenderer()
{
	return *_renderer;
}

void RenderSystem::AddRenderable(Renderable* renderable)
{
	_renderables.push_back(renderable);
}

void RenderSystem::RemoveRenderable(Renderable* renderable)
{
	_renderables.remove(renderable);
}

void RenderSystem::WindowBackgroundColor(int r, int g, int b, int a)
{
	_backgroundColor.r = r;
	_backgroundColor.g = g;
	_backgroundColor.b = b;
	_backgroundColor.a = a;
}

void RenderSystem::WindowSize(int width, int height)
{
	if (!_fullScreen)
	{
		_width = width;
		_height = height;
		SDL_SetWindowSize(_window, _width, _height);
	}
	else
	{
		std::cout << "Can't resize a fullscreen window. Change the RenderSettings if you want to use this method." << std::endl;
	}
}

IVec2 RenderSystem::GetWindowSize() const
{
	return {static_cast<int>(_width), static_cast<int>(_height)};
}
