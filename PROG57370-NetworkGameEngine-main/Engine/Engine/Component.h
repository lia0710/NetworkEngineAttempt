#pragma once

#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include "Object.h"

class Entity;

class Component : public Object
{
    DECLARE_ABSTRACT_DERIVED_CLASS(Component, Object)

public:
    Entity* GetOwner() const { return owner; }

    // Inherited via ISerializable
    void Serialize(RakNet::BitStream& bitStream) const override;
    void Deserialize(RakNet::BitStream& bitStream) override;
    void SerializeCreate(RakNet::BitStream& bitStream) const override;
    void DeserializeCreate(RakNet::BitStream& bitStream) override;

protected:
    void Destroy() override;

    // Update is protected and can only be accessed from Entity
    virtual void PreUpdate() {};
    virtual void Update() {};
    virtual void PostUpdate() {};

protected:
    Entity* owner = nullptr;  //< Pointer to the entity that owns this component.

    // To allow Entity to access protected/private members
    friend class Entity;
};

#endif // !_COMPONENT_H_
