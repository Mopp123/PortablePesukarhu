#pragma once

#include "Component.h"
#include "../../utils/pkmath.h"

namespace pk
{

	class Transform : public Component
	{
	private:

		mat4 _transformationMatrix;

	public:

		Transform(vec2 pos, vec2 scale) :
			Component(ComponentType::PK_TRANSFORM)
		{
			_transformationMatrix.setIdentity();
			
			setPos(pos);
			setScale(scale);
		}

		void setPos(vec2 pos)
		{
			_transformationMatrix[0 + 3 * 4] = pos.x;
			_transformationMatrix[1 + 3 * 4] = pos.y;
		}

		void setScale(vec2 scale)
		{
			_transformationMatrix[0 + 0 * 4] = scale.x;
			_transformationMatrix[1 + 1 * 4] = scale.y;
		}

		inline vec2 getPos() const { return { _transformationMatrix[0 + 3 * 4], _transformationMatrix[1 + 3 * 4] }; }

		const mat4& getTransformationMatrix() const { return _transformationMatrix; }
		mat4& accessTransformationMatrix() { return _transformationMatrix; }
	};
}