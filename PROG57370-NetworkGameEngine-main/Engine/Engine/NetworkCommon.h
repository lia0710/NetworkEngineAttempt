#pragma once

#include <MessageIdentifiers.h>

enum NetworkPacketIds
{
	ID_DEFAULT = ID_USER_PACKET_ENUM,

	//Scene Manager
	MSG_SCENE_MANAGER,
	MSG_SCENE_UPDATE,
	MSG_CREATE_ENTITY,
	MSG_CREATE_COMPONENT,
	MSG_SNAPSHOT
};
