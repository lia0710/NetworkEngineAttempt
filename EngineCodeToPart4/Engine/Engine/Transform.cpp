#include "EngineCore.h"
#include "Transform.h"

IMPLEMENT_DYNAMIC_CLASS(Transform);

Transform::Transform() : 
	position(Vec2::Zero), rotation(0), scale(Vec2(1))
{
}

void Transform::Load(json::JSON& node)
{
	Component::Load(node);

	if (node.hasKey("Position"))
	{
		position = vec2_from_json(node["Position"]);
	}
	if (node.hasKey("Rotation"))
	{
		rotation = static_cast<float>(node["Rotation"].ToFloat());
	}
	if (node.hasKey("Scale"))
	{
		scale = vec2_from_json(node["Scale"]);
	}
}

void Transform::Translate(const Vec2& delta)
{
	position += delta;
}

void Transform::Rotate(float delta)
{
	rotation += delta;
}

void Transform::Scale(const Vec2& delta)
{
	scale *= delta;
}

void Transform::Serialize(RakNet::BitStream& bitStream) const
{
	bitStream.Write(position.x);
	bitStream.Write(position.y);
	bitStream.Write(rotation);
	bitStream.Write(scale.x);
	bitStream.Write(scale.y);
}

void Transform::Deserialize(RakNet::BitStream& bitStream)
{
	bitStream.Read(position.x);
	bitStream.Read(position.y);
	bitStream.Read(rotation);
	bitStream.Read(scale.x);
	bitStream.Read(scale.y);
}
