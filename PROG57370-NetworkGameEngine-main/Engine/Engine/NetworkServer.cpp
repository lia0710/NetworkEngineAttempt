#include "EngineCore.h"
#include "NetworkServer.h"
#include "SceneManager.h"

void NetworkServer::Initialize()
{
	initialized = true;
	LoadSettings();
	rakInterface = RakNet::RakPeerInterface::GetInstance();
	state = NetworkServer::CREATE_SERVER;
}

void NetworkServer::LoadSettings()
{
	std::ifstream inputStream("../Assets/NetworkSettings.json");
	std::string str((std::istreambuf_iterator<char>(inputStream)), std::istreambuf_iterator<char>());
	json::JSON document = json::JSON::Load(str);

	THROW_RUNTIME_ERROR(document.hasKey("port") == false, "Unable to determine ip address");
	port = document["port"].ToInt();
}

void NetworkServer::Update()
{
	if (!initialized)
	{
		return;
	}

	switch (state)
	{
	case NetworkServer::DISABLED:
		break;

	case NetworkServer::CREATE_SERVER:
		SetupServer();
		break;

	case NetworkServer::RUNNING:
		_Update();
		break;

	default:
		break;
	}
}

void NetworkServer::SendPacket(RakNet::BitStream& bs)
{
	for (int i = 0; i < clientConnections.size(); i++)
	{
		rakInterface->Send(&bs, MEDIUM_PRIORITY, RELIABLE_ORDERED, 0, clientConnections[i], false);
	}
}

void NetworkServer::SetupServer()
{
	RakNet::SocketDescriptor sd(port, NULL);
	if (rakInterface->Startup(8, &sd, 1) != RakNet::RAKNET_STARTED)
	{
		std::cerr << "*** Failed to start server on port " << port << std::endl;
		exit(1);
	}

	// set maximum number of incoming connections
	rakInterface->SetMaximumIncomingConnections(8);

	std::cout << "Listening on port " << port << std::endl;
	std::cout << "IP addresses:" << std::endl;
	for (unsigned i = 0; i < rakInterface->GetNumberOfAddresses(); i++)
	{
		std::cout << rakInterface->GetLocalIP(i) << std::endl;
	}

	state = NetworkServer::RUNNING;
}

void NetworkServer::_Update()
{
	// get a packet
	RakNet::Packet* packet = rakInterface->Receive();

	while (packet != NULL)
	{
		RakNet::BitStream bs(packet->data, packet->length, false);

		unsigned char packetId;
		bs.Read(packetId);

		switch (packetId) {

		case ID_NEW_INCOMING_CONNECTION:
			// Somebody connected
			std::cout << "Got connection from " << packet->systemAddress.ToString(true) << std::endl;
			clientConnections.push_back(packet->guid);
			SceneManager::Instance().SerializeSnapshot();
			break;

		case ID_DISCONNECTION_NOTIFICATION:
			// Connection lost normally
			std::cout << "Disconnected from " << packet->systemAddress.ToString(true) << std::endl;
			{
				std::vector<RakNet::RakNetGUID>::iterator position = std::find(clientConnections.begin(), clientConnections.end(), packet->guid);
				if (position != clientConnections.end())
				{
					clientConnections.erase(position);
				}
			}
			break;

		case ID_CONNECTION_LOST:
			// Couldn't deliver a reliable packet - i.e. the other system was abnormally terminated
			std::cout << "Connection lost to " << packet->systemAddress.ToString(true) << std::endl;
			break;

		case ID_INCOMPATIBLE_PROTOCOL_VERSION:
			std::cerr << "Incomplatible protocol version from " << packet->systemAddress.ToString(true) << std::endl;
			break;

		default:
			std::cout << "Oops, received an unhandled packet with id " << (unsigned)packetId << std::endl;
			break;
		}

		rakInterface->DeallocatePacket(packet);

		// get next packet
		packet = rakInterface->Receive();
	}
}