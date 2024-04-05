#include "EngineCore.h"
#include "Object.h"

IMPLEMENT_ABSTRACT_CLASS(Object)

Object::Object()
{
    UUID uuid;
    CreateUUID(&uuid);

    uid = GUIDToSTRCODE(uuid);
    guid = GUIDTostring(uuid);
}

void Object::Load(json::JSON& node)
{
    if (node.hasKey("Name"))
    {
        name = node["Name"].ToString();
    }

    THROW_RUNTIME_ERROR(!node.hasKey("GUID"), "All objects require a GUID");
    guid = node["GUID"].ToString();
    uid = GetHashCode(guid.c_str());
}

#pragma region Network Code

void Object::Serialize(RakNet::BitStream& bitStream) const
{
    bitStream.Write(uid);
}

void Object::Deserialize(RakNet::BitStream& bitStream)
{
    bitStream.Read(uid);
}

void Object::SerializeCreate(RakNet::BitStream& bitStream) const
{
    bitStream.Write(uid);
}

void Object::DeserializeCreate(RakNet::BitStream& bitStream)
{
    bitStream.Read(uid);
}

#pragma endregion
