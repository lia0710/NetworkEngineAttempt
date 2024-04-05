#include "EngineCore.h"
#include "Asset.h"

#define NDEBUG_ASSET_INITIALIZE
#define NDEBUG_ASSET_LOAD

IMPLEMENT_ABSTRACT_CLASS(Asset)

void Asset::Load(json::JSON& json)
{
	Object::Load(json);
	name = filepath;
}
