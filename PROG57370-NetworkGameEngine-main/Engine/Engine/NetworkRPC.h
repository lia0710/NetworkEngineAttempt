#pragma once

#ifndef _NETWORK_RPC
#define _NETWORK_RPC

#include "EngineCore.h"

class NetworkRPC
{
private:
	std::map<STRCODE, std::function<void(RakNet::BitStream&)>> rpcFunctions;

public:
	void InvokeRPC(RakNet::BitStream& bitStream);

protected:
	virtual void RegisterRPC(STRCODE functionName, std::function<void(RakNet::BitStream&)> functionPtr);
};

#endif