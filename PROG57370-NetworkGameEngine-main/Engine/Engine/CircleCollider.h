#pragma once
#ifndef _CIRCLECOLLIDER_H_
#define _CIRCLECOLLIDER_H_

#include"EngineCore.h"
#include"Collider.h"

class CircleCollider : public Collider
{
	DECLARE_DYNAMIC_DERIVED_CLASS(CircleCollider, Collider)

public:
	void SetRadius(float radius) { m_radius = radius; };
	float GetRadius() const { return m_radius; };
	float GetBroadPhaseRadius() const override { return m_radius; };
	bool HandleCollision(Collider* other) override { return true; };
	ColliderType GetType() const override { return ColliderType::Circle; };

private:
	float m_radius = 0;
};

#endif
