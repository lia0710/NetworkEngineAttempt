#pragma once

#ifndef _INETWORK_RPC_H
#define _INETWORK_RPC_H

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