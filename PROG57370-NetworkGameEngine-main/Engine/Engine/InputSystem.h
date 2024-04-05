#pragma once
#include "Engine.h"
#include <SDL_keycode.h>
#include <SDL.h>

class InputSystem
{
	DECLARE_SINGLETON(InputSystem)

public:

	void Initialize();
	void Update();

	void TriggerKeyEvent(SDL_Keycode key, bool pressed);

	void TriggerMouseEvent(Uint8 button, bool pressed);

	bool IsKeyPressed(SDL_Keycode key) const;
	bool AreKeysPressed(const std::vector<SDL_Keycode>& keys) const;
	bool IsMouseButtonPressed(Uint8 button) const;

	bool IsGamepadButtonPressed(SDL_JoystickID joystickID, SDL_GameControllerButton button) const;

	void HandleGamepadAxis(SDL_JoystickID joystickID, SDL_GameControllerAxis axis, Sint16 value);

	float GetGamepadAxisState(SDL_JoystickID joystickID, SDL_GameControllerAxis axis) const;

	void RegisterQuitEventHandler(std::function<void()> handler);

	void HandleQuitEvent();

	void RegisterKeyEventHandler(SDL_Keycode key, bool onPress, std::function<void()> handler);
	void HandleGamepadConnection(int joystickIndex);
	void RegisterMouseEventHandler(Uint8 button, bool onPress, std::function<void()> handler);

	void InitializeGamepads();

	void HandleGamepadButton(SDL_JoystickID joystickID, SDL_GameControllerButton button, bool pressed);
	void RegisterGamepadButtonEventHandler(SDL_JoystickID joystickID, SDL_GameControllerButton button, std::function<void(bool)> handler);

private:
	bool  keyStates[SDL_NUM_SCANCODES] = { false };
	bool mouseButtonStates[5];
	int gamepadId;

	std::map<SDL_Keycode, std::function<void()>> keyPressHandlers;
	std::map<SDL_Keycode, std::function<void()>> keyReleaseHandlers;
	std::map<Uint8, std::function<void()>> mousePressHandlers;
	std::map<Uint8, std::function<void()>> mouseReleaseHandlers;
	std::map<SDL_JoystickID, std::map<SDL_GameControllerButton, bool>> gamepadButtonStates;
	std::map<SDL_JoystickID, std::map<SDL_GameControllerButton, std::function<void(bool)>>> gamepadButtonEventHandlers;
	std::map<SDL_JoystickID, std::map<SDL_GameControllerAxis, Sint16>> gamepadAxisStates;
	std::map<int, SDL_GameController*> gamepadMap;
	std::map<SDL_JoystickID, std::map<SDL_GameControllerAxis, std::function<void(Sint16)>>> gamepadAxisEventHandlers;

	std::function<void()> quitEventHandler;

	friend class Engine;

};


