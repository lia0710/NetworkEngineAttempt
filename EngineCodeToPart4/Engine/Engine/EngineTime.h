#pragma once

#ifndef _TIME_H_
#define _TIME_H_

#include <chrono>

class Time
{
	DECLARE_SINGLETON(Time)

public:
	float DeltaTime() { return deltaTime.count(); }
	float TotalTime() { return totalTime.count(); }
	unsigned int FrameCount() { return frameCount; }

private:
	void Initialize();
	void Update();
	void Destroy();

private:
	unsigned int frameCount = 0;
	std::chrono::duration<float> deltaTime = std::chrono::duration<float>(0.0f);
	std::chrono::duration<float> totalTime = std::chrono::duration<float>(0.0f);
	std::chrono::time_point<std::chrono::system_clock> beginTime;
	std::chrono::time_point<std::chrono::system_clock> endTime;

	friend class Engine;
};

#endif