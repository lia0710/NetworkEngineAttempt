#include "EngineCore.h"
#include "SceneManager.h"
#include "Scene.h"

#include "NetworkServer.h"

void SceneManager::Load()
{
	std::ifstream inputStream(DATA_FILE);
	std::string str((std::istreambuf_iterator<char>(inputStream)), std::istreambuf_iterator<char>());
	json::JSON sceneManagerJSON = json::JSON::Load(str);

	// Store mapping of scene id to scene path location
	// SceneManager must know about every scene in existance
	THROW_RUNTIME_ERROR(!sceneManagerJSON.hasKey("AllScenes"), "Scene Manager must have GIUD & path data of all available scenes.");
	json::JSON allScenes = sceneManagerJSON["AllScenes"];
	for (json::JSON& sceneInfo : allScenes.ArrayRange())
	{
		if (sceneInfo.hasKey("GUID") && sceneInfo.hasKey("Path"))
		{
			std::string sceneGUID = sceneInfo["GUID"].ToString();
			std::string scenePath = sceneInfo["Path"].ToString();

			STRCODE sceneId = GetHashCode(sceneGUID.c_str());
			stringUIDToFile[sceneId] = scenePath;
		}
	}
	
	// Get the active scene GUID. Convert it to STRCODE
	THROW_RUNTIME_ERROR(!sceneManagerJSON.hasKey("ActiveSceneGUID"), "Scene Manager must have an active scene.");
	activeSceneId = GetHashCode(sceneManagerJSON["ActiveSceneGUID"].ToString().c_str());

	// Load the scenes to be loaded at the beginning of the game
	if (sceneManagerJSON.hasKey("LoadedScenes"))
	{
		json::JSON scenesJSON = sceneManagerJSON["LoadedScenes"];
		for (json::JSON& sceneGuidJSON : scenesJSON.ArrayRange())
		{
			THROW_RUNTIME_ERROR(!sceneGuidJSON.hasKey("GUID"), "A loaded scene must have a GUID.");

			STRCODE sceneID = GetHashCode(sceneGuidJSON["GUID"].ToString().c_str());
			if (stringUIDToFile.find(sceneID) != stringUIDToFile.end())
			{
				// Load up Scene JSON file & send the JSON object to Scene
				std::string sceneFileLocation = "../Assets/" + stringUIDToFile[sceneID];

				std::ifstream sceneInputStream(sceneFileLocation);
				std::string sceneStr((std::istreambuf_iterator<char>(sceneInputStream)), std::istreambuf_iterator<char>());
				json::JSON sceneJSON = json::JSON::Load(sceneStr);

				// Load the scene & track it
				Scene* scene = LoadScene(sceneJSON);

				// If this is the active scene, set it
				if (sceneID == activeSceneId)
				{
					activeScene = scene;
					// active scene has to be enabled, or else it won't update and render
					activeScene->isEnabled = true;
				}
			}
		}
	}
	LOG("Loaded SceneManager: " << scenesToBeLoaded.size() << " scenes loaded from JSON.");
}

void SceneManager::Initialize()
{
	for (Scene* scene : loadedScenes)
	{
		scene->Initialize();
	}
}

void SceneManager::NetworkUpdate()
{
	for (Scene* scene : loadedScenes)
	{
		RakNet::BitStream bs;
		bs.Write((unsigned char)NetworkPacketIds::MSG_SCENE_MANAGER);
		bs.Write((unsigned char)NetworkPacketIds::MSG_SCENE_UPDATE);
		bs.Write(scene->uid);
		scene->Serialize(bs);

		NetworkServer::Instance().SendPacket(bs);
	}
}

void SceneManager::SerializeSnapshot()
{
	for (Scene* scene : loadedScenes)
	{
		RakNet::BitStream bitStream;
		bitStream.Write((unsigned char)NetworkPacketIds::MSG_SCENE_MANAGER);
		bitStream.Write((unsigned char)NetworkPacketIds::MSG_SNAPSHOT);
		scene->SerializeSnapshot(bitStream);
		NetworkServer::Instance().SendPacket(bitStream);
	}
}

void SceneManager::ProcessPacket(RakNet::BitStream& bitStream)
{
	unsigned char packet = 0;
	bitStream.Read(packet);
	switch (packet)
	{
		case NetworkPacketIds::MSG_SCENE_UPDATE:
		{
			STRCODE sceneUid = 0;
			bitStream.Read(sceneUid);
			for (Scene* scene : loadedScenes)
			{
				if (scene->uid == sceneUid)
				{
					scene->Deserialize(bitStream);
					break;
				}
			}
		}
		break;

		case NetworkPacketIds::MSG_CREATE_ENTITY:
		{
			STRCODE sceneUid = 0;
			bitStream.Read(sceneUid);
			for (Scene* scene : loadedScenes)
			{
				if (scene->uid == sceneUid)
				{
					scene->DeserializeCreateEntity(bitStream);
					break;
				}
			}
		}
		break;

		case NetworkPacketIds::MSG_CREATE_COMPONENT:
		{
			STRCODE sceneUid = 0;
			bitStream.Read(sceneUid);
			for (Scene* scene : loadedScenes)
			{
				if (scene->uid == sceneUid)
				{
					scene->DeserializeCreateEntityComponent(bitStream);
					break;
				}
			}
		}
		break;


		case MSG_SNAPSHOT:
		{
			STRCODE sceneUid = 0;
			bitStream.Read(sceneUid);
			for (Scene* scene : loadedScenes)
			{
				if (scene->uid == sceneUid)
				{
					scene->DeserializeSnapshot(bitStream);
					break;
				}
			}
		}
		break;

	}
}

