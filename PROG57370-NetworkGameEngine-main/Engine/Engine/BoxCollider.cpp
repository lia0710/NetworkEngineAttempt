#include "EngineCore.h"
#include "BoxCollider.h"

IMPLEMENT_DYNAMIC_CLASS(BoxCollider);

void BoxCollider::SetSize(int width, int height) const
{
	if (rect == nullptr) 
	{
		return;
	}
	*rect = { static_cast<int>(owner->GetTransform().position.x), static_cast<int>(owner->GetTransform().position.y), width, height };
}

SDL_Rect BoxCollider::GetBounds() const
{
	if (rect == nullptr) 
	{
		return { 0, 0, 0, 0 };
	}
	return *rect;
}

float BoxCollider::GetBroadPhaseRadius() const 
{
	if (rect == nullptr) 
	{
		return 0.0f;
	}
	return Vec2(static_cast<float>(rect->w), static_cast<float>(rect->h)).Magnitude();
}