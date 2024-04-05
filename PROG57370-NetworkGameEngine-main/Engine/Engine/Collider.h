#pragma once

#ifndef _ICOLLIDER_H
#define _ICOLLIDER_H

#include "EngineCore.h"
#include "SDL.h"

enum class ColliderType 
{
	Box,
	Circle,
	// ... potentially more collider types
};

class Collider : public Component 
{
	DECLARE_ABSTRACT_DERIVED_CLASS(Collider, Component)

public:
	bool IsSolid() const { return isSolid; }
	void SetSolid(bool solid) { isSolid = solid; }

	virtual ColliderType GetType() const = 0;
	virtual float GetBroadPhaseRadius() const = 0;
	
	virtual std::list<Collider*> OnCollisionEnter();
	virtual std::list<Collider*> OnCollisionStay();
	virtual std::list<Collider*> OnCollisionExit();

	virtual bool HandleCollision(Collider*) = 0;

	void StorePosition(Vec2 position) { previousPosition = position; }
	Vec2 GetPosition() const;
	void ResetPosition() const;

protected:
	Collider();
	~Collider() override;
	void Initialize() override;

protected:
	int size = 0;
	bool isSolid = false;
	SDL_Rect* rect = nullptr;
	Vec2 previousPosition;

	friend class CollisionSystem;
};


#endif