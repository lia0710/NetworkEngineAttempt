#pragma once

#include <BitStream.h> // RakNet bitstreams

class ISerializable
{
public:
    virtual ~ISerializable() {}

    virtual void SerializeCreate(RakNet::BitStream& bitStream) const {};
    virtual void DeserializeCreate(RakNet::BitStream& bitStream) {};

    virtual void Serialize(RakNet::BitStream& bitStream) const {};
    virtual void Deserialize(RakNet::BitStream& bitStream) {};
};