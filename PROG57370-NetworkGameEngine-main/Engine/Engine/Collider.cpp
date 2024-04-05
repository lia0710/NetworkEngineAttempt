#include "EngineCore.h"
#include "Collider.h"
#include "CollisionSystem.h"
#include "Sprite.h"
#include "AnimatedSprite.h"

IMPLEMENT_ABSTRACT_CLASS(Collider);

Collider::Collider() 
{
	CollisionSystem::Instance().AddCollider(this);
}

Collider::~Collider() 
{
	CollisionSystem::Instance().RemoveCollider(this);
}

void Collider::Initialize()
{
	Component::Initialize();
	if (owner->HasComponent("Sprite"))
	{
		// TODO: unsafe if our Sprite Component is destroyed.
		rect = &((Sprite*)(owner->GetComponent("Sprite")))->GetTargetRect();
	}
	else if (owner->HasComponent("AnimatedSprite"))
	{
		// TODO: unsafe if our AnimatedSprite Component is destroyed.
		rect = &((AnimatedSprite*)(owner->GetComponent("AnimatedSprite")))->GetTargetRect();
	}
}

Vec2 Collider::GetPosition() const
{
	return owner->GetTransform().position;
}

void Collider::ResetPosition() const
{
	owner->GetTransform().position = previousPosition;
}

std::list<Collider*> Collider::OnCollisionEnter() 
{
	std::list<Collider*> result;
	for (const auto& [first, second] : CollisionSystem::Instance().enterCollisions) 
	{
		// Skip checking for collisions if both Colliders are the same
		if (first->GetUid() == second->GetUid()) 
		{
			continue;
		}

		if (first->GetUid() == GetUid()) 
		{
			result.push_back(second);
		} 
		else if (second->GetUid() == GetUid()) 
		{
			result.push_back(first);
		}
	}
	return result;
}

// Called when the collider stays in collision
std::list<Collider*> Collider::OnCollisionStay() 
{
	std::list<Collider*> result;
	for (const auto& [first, second] : CollisionSystem::Instance().stayCollisions) 
	{
		// Skip checking for collisions if both Colliders are the same
		if (first->GetUid() == second->GetUid()) 
		{
			continue;
		}

		if (first->GetUid() == GetUid()) 
		{
			result.push_back(second);
		} 
		else if (second->GetUid() == GetUid()) 
		{
			result.push_back(first);
		}
	}
	return result;
}

// Called when the collider exits a collision
std::list<Collider*> Collider::OnCollisionExit() 
{
	std::list<Collider*> result;
	for (const auto& [first, second] : CollisionSystem::Instance().exitCollisions) 
	{
		// Skip checking for collisions if both Colliders are the same
		if (first->GetUid() == second->GetUid()) 
		{
			continue;
		}

		if (first->GetUid() == GetUid()) 
		{
			result.push_back(second);
		} 
		else if (second->GetUid() == GetUid()) 
		{
			result.push_back(first);
		}
	}
	return result;
}
