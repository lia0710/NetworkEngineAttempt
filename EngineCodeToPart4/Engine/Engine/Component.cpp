#include "EngineCore.h"
#include "Component.h"

IMPLEMENT_ABSTRACT_CLASS(Component)

void Component::Destroy()
{
    Object::Destroy();
    owner = nullptr;
}

void Component::Serialize(RakNet::BitStream& bitStream) const
{
    Object::Serialize(bitStream);
}

void Component::Deserialize(RakNet::BitStream& bitStream)
{
    Object::Deserialize(bitStream);
}

void Component::SerializeCreate(RakNet::BitStream& bitStream) const
{
    Object::SerializeCreate(bitStream);
}

void Component::DeserializeCreate(RakNet::BitStream& bitStream)
{
    Object::DeserializeCreate(bitStream);
}

