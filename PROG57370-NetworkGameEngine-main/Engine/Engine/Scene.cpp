#include "EngineCore.h"
#include "Scene.h"
#include "Entity.h"
#include "AssetManager.h"
#include "NetworkServer.h"

void Scene::Serialize(RakNet::BitStream& bitStream) const
{
	bitStream.Write((unsigned int)entities.size());
	for (Entity* entity : entities)
	{
		// Write the component id so we can look it up later
		bitStream.Write(entity->uid);
		entity->Serialize(bitStream);
	}
}

void Scene::Deserialize(RakNet::BitStream& bitStream)
{
	// Now the rest of the components
	unsigned int entityCount = 0;
	bitStream.Read(entityCount);
	for (int i = 0; i < entityCount; i++)
	{
		STRCODE entityUid;
		bitStream.Read(entityUid);
		for (const auto entity : entities)
		{
			if (entity->GetUid() == entityUid)
			{
				entity->Deserialize(bitStream);
			}
		}
	}
}

void Scene::SerializeCreateEntity(Entity* entity, RakNet::BitStream& bitStream) const
{
	// Write the Scene id (looked up by the manager)
	bitStream.Write(uid);

	// Entity will write the id and other associated data
	entity->SerializeCreate(bitStream);
}

void Scene::DeserializeCreateEntity(RakNet::BitStream& bitStream)
{
	Entity* entity = new Entity();
	entity->ownerScene = this;
	entity->DeserializeCreate(bitStream);
	entities.push_back(entity);
}

void Scene::DeserializeCreateEntityComponent(RakNet::BitStream& bitStream)
{
	STRCODE entityUid;
	bitStream.Read(entityUid);

	for (const auto entity : entities)
	{
		if (entity->GetUid() == entityUid)
		{
			entity->DeserializeCreateComponent(bitStream);
		}
	}
}

void Scene::SerializeSnapshot(RakNet::BitStream& bitStream)
{
	// Write the Scene id (looked up by the scene manager)
	bitStream.Write(uid);

	// Write the total number of enities
	bitStream.Write((unsigned int)entities.size());

	for (const auto entity : entities)
	{
		// Write the Entity id (looked up by the scene)
		bitStream.Write(entity->uid);

		// Entity will write entity (everything neded)
		entity->SerializeCreate(bitStream);
	}
}

void Scene::DeserializeSnapshot(RakNet::BitStream& bitStream)
{
	unsigned int numberOfEntities = -1;
	bitStream.Read(numberOfEntities);

	for (int i = 0; i < numberOfEntities; i++)
	{
		STRCODE entityId = 0;
		bitStream.Read(entityId);

		bool found = false;
		for (const auto entity : entities)
		{
			if (entity->GetUid() == entityId)
			{
				entity->DeserializeCreate(bitStream);
				found = true;
				break;
			}
		}

		if (found == false)
		{
			Entity* entity = new Entity();
			entity->ownerScene = this;
			entity->DeserializeCreate(bitStream);
			entities.push_back(entity);
		}
	}
}

void Scene::Initialize()
{
	for (Entity* entity : entities)
	{
		entity->Initialize();
	}
}

void Scene::Load(json::JSON& sceneJSON)
{
	THROW_RUNTIME_ERROR(!sceneJSON.hasKey("Scene"), "Scene JSON must contain scene info.");

	// Load all the assets used by this scene
	if (sceneJSON.hasKey("AssetManager"))
	{
		json::JSON assetsJSON = sceneJSON["AssetManager"];
		if (assetsJSON.hasKey("Assets"))
		{
			for (json::JSON& assetJSON : assetsJSON["Assets"].ArrayRange())
			{
				if (assetJSON.hasKey("GUID"))
				{
					std::string assetGUID = assetJSON["GUID"].ToString();
					AssetManager::Instance().LoadSceneAsset(assetGUID);
					assetsGUIDs.push_back(assetGUID);
				}
			}
		}
	}

	json::JSON sceneData = sceneJSON["Scene"];
	if (sceneData.hasKey("Name"))
	{
		name = sceneData["Name"].ToString();
	}

	// If GUID exists, it overwrites the guid & uid populated in Scene constructor
	if (sceneData.hasKey("GUID"))
	{
		guid = sceneData["GUID"].ToString();
		uid = GetHashCode(guid.c_str());
	}

	if (sceneData.hasKey("IsEnabled"))
	{
		isEnabled = sceneData["IsEnabled"].ToBool();
	}

	// Load the entities
	if (sceneData.hasKey("Entities"))
	{
		json::JSON entitiesJSON = sceneData["Entities"];
		for (json::JSON& entityJSON : entitiesJSON.ArrayRange())
		{
			Entity* entity = CreateEntity();
			entity->Load(entityJSON);
		}
	}
}

