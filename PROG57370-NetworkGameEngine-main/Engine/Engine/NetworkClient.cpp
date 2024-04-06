#include "EngineCore.h"
#include "NetworkClient.h"
#include "SceneManager.h"

void NetworkClient::Initialize()
{
	initialized = true;
	LoadSettings();
	state = NetworkClient::WAITING_TO_CONNECT;
	rakInterface = RakNet::RakPeerInterface::GetInstance();
	RakNet::SocketDescriptor sd(0, NULL);
	rakInterface->Startup(1, &sd, 1);
}

void NetworkClient::LoadSettings()
{
	std::ifstream inputStream("../Assets/NetworkSettings.json");
	std::string str((std::istreambuf_iterator<char>(inputStream)), std::istreambuf_iterator<char>());
	json::JSON document = json::JSON::Load(str);

	THROW_RUNTIME_ERROR(document.hasKey("ipaddress") == false, "Unable to determine ip address");
	ipAddress = document["ipaddress"].ToString();

	THROW_RUNTIME_ERROR(document.hasKey("port") == false, "Unable to determine ip address");
	port = document["port"].ToInt();
}

void NetworkClient::Update()
{
	if (!initialized)
	{
		return;
	}

	switch (state)
	{
	case NetworkClient::DISABLED:
		break;

	case NetworkClient::WAITING_TO_CONNECT:
		if (rakInterface->Connect(ipAddress.c_str(), port, NULL, 0) != RakNet::CONNECTION_ATTEMPT_STARTED)
		{
			std::cerr << "*** Failed to connect to server. Going to try later....." << std::endl;
		}
		else
		{
			state = NetworkClient::WAITING_FOR_FIRST_PACKET;
		}
		break;

	case NetworkClient::WAITING_FOR_FIRST_PACKET:
		WaitingForFirstPacket();
		break;

	case NetworkClient::CONNECTED:
		_Update();
		break;

	default:
		break;
	}
}

void NetworkClient::WaitingForFirstPacket()
{
	// wait for the first packet to arrive
	RakNet::Packet* packet = rakInterface->Receive();
	if (packet == nullptr)
	{
		return;
	}

	// get the packet type identifier
	unsigned char packetId = packet->data[0];
	switch (packetId) {
	case ID_CONNECTION_REQUEST_ACCEPTED:
		std::cout << "\nConnected to " << packet->systemAddress.ToString(true) << std::endl;
		state = NetworkClient::CONNECTED;
		break;

	case ID_CONNECTION_ATTEMPT_FAILED:
		std::cerr << "*** Connection attempt failed" << std::endl;
		state = NetworkClient::NETWORK_ERROR;
		break;

	case ID_NO_FREE_INCOMING_CONNECTIONS:
		std::cerr << "*** Server is full" << std::endl;
		state = NetworkClient::NETWORK_ERROR;
		break;

	case ID_INCOMPATIBLE_PROTOCOL_VERSION:
		std::cerr << "*** Incompatible protocol version" << std::endl;
		state = NetworkClient::NETWORK_ERROR;
		break;

	default:
		std::cerr << "*** Unknown connection response" << std::endl;
		state = NetworkClient::NETWORK_ERROR;
		break;
	}

	serverGUID = packet->guid;

	rakInterface->DeallocatePacket(packet);
}

void NetworkClient::_Update()
{
	unsigned char packetId;

	RakNet::Packet* packet = rakInterface->Receive();
	while (packet != nullptr)
	{
		RakNet::BitStream bs(packet->data, packet->length, false);
		bs.Read(packetId);

		switch (packetId)
		{
		case ID_DISCONNECTION_NOTIFICATION:
		case ID_CONNECTION_LOST:
			state = NetworkClient::NETWORK_ERROR;
			std::cout << "Disconnected from server" << std::endl;
			break;

		case MSG_SCENE_MANAGER:
			SceneManager::Instance().ProcessPacket(bs);
			break;
		}

		rakInterface->DeallocatePacket(packet);
		packet = rakInterface->Receive();
	}
}

void NetworkClient::SendPacket(RakNet::BitStream& bs)
{
	rakInterface->Send(&bs, MEDIUM_PRIORITY, RELIABLE_ORDERED, 0, serverGUID, false);
}
