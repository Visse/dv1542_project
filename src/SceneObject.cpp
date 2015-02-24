#include "SceneObject.h"

#include <glm/gtx/transform.hpp>


void SceneObject::_updateTransform()
{
    mTransform = glm::translate(mPosition) * glm::mat4_cast(mOrientation);
    glm::vec3 center( mTransform * glm::vec4(mBoundingSphere.getCenter(),1.0f) );
    mTransformedBoundingSphere = BoundingSphere( center, mBoundingSphere.getRadius() );
    
    mDirty = false;
}