void Scene::PreUpdate()
{
	for (Entity* entity : entitiesToBeAdded)
	{
		entities.push_back(entity);

		if (NetworkServer::Instance().IsInitialized())
		{
			RakNet::BitStream bitStream;
			bitStream.Write((unsigned char)NetworkPacketIds::MSG_SCENE_MANAGER);
			bitStream.Write((unsigned char)NetworkPacketIds::MSG_CREATE_ENTITY);
			SerializeCreateEntity(entity, bitStream);
			NetworkServer::Instance().SendPacket(bitStream);
		}
	}
	entitiesToBeAdded.clear();

	for (Entity* entity : entities)
	{
		if (entity->IsActive())
		{
			entity->PreUpdate();
		}
	}
}

void Scene::Update()
{
	for (Entity* entity : entities)
	{
		if (entity->IsActive())
		{
			entity->Update();
		}
	}
}

void Scene::PostUpdate()
{
	for (Entity* entity : entitiesToDestroy)
	{
		entity->Destroy();
		delete entity;
		entities.remove(entity);
	}
	entitiesToDestroy.clear();

	for (Entity* entity : entities)
	{
		if (entity->IsActive())
		{
			entity->PostUpdate();
		}
	}
}

void Scene::Destroy()
{
	for (Entity* entity : entities)
	{
		entity->Destroy();
		delete entity;
	}
	entities.clear();

	// Unload all assets
	for (std::string& assetGUID : assetsGUIDs)
	{
		AssetManager::Instance().UnloadSceneAsset(assetGUID);
	}
}

Entity* Scene::CreateEntity()
{
	Entity* entity = new Entity();
	entity->ownerScene = this;
	entitiesToBeAdded.push_back(entity);
	return entity;
}

Entity* Scene::FindEntity(std::string entityGuid)
{
	STRCODE entityId = GetHashCode(entityGuid.c_str());
	return FindEntity(entityId);
}

Entity* Scene::FindEntity(STRCODE entityId)
{
	for (Entity* entity : entities)
	{
		if (entity->GetUid() == entityId)
		{
			return entity;
		}
	}
	return nullptr;
}

std::list<Entity*> Scene::FindEntityByName(std::string entityName)
{
	std::list<Entity*> foundEntities;
	for (Entity* entity : entities)
	{
		if (entity->GetName() == entityName)
		{
			foundEntities.push_back(entity);
		}
	}
	return foundEntities;
}

std::list<Entity*> Scene::FindEntityWithComponent(std::string componentName)
{
	std::list<Entity*> foundEntities;
	for (Entity* entity : entities)
	{
		if (entity->GetComponent(componentName))
		{
			foundEntities.push_back(entity);
		}
	}
	return foundEntities;
}

bool Scene::RemoveEntity(std::string entityGuid)
{
	STRCODE entityId = GetHashCode(entityGuid.c_str());
	return RemoveEntity(entityId);
}

bool Scene::RemoveEntity(STRCODE entityId)
{
	for (Entity* entity : entities)
	{
		if (entity->GetUid() == entityId)
		{
			entitiesToDestroy.push_back(entity);
			return true;
		}
	}
	return false;
}