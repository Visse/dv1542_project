#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

void Camera::update( float dt )
{
    mProjectionMatrix = glm::perspective( 
        mFOV, mAspect, mNearPlane, mFarPlane
    );
}
