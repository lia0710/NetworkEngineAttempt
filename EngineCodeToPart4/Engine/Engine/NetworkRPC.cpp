#include "EngineCore.h"
#include "NetworkRPC.h"
#include "EngineCore.h"
#include "NetworkRPC.h"

void NetworkRPC::RegisterRPC(STRCODE functionName, std::function<void(RakNet::BitStream&)> functionPtr)
{
	rpcFunctions.insert(std::pair<STRCODE, std::function<void(RakNet::BitStream&)>>(functionName, functionPtr));
}

void NetworkRPC::InvokeRPC(RakNet::BitStream& bitStream)
{
	STRCODE function;
	bitStream.Read(function);

	auto iter = rpcFunctions.find(function);
	if (iter != rpcFunctions.end())
	{
		(*iter).second(bitStream);
	}
}