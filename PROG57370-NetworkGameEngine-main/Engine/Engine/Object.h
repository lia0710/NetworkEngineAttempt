#pragma once

#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "ISerializable.h"

class Object : public ISerializable
{
    DECLARE_ABSTRACT_BASE_CLASS(Object)

public:

    bool IsActive() const { return isActive; }
    std::string& GetName() { return name; }
    std::string& GetGuid() { return guid; }
    STRCODE GetUid() const { return uid; }

    void Serialize(RakNet::BitStream& bitStream) const;
    void Deserialize(RakNet::BitStream& bitStream);
    void SerializeCreate(RakNet::BitStream& bitStream) const override;
    void DeserializeCreate(RakNet::BitStream& bitStream) override;

protected:
    Object();
    virtual ~Object() = default;

    virtual void Load(json::JSON&);
    virtual void Initialize() {};
    virtual void Destroy() {}

protected:
    bool isActive = true;
    std::string name;
    std::string guid;
    STRCODE uid = -1;
};

#endif // !_OBJECT_H_
