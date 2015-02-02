#include "SceneObject.h"

#include <glm/gtx/transform.hpp>

void SceneObject::setPosition( const glm::vec3 &position )
{
    mPosition = position;
    mDirty = true;
}

void SceneObject::setOrientation( const glm::quat &orientation )
{
    mOrientation = orientation;
    mDirty = true;
}

void SceneObject::_updateTransform()
{
    mTransform = glm::translate(mPosition) * glm::mat4_cast(mOrientation);
    mDirty = false;
}



