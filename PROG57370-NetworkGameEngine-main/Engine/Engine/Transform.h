#pragma once

#include "Component.h"
#include "EngineMath.h"  

class Transform : public Component 
{
    DECLARE_DYNAMIC_DERIVED_CLASS(Transform, Component)

public:
    Transform();

    void Serialize(RakNet::BitStream& bitStream) const override;
    void Deserialize(RakNet::BitStream& bitStream) override;

    void Load(json::JSON& node) override;

    void Translate(const Vec2& delta);
    void Rotate(float delta);
    void Scale(const Vec2& delta);

public:
    Vec2 position; ///< The position of the entity in the game world.
    float rotation; ///< The rotation of the entity, in degrees or radians.
    Vec2 scale; ///< The scale of the entity.
};