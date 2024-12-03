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

        // Don't know why previously scale was between translation and rotation..
        //_transformationMatrix = translationMatrix * scaleMatrix * yawMatrix * pitchMatrix;
        _transformationMatrix = translationMatrix * yawMatrix * pitchMatrix * scaleMatrix;

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

        _transformationMatrix = translationMat * rotationMat * scaleMat;
        _localTransformationMatrix = _transformationMatrix;
    }

    Transform::Transform(mat4 matrix) :
        Component(ComponentType::PK_TRANSFORM),
        _transformationMatrix(matrix),
        _localTransformationMatrix(matrix)
    {
    }

    Transform::Transform(const Transform& other) :
        Component(ComponentType::PK_TRANSFORM),
        _hasParent(other._hasParent)
    {
        _transformationMatrix = other._transformationMatrix;
        _localTransformationMatrix = other._localTransformationMatrix;
    }

    Transform* Transform::create(entityID_t target, vec2 pos, vec2 scale)
    {
        Scene* pScene = Application::get()->accessCurrentScene();
        Transform* pTransform = (Transform*)pScene->componentPools[ComponentType::PK_TRANSFORM].allocComponent(target);
        *pTransform = Transform(pos, scale);
        pScene->addComponent(target, pTransform);
        return pTransform;
    }

    Transform* Transform::create(entityID_t target, vec3 pos, vec3 scale)
    {
        Scene* pScene = Application::get()->accessCurrentScene();
        Transform* pTransform = (Transform*)pScene->componentPools[ComponentType::PK_TRANSFORM].allocComponent(target);
        *pTransform = Transform(pos, scale);
        pScene->addComponent(target, pTransform);
        return pTransform;
    }

    Transform* Transform::create(entityID_t target, vec3 pos, vec3 scale, float pitch, float yaw)
    {
        Scene* pScene = Application::get()->accessCurrentScene();
        Transform* pTransform = (Transform*)pScene->componentPools[ComponentType::PK_TRANSFORM].allocComponent(target);
        *pTransform = Transform(pos, scale, pitch, yaw);
        pScene->addComponent(target, pTransform);
        return pTransform;
    }

    Transform* Transform::create(entityID_t target, vec3 pos, quat rotation, vec3 scale)
    {
        Scene* pScene = Application::get()->accessCurrentScene();
        Transform* pTransform = (Transform*)pScene->componentPools[ComponentType::PK_TRANSFORM].allocComponent(target);
        *pTransform = Transform(pos, rotation, scale);
        pScene->addComponent(target, pTransform);
        return pTransform;
    }

    Transform* Transform::create(entityID_t target, mat4 matrix)
    {
        Scene* pScene = Application::get()->accessCurrentScene();
        Transform* pTransform = (Transform*)pScene->componentPools[ComponentType::PK_TRANSFORM].allocComponent(target);
        *pTransform = Transform(matrix);
        pScene->addComponent(target, pTransform);
        return pTransform;
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
        // NOTE: This might be incorrect!!!
        mat4& m = _hasParent ? _localTransformationMatrix : _transformationMatrix;

        mat4 scaleMatrix(1.0f);
        scaleMatrix[0 + 0 * 4] = scale.x;
        scaleMatrix[1 + 1 * 4] = scale.y;
        scaleMatrix[2 + 2 * 4] = scale.z;

        float currentSX = vec3(m[0 + 0 * 4], m[1 + 0 * 4], m[2 + 0 * 4]).length();
        float currentSY = vec3(m[0 + 1 * 4], m[1 + 1 * 4], m[2 + 1 * 4]).length();
        float currentSZ = vec3(m[0 + 2 * 4], m[1 + 2 * 4], m[2 + 2 * 4]).length();

        m[0 + 0 * 4] = m[0 + 0 * 4] / currentSX * scale.x;
        m[1 + 1 * 4] = m[1 + 1 * 4] / currentSY * scale.y;
        m[2 + 2 * 4] = m[2 + 2 * 4] / currentSZ * scale.z;
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

    vec3 Transform::getLocalPos() const
    {
        return {
            _localTransformationMatrix[0 + 3 * 4],
            _localTransformationMatrix[1 + 3 * 4],
            _localTransformationMatrix[2 + 3 * 4]
        };
    }

    vec3 Transform::getGlobalPos() const
    {
        return {
            _transformationMatrix[0 + 3 * 4],
            _transformationMatrix[1 + 3 * 4],
            _transformationMatrix[2 + 3 * 4]
        };
    }

    vec3 Transform::getLocalScale() const
    {
        const mat4& m = _localTransformationMatrix;
        return {
            vec3(m[0 + 0 * 4], m[1 + 0 * 4], m[2 + 0 * 4]).length(),
            vec3(m[0 + 1 * 4], m[1 + 1 * 4], m[2 + 1 * 4]).length(),
            vec3(m[0 + 2 * 4], m[1 + 2 * 4], m[2 + 2 * 4]).length()
        };
    }

    vec3 Transform::getGlobalScale() const
    {
        const mat4& m = _transformationMatrix;
        return {
            vec3(m[0 + 0 * 4], m[1 + 0 * 4], m[2 + 0 * 4]).length(),
            vec3(m[0 + 1 * 4], m[1 + 1 * 4], m[2 + 1 * 4]).length(),
            vec3(m[0 + 2 * 4], m[1 + 2 * 4], m[2 + 2 * 4]).length()
        };
    }
}
