#pragma once

#include "../Component.h"
#include "../../../utils/pkmath.h"
#include "../../../graphics/Texture.h"

namespace pk
{
	class Sprite3DRenderable : public Component
	{
	public:

		vec3 position;
		vec2 scale;
		vec2 textureOffset;
		Texture* texture;

		Sprite3DRenderable(const vec3& pos, const vec2& scale, Texture* texture) :
			Component(ComponentType::PK_RENDERABLE_SPRITE3D),
			position(pos), scale(scale), textureOffset(0, 0),
			texture(texture)
		{}
	};
}
