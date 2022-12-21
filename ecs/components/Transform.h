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

		Transform(vec3 pos, vec3 scale) :
			Component(ComponentType::PK_TRANSFORM)
		{
			_transformationMatrix.setIdentity();
			setPos(pos);
			setScale(scale);
		}

                Transform(vec3 pos, vec3 scale, float pitch, float yaw):
		    Component(ComponentType::PK_TRANSFORM)
                {
                    _transformationMatrix.setIdentity();

                    mat4 pitchMatrix;
                    pitchMatrix.setIdentity();
                    pitchMatrix[1 + 1 * 4] =  std::cos(-pitch);
                    pitchMatrix[1 + 2 * 4] = -std::sin(-pitch);
                    pitchMatrix[2 + 1 * 4] =  std::sin(-pitch);
                    pitchMatrix[2 + 2 * 4] =  std::cos(-pitch);

                    mat4 yawMatrix;
                    yawMatrix.setIdentity();
                    yawMatrix[0 + 0 * 4] = std::cos(yaw);
                    yawMatrix[0 + 2 * 4] = std::sin(yaw);
                    yawMatrix[2 + 0 * 4] = -std::sin(yaw);
                    yawMatrix[2 + 2 * 4] = std::cos(yaw);

                    mat4 scaleMatrix;
                    scaleMatrix.setIdentity();
                    scaleMatrix[0 + 0 * 4] = scale.x;
                    scaleMatrix[1 + 1 * 4] = scale.y;
                    scaleMatrix[2 + 2 * 4] = scale.z;

                    mat4 translationMatrix;
                    translationMatrix.setIdentity();
                    translationMatrix[0 + 3 * 4] = pos.x;
                    translationMatrix[1 + 3 * 4] = pos.y;
                    translationMatrix[2 + 3 * 4] = pos.z;

                    _transformationMatrix = translationMatrix * scaleMatrix * yawMatrix * pitchMatrix;
                }

		void setPos(vec2 pos)
		{
			_transformationMatrix[0 + 3 * 4] = pos.x;
			_transformationMatrix[1 + 3 * 4] = pos.y;
		}

		void setPos(vec3 pos)
		{
			_transformationMatrix[0 + 3 * 4] = pos.x;
			_transformationMatrix[1 + 3 * 4] = pos.y;
			_transformationMatrix[2 + 3 * 4] = pos.z;
		}

		void setScale(vec2 scale)
		{
			_transformationMatrix[0 + 0 * 4] = scale.x;
			_transformationMatrix[1 + 1 * 4] = scale.y;
		}

		void setScale(vec3 scale)
		{
			_transformationMatrix[0 + 0 * 4] = scale.x;
			_transformationMatrix[1 + 1 * 4] = scale.y;
			_transformationMatrix[2 + 1 * 4] = scale.z;
		}

		vec3 right() const
		{	
			return vec3(
				_transformationMatrix[0 + 0 * 4],
				_transformationMatrix[1 + 0 * 4],
				_transformationMatrix[2 + 0 * 4]
			);
		}

		vec3 up() const
		{	
			return vec3(
				_transformationMatrix[0 + 1 * 4],
				_transformationMatrix[1 + 1 * 4],
				_transformationMatrix[2 + 1 * 4]
			);
		}

		vec3 forward() const
		{	
			vec3 backwards(
				_transformationMatrix[0 + 2 * 4],
				_transformationMatrix[1 + 2 * 4],
				_transformationMatrix[2 + 2 * 4]
			);
			return backwards * -1.0f;
		}

		inline vec2 getPos() const { return { _transformationMatrix[0 + 3 * 4], _transformationMatrix[1 + 3 * 4] }; }
		
		const mat4& getTransformationMatrix() const { return _transformationMatrix; }
		mat4& accessTransformationMatrix() { return _transformationMatrix; }
	};
}
