#include "GameCore.h"
#include "Player.h"
#include "Sprite.h"
#include "TextureAsset.h"
#include "BoxCollider.h"
#include "NetworkRPC.h"
#include "NetworkServer.h"
#include "NetworkClient.h"

#define NDEBUG_PLAYER

IMPLEMENT_DYNAMIC_CLASS(Player)

void Player::Initialize()
{
	Component::Initialize();
	start_pos = owner->GetTransform().position;
	collider = (BoxCollider*)owner->GetComponent("BoxCollider");
	
	RegisterRPC(GetHashCode("RPC"), std::bind(&Player::RPC, this, std::placeholders::_1));
	RegisterRPC(GetHashCode("RPCNetworkedEntity"), std::bind(&Player::RPCNetworkedEntity, this, std::placeholders::_1));
}

void Player::Update() 
{
	bool spawn = false;
	if (NetworkServer::Instance().IsInitialized() == false)
	{
		movement = Vec2::Zero;
		const InputSystem& input = InputSystem::Instance();

		if (input.IsKeyPressed(SDLK_q) && !spawned)//networkedEntity == nullptr) //there's a several frame delay when an rpc sends
		{
			/*networkedEntity = SceneManager::Instance().CreateEntity();
			Sprite* sprite = (Sprite*)networkedEntity->CreateComponent("Sprite");
			TextureAsset* asset = (TextureAsset*)AssetManager::Instance().GetAsset("Explosion_435e0fce-7b11-409c-858e-af4bd7fe99c0");
			sprite->SetTextureAsset(asset);*/

			spawned = true;
			spawn = true;
		}

		// Handle horizontal movement
		if (input.IsKeyPressed(SDLK_LEFT) || input.IsKeyPressed(SDLK_a) || input.IsGamepadButtonPressed(0, SDL_CONTROLLER_BUTTON_DPAD_LEFT)) {
			movement.x -= 1;
			//if (networkedEntity) networkedEntity->GetTransform().position.x -= 1;
		}
		if (input.IsKeyPressed(SDLK_RIGHT) || input.IsKeyPressed(SDLK_d) || input.IsGamepadButtonPressed(0, SDL_CONTROLLER_BUTTON_DPAD_RIGHT)) {
			movement.x += 1;
			//if (networkedEntity) networkedEntity->GetTransform().position.x += 1;
		}

		// Handle vertical movement
		if (input.IsKeyPressed(SDLK_UP) || input.IsKeyPressed(SDLK_w) || input.IsGamepadButtonPressed(0, SDL_CONTROLLER_BUTTON_DPAD_UP)) {
			movement.y -= 1;
			//if (networkedEntity) networkedEntity->GetTransform().position.y -= 1;
		}
		if (input.IsKeyPressed(SDLK_DOWN) || input.IsKeyPressed(SDLK_s) || input.IsGamepadButtonPressed(0, SDL_CONTROLLER_BUTTON_DPAD_DOWN)) {
			movement.y += 1;
			//if (networkedEntity) networkedEntity->GetTransform().position.y += 1;
		}

		// Handle gamepad analog stick input
		if (movement == Vec2::Zero) {
			movement.x = input.GetGamepadAxisState(0, SDL_CONTROLLER_AXIS_LEFTX);
			movement.y = input.GetGamepadAxisState(0, SDL_CONTROLLER_AXIS_LEFTY);
		}

		// Normalize the direction vector if it's not zero
		if (movement != Vec2::Zero) {
			movement.Normalize();
#ifdef DEBUG_PLAYER
			LOG("Input: " << dir.x << ", " << dir.y);
#endif
		}


	}

	if (NetworkClient::Instance().IsInitialized() == true)
	{
		if (movement != Vec2::Zero)
		{
			RakNet::BitStream bitStream;

			bitStream.Write((unsigned char)MSG_SCENE_MANAGER);
			bitStream.Write((unsigned char)MSG_RPC);

			bitStream.Write(owner->GetParentScene()->GetUid());

			bitStream.Write(owner->GetUid());

			bitStream.Write(GetUid());
			bitStream.Write(GetHashCode("RPC"));

			bitStream.Write(movement.x);
			bitStream.Write(movement.y);

			NetworkClient::Instance().SendPacket(bitStream);
		}
		if (spawn) 
		{
			//one time code, spawn the explosion
			RakNet::BitStream bitStream;

			bitStream.Write((unsigned char)MSG_SCENE_MANAGER);
			bitStream.Write((unsigned char)MSG_RPC);

			bitStream.Write(owner->GetParentScene()->GetUid());

			bitStream.Write(owner->GetUid());

			bitStream.Write(GetUid());
			bitStream.Write(GetHashCode("RPCNetworkedEntity"));

			NetworkClient::Instance().SendPacket(bitStream);
		}
		return;
	}

	//this code here is server only
	if (movement != Vec2::Zero)
	{
		//move the explosion
		if (networkedEntity)
		{
			networkedEntity->GetTransform().position += movement * (speed * Time::Instance().DeltaTime());
		}
		owner->GetTransform().position += movement * (speed * Time::Instance().DeltaTime());

		if (collider == nullptr)
		{
			LOG("no collider uwu");
			return;
		}

		//////////////////collision code handled here
		for (const auto& other : collider->OnCollisionEnter())
		{
			if (other->GetOwner()->GetName() != "Enemy")
			{
				continue;
			}

			Scene* current_scene = SceneManager::Instance().GetActiveScene();
			if (SceneManager::Instance().SetActiveScene(game_over_scene))
			{
				current_scene->SetEnabled(false);
			}
			// Move the player
			owner->GetTransform().position = start_pos;
		}
		movement = Vec2::Zero;
	}





















	/*
	//////////////////////////////////////
	Vec2 dir = Vec2::Zero;
	const InputSystem& input = InputSystem::Instance();

	if (input.IsKeyPressed(SDLK_q) && networkedEntity == nullptr)
	{
			networkedEntity = SceneManager::Instance().CreateEntity();
			Sprite* sprite = (Sprite*)networkedEntity->CreateComponent("Sprite");
			TextureAsset* asset = (TextureAsset*)AssetManager::Instance().GetAsset("Explosion_435e0fce-7b11-409c-858e-af4bd7fe99c0");
			sprite->SetTextureAsset(asset);
	}

	// Handle horizontal movement
	if (input.IsKeyPressed(SDLK_LEFT) || input.IsKeyPressed(SDLK_a) || input.IsGamepadButtonPressed(0, SDL_CONTROLLER_BUTTON_DPAD_LEFT)) {
		dir.x -= 1;
		if (networkedEntity) networkedEntity->GetTransform().position.x -= 1;
	}
	if (input.IsKeyPressed(SDLK_RIGHT) || input.IsKeyPressed(SDLK_d) || input.IsGamepadButtonPressed(0, SDL_CONTROLLER_BUTTON_DPAD_RIGHT)) {
		dir.x += 1;
		if (networkedEntity) networkedEntity->GetTransform().position.x += 1;
	}

	// Handle vertical movement
	if (input.IsKeyPressed(SDLK_UP) || input.IsKeyPressed(SDLK_w) || input.IsGamepadButtonPressed(0, SDL_CONTROLLER_BUTTON_DPAD_UP)) {
		dir.y -= 1;
		if (networkedEntity) networkedEntity->GetTransform().position.y -= 1;
	}
	if (input.IsKeyPressed(SDLK_DOWN) || input.IsKeyPressed(SDLK_s) || input.IsGamepadButtonPressed(0, SDL_CONTROLLER_BUTTON_DPAD_DOWN)) {
		dir.y += 1;
		if (networkedEntity) networkedEntity->GetTransform().position.y += 1;
	}

	// Handle gamepad analog stick input
	if (dir == Vec2::Zero) {
		dir.x = input.GetGamepadAxisState(0, SDL_CONTROLLER_AXIS_LEFTX);
		dir.y = input.GetGamepadAxisState(0, SDL_CONTROLLER_AXIS_LEFTY);
	}

	// Normalize the direction vector if it's not zero
	if (dir != Vec2::Zero) {
		dir.Normalize();
#ifdef DEBUG_PLAYER
		LOG("Input: " << dir.x << ", " << dir.y);
#endif
	}

	// Move the player
	owner->GetTransform().position += dir * (speed * Time::Instance().DeltaTime());

	if (collider == nullptr)
	{
		LOG("no collider uwu");
		return;
	}
	for (const auto& other : collider->OnCollisionEnter())
	{
		if (other->GetOwner()->GetName() != "Enemy")
		{
			continue;
		}

		Scene* current_scene = SceneManager::Instance().GetActiveScene();
		if (SceneManager::Instance().SetActiveScene(game_over_scene))
		{
			current_scene->SetEnabled(false);
		}

		owner->GetTransform().position = start_pos;
	}*/
}
void Player::Load(json::JSON& node)
{
	Component::Load(node);
	if (node.hasKey("Speed"))
	{
		speed = static_cast<float>(node.at("Speed").ToFloat());
	}

	if (node.hasKey("DeathScene"))
	{
		game_over_scene = GetHashCode(node.at("DeathScene").ToString().c_str());
	}
}

void Player::RPC(RakNet::BitStream& bitStream)
{
	float value = 0.0f;
	bitStream.Read(value);
	movement.x += value;
	bitStream.Read(value);
	movement.y += value;
}

void Player::RPCNetworkedEntity(RakNet::BitStream& bitStream)
{
	//spawn the explosion on server

	networkedEntity = SceneManager::Instance().CreateEntity();
	Sprite* sprite = (Sprite*)networkedEntity->CreateComponent("Sprite");
	TextureAsset* asset = (TextureAsset*)AssetManager::Instance().GetAsset("Explosion_435e0fce-7b11-409c-858e-af4bd7fe99c0");
	sprite->SetTextureAsset(asset);
}