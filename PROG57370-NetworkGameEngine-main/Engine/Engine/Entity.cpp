#include "EngineCore.h"
#include "Entity.h"
#include "Component.h"
#include "Transform.h"
#include "Scene.h"
#include "NetworkServer.h"

IMPLEMENT_DYNAMIC_CLASS(Entity)

void Entity::Initialize()
{
	transform.Initialize();
	for (const auto component : components)
	{
		component->Initialize();
	}
}

void Entity::Serialize(RakNet::BitStream& bitStream) const
{
	// First the transform
	transform.Serialize(bitStream);

	// Now the rest of the components
	bitStream.Write((unsigned int)components.size());
	for (const auto component : components)
	{
		// Write the component id so we can find it
		bitStream.Write(component->uid);
		component->Serialize(bitStream);
	}
}

void Entity::Deserialize(RakNet::BitStream& bitStream)
{
	// First the transform
	transform.Deserialize(bitStream);

	// Now the rest of the components
	unsigned int compUpdateSize;
	bitStream.Read(compUpdateSize);
	for (int i = 0; i < compUpdateSize; i++)
	{
		STRCODE compUid;
		bitStream.Read(compUid);
		for (const auto component : components)
		{
			if (component->GetUid() == compUid)
			{
				component->Deserialize(bitStream);
				break;
			}
		}
		// Note we should put something here in case we 
		// don't find the component...
	}
}

void Entity::SerializeCreate(RakNet::BitStream& bitStream) const
{
	// Write the entity id
	bitStream.Write(uid);

	// Write the total number of components
	bitStream.Write((unsigned int)components.size());
	for (const auto component : components)
	{
		bitStream.Write(component->uid);

		// Write the STRCODE which is the type of component!
		bitStream.Write(component->GetDerivedClassHashCode());

		component->SerializeCreate(bitStream);
	}
}

void Entity::DeserializeCreate(RakNet::BitStream& bitStream)
{
	bitStream.Read(uid);

	unsigned int numComponents = 0;
	bitStream.Read(numComponents);

	for (int i = 0; i < numComponents; i++)
	{
		unsigned int componentId = 0;
		bitStream.Read(componentId);

		STRCODE componentHash;
		bitStream.Read(componentHash);

		bool found = false;
		for (const auto component : components)
		{
			if (component->uid == componentId)
			{
				component->DeserializeCreate(bitStream);
				found = true;
				break;
			}
		}

		if (found == false)
		{
			Component* component = (Component*)TypeClass::ConstructObject(componentHash);
			component->owner = this;
			component->DeserializeCreate(bitStream);
			component->Initialize();
			components.push_back(component);
		}
	}
}

void Entity::SerializeCreateComponent(Component* component, RakNet::BitStream& bitStream) const
{
	// Write the entity scene id to find later by the manager
	bitStream.Write(ownerScene->GetUid());

	// Write the entity component to find later by Scene
	bitStream.Write(uid);

	// Write the STRCODE which is the type of component!
	bitStream.Write(component->GetDerivedClassHashCode());
	// Write the data
	component->SerializeCreate(bitStream);
}

void Entity::DeserializeCreateComponent(RakNet::BitStream& bitStream)
{
	STRCODE componentHash;
	bitStream.Read(componentHash);

	Component* component = (Component*)TypeClass::ConstructObject(componentHash);
	component->owner = this;
	component->DeserializeCreate(bitStream);
	component->Initialize();
	components.push_back(component);
}

void Entity::Load(json::JSON& node)
{
	Object::Load(node);

	// Load Transform
	if (node.hasKey("Transform"))
	{
		json::JSON transform_json = node["Transform"];
		transform.Load(transform_json);
	}

	// Load the components
	if (node.hasKey("Components"))
	{
		json::JSON components_json = node["Components"];

		for (json::JSON& component_json : components_json.ArrayRange())
		{
			std::string component_name = component_json["ClassName"].ToString();
			Component* component = CreateComponent(component_name);
			component->Load(component_json["ClassData"]);
		}
	}
}

void Entity::PreUpdate()
{
	for (auto component : componentsToAdd)
	{
		components.push_back(component);
		component->Initialize();

		if (NetworkServer::Instance().IsInitialized())
		{
			RakNet::BitStream bitStream;
			bitStream.Write((unsigned char)NetworkPacketIds::MSG_SCENE_MANAGER);
			bitStream.Write((unsigned char)NetworkPacketIds::MSG_CREATE_COMPONENT);
			SerializeCreateComponent(component, bitStream);
			NetworkServer::Instance().SendPacket(bitStream);
		}
	}
	componentsToAdd.clear();
}

void Entity::Update()
{
	transform.Update();
	for (const auto component : components)
	{
		component->Update();
	}
}

void Entity::PostUpdate()
{
	for (auto component : componentsToRemove)
	{
		component->Destroy();
		components.remove(component);
		delete component;
	}
	componentsToRemove.clear();
}

void Entity::Destroy()
{
	for (const auto component : components)
	{
		component->Destroy();
		delete component;
	}
	components.clear();
}

bool Entity::HasComponent(const std::string& componentName) const
{
	for (const auto component : components)
	{
		if (component->GetDerivedClassName() == componentName)
		{
			return true;
		}
	}
	return false;
}

void Entity::AddComponents(const std::vector<std::string>& componentList)
{
	for (const auto& component : componentList)
	{
		CreateComponent(component);
	}
}

Component* Entity::GetComponent(const std::string& componentName) const
{
	for (auto component : components)
	{
		if (component->GetDerivedClassName() == componentName)
		{
			return component;
		}
	}
	return nullptr;
}

Component* Entity::GetComponentByUiD(STRCODE uid)
{
	for (auto component : components)
	{
		if (component->GetUid() == uid)
		{
			return component;
		}
	}
	return nullptr;
}

Component* Entity::CreateComponent(const std::string& componentName)
{
    const auto component = (Component*)CreateObject(componentName.c_str());
	component->owner = this;
	componentsToAdd.push_back(component);
	return component;
}

bool Entity::RemoveComponent(const Component* component)
{
	for (auto c : components)
	{
		if (c == component)
		{
			componentsToRemove.push_back(c);
			return true;
		}
	}
	return false;
}
