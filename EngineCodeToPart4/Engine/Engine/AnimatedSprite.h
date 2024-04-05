#pragma once

#ifndef _ANIMATED_SPRITE_H_
#define _ANIMATED_SPRITE_H_

#include "Sprite.h"

class AnimatedSprite : public Sprite 
{
	DECLARE_DYNAMIC_DERIVED_CLASS(AnimatedSprite, Sprite)

protected:
	void Update() override;
	void Load(json::JSON&) override;

public:
	void SetSpriteSheet(int rows, int cols, int frames);
	void Restart();
	void Stop();
	void Play();
	void Pause();

	void DeserializeCreate(RakNet::BitStream& bitStream) override;

protected:
	int spriteSheetRows = 1;
	int spriteSheetColumns = 1;

	int spriteWidth = 0;
	int spriteHeight = 0;

	int totalFrames = 0;
	int defaultFrameNumber = 0;

	int currentFrame = 0;
	float frameDelay = 0.05f;
	float frameCounter = 0;

	bool running = true;
	bool loop = true;
};

#endif