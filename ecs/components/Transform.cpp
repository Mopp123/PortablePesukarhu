#include "Transform.h"
#include "core/Application.h"


namespace pk
{

    Transform::Transform(vec2 pos, vec2 scale) :
        Component(ComponentType::PK_TRANSFORM)
    {
        _transformationMatrix.setIdentity();
        _localTransformationMatrix.setIdentity();

        setPos(pos);
        setScale(scale);
    }

    Transform::Transform(vec3 pos, vec3 scale) :
        Component(ComponentType::PK_TRANSFORM)
    {
        _transformationMatrix.setIdentity();
        _localTransformationMatrix.setIdentity();
        setPos(pos);
        setScale(scale);
    }

    Transform::Transform(vec3 pos, vec3 scale, float pitch, float yaw) :
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
        // Not sure if to do below..?
        _localTransformationMatrix = _transformationMatrix;
    }

    Transform::Transform(vec3 pos, quat rotation, vec3 scale) :
        Component(ComponentType::PK_TRANSFORM)
    {
        mat4 translationMat(1.0f);
        translationMat[0 + 3 * 4] = pos.x;
        translationMat[1 + 3 * 4] = pos.y;
        translationMat[2 + 3 * 4] = pos.z;

        mat4 rotationMat = rotation.toRotationMatrix();

        mat4 scaleMat(1.0f);
        scaleMat[0 + 0 * 4] = scale.x;
        scaleMat[1 + 1 * 4] = scale.y;
        scaleMat[2 + 2 * 4] = scale.z;

        _transformationMatrix = translationMat * rotationMat;
        _localTransformationMatrix = _transformationMatrix;
    }

    Transform::Transform(mat4 matrix, mat4 inverseBindMatrix) :
        Component(ComponentType::PK_TRANSFORM),
        _transformationMatrix(matrix),
        _localTransformationMatrix(matrix)
    {
        if (inverseBindMatrix != mat4(0.0f))
        {
            _isJoint = true;
            _inverseBindMatrix = inverseBindMatrix;
        }
    }

    Transform::Transform(const Transform& other) :
        Component(ComponentType::PK_TRANSFORM),
        _hasParent(other._hasParent)
    {
        _transformationMatrix = other._transformationMatrix;
        _localTransformationMatrix = other._localTransformationMatrix;
    }

    void Transform::setPos(vec2 pos)
    {
        mat4& mat = _hasParent ? _localTransformationMatrix : _transformationMatrix;
        mat[0 + 3 * 4] = pos.x;
        mat[1 + 3 * 4] = pos.y;
    }

    void Transform::setPos(vec3 pos)
    {
        mat4& mat = _hasParent ? _localTransformationMatrix : _transformationMatrix;
        mat[0 + 3 * 4] = pos.x;
        mat[1 + 3 * 4] = pos.y;
        mat[2 + 3 * 4] = pos.z;
    }

    void Transform::setRotation(float pitch, float yaw, float roll)
    {
        mat4 rotationMatrix = create_rotation_matrix(pitch, yaw, roll);
        //_transformationMatrix = _transformationMatrix * rotationMatrix;

        mat4& mat = _hasParent ? _localTransformationMatrix : _transformationMatrix;

	      mat[1 + 1 * 4] = rotationMatrix[1 + 1 * 4];
	      mat[1 + 2 * 4] = rotationMatrix[1 + 2 * 4];
	      mat[2 + 1 * 4] = rotationMatrix[2 + 1 * 4];
	      mat[2 + 2 * 4] = rotationMatrix[2 + 2 * 4];

	      mat[0 + 0 * 4] = rotationMatrix[0 + 0 * 4];
	      mat[0 + 2 * 4] = rotationMatrix[0 + 2 * 4];
	      mat[2 + 0 * 4] = rotationMatrix[2 + 0 * 4];
	      mat[2 + 2 * 4] = rotationMatrix[2 + 2 * 4];

	      mat[0 + 0 * 4] = rotationMatrix[0 + 0 * 4];
	      mat[0 + 1 * 4] = rotationMatrix[0 + 1 * 4];
	      mat[1 + 0 * 4] = rotationMatrix[1 + 0 * 4];
	      mat[1 + 1 * 4] = rotationMatrix[1 + 1 * 4];
    }

    void Transform::setRotation(const quat& rotation)
    {
        mat4 rotationMatrix = rotation.toRotationMatrix();
        mat4& mat = _hasParent ? _localTransformationMatrix : _transformationMatrix;

	      mat[1 + 1 * 4] = rotationMatrix[1 + 1 * 4];
	      mat[1 + 2 * 4] = rotationMatrix[1 + 2 * 4];
	      mat[2 + 1 * 4] = rotationMatrix[2 + 1 * 4];
	      mat[2 + 2 * 4] = rotationMatrix[2 + 2 * 4];

	      mat[0 + 0 * 4] = rotationMatrix[0 + 0 * 4];
	      mat[0 + 2 * 4] = rotationMatrix[0 + 2 * 4];
	      mat[2 + 0 * 4] = rotationMatrix[2 + 0 * 4];
	      mat[2 + 2 * 4] = rotationMatrix[2 + 2 * 4];

	      mat[0 + 0 * 4] = rotationMatrix[0 + 0 * 4];
	      mat[0 + 1 * 4] = rotationMatrix[0 + 1 * 4];
	      mat[1 + 0 * 4] = rotationMatrix[1 + 0 * 4];
	      mat[1 + 1 * 4] = rotationMatrix[1 + 1 * 4];
    }

    void Transform::rotate(float pAmount, float yAmount, float rAmount)
    {
        mat4& mat = _hasParent ? _localTransformationMatrix : _transformationMatrix;
        mat = mat * create_rotation_matrix(pAmount, yAmount, rAmount);
    }

    void Transform::setScale(vec2 scale)
    {

        mat4& mat = _hasParent ? _localTransformationMatrix : _transformationMatrix;
        mat[0 + 0 * 4] = scale.x;
        mat[1 + 1 * 4] = scale.y;
    }

    void Transform::setScale(vec3 scale)
    {
        Debug::notify_unimplemented("Transform::setScale(2)");
        return;

        _transformationMatrix[0 + 0 * 4] = scale.x;
        _transformationMatrix[1 + 1 * 4] = scale.y;
        _transformationMatrix[2 + 1 * 4] = scale.z;
    }

    vec3 Transform::right() const
    {
        return vec3(
                _transformationMatrix[0 + 0 * 4],
                _transformationMatrix[1 + 0 * 4],
                _transformationMatrix[2 + 0 * 4]
                );
    }

    vec3 Transform::up() const
    {
        return vec3(
                _transformationMatrix[0 + 1 * 4],
                _transformationMatrix[1 + 1 * 4],
                _transformationMatrix[2 + 1 * 4]
                );
    }

    vec3 Transform::forward() const
    {
        vec3 backwards(
                _transformationMatrix[0 + 2 * 4],
                _transformationMatrix[1 + 2 * 4],
                _transformationMatrix[2 + 2 * 4]
                );
        return backwards * -1.0f;
    }
}