void SceneManager::PreUpdate()
{
	for (Scene* scene : scenesToBeLoaded)
	{
		loadedScenes.push_back(scene);
	}
	scenesToBeLoaded.clear();
	// Check if active scene got modified
	if (toBeSetAsActive != nullptr)
	{
		activeScene = toBeSetAsActive;
		// Active scene must be enabled
		activeScene->isEnabled = true;
		toBeSetAsActive = nullptr;
	}
}

void SceneManager::Update()
{
	for (Scene* scene : loadedScenes)
	{
		if (scene->isEnabled)
		{
			scene->PreUpdate();
			scene->Update();
			scene->PostUpdate();
		}
	}
}

void SceneManager::PostUpdate()
{
	for (Scene* scene : scenesToBeUnloaded)
	{
		scene->Destroy();
		delete scene;
		loadedScenes.remove(scene);
	}
	scenesToBeUnloaded.clear();
}

void SceneManager::Destroy()
{
	if (scenesToBeLoaded.size() != 0)
	{
		loadedScenes.merge(scenesToBeLoaded);
	}
	if (scenesToBeUnloaded.size() != 0)
	{
		loadedScenes.merge(scenesToBeUnloaded);
	}
	
	for (Scene* scene : loadedScenes)
	{
		scene->Destroy();
		delete scene;
	}
	scenesToBeUnloaded.clear();
	loadedScenes.clear();
	scenesToBeLoaded.clear();
}

// ------------------------- Scene-related member functions -------------------------

Scene* SceneManager::CreateScene()
{
	// This scene might or might not have a JSON file. Can not add it to stringUIDToFile
	Scene* scene = new Scene();

	// Created scenes automatically get added
	scenesToBeLoaded.push_back(scene);
	
	return scene;
}

Scene* SceneManager::LoadScene(json::JSON& sceneJSON)
{
	Scene* scene = new Scene();
	scene->Load(sceneJSON);

	// Loaded scenes automatically get added
	scenesToBeLoaded.push_back(scene);

	return scene;
}

Scene* SceneManager::GetActiveScene()
{
	return activeScene;
}

STRCODE SceneManager::GetActiveSceneId()
{
	return activeScene->uid;
}

bool SceneManager::SetActiveScene(std::string sceneGuid)
{
	STRCODE sceneId = GetHashCode(sceneGuid.c_str());
	return SetActiveScene(sceneId);
}

bool SceneManager::SetActiveScene(STRCODE sceneId)
{
	// Look for the scene in scenesToBeLoaded & loadedScenes
	for (Scene* scene : scenesToBeLoaded)
	{
		if (scene->GetUid() == sceneId)
		{
			toBeSetAsActive = scene;
			return true;
		}
	}
	for (Scene* scene : loadedScenes)
	{
		if (scene->GetUid() == sceneId)
		{
			// Ensure that this scene is not scheduled to be deleted
			auto it = std::find_if(scenesToBeUnloaded.begin(), scenesToBeUnloaded.end(),
								   [sceneId](Scene* sc) {
										return sc->GetUid() == sceneId;
									});
			THROW_RUNTIME_ERROR(it != scenesToBeUnloaded.end(), "Error! The scene being set as active does not exist anymore.");
			toBeSetAsActive = scene;
			return true;
		}
	}
	return false;
}

Scene* SceneManager::FindScene(std::string sceneGuid)
{
	STRCODE sceneId = GetHashCode(sceneGuid.c_str());
	return FindScene(sceneId);
}

Scene* SceneManager::FindScene(STRCODE sceneId)
{
	for (Scene* scene : loadedScenes)
	{
		if (scene->GetUid() == sceneId)
		{
			return scene;
		}
	}
	return nullptr;
}

bool SceneManager::UnloadScene(std::string sceneGuid)
{
	STRCODE sceneId = GetHashCode(sceneGuid.c_str());
	return UnloadScene(sceneId);
}

bool SceneManager::UnloadScene(STRCODE sceneId)
{
	// Ensure that user is not trying to remove the active scene
	Scene* actualActiveScene = (toBeSetAsActive == nullptr) ? activeScene : toBeSetAsActive;
	if (sceneId == actualActiveScene->GetUid())
	{
		return false;
	}

	for (Scene* scene : loadedScenes)
	{
		if (scene->GetUid() == sceneId)
		{
			scenesToBeUnloaded.push_back(scene);
			return true;
		}
	}
	return false;
}

// ------------------------- Entity-related member functions -------------------------

Entity* SceneManager::CreateEntity()
{
	return activeScene->CreateEntity();
}

Entity* SceneManager::FindEntity(std::string entityGuid)
{
	return activeScene->FindEntity(entityGuid);
}

Entity* SceneManager::FindEntity(STRCODE entityId)
{
	return activeScene->FindEntity(entityId);
}

std::list<Entity*> SceneManager::FindEntityByName(std::string entityName)
{
	return activeScene->FindEntityByName(entityName);
}

std::list<Entity*> SceneManager::FindEntityWithComponent(std::string componentClassName)
{
	return activeScene->FindEntityWithComponent(componentClassName);
}

bool SceneManager::RemoveEntity(std::string entityGuid)
{
	return activeScene->RemoveEntity(entityGuid);
}

bool SceneManager::RemoveEntity(STRCODE entityId)
{
	return activeScene->RemoveEntity(entityId);
}