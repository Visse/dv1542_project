#include "RandomMovingPointLight.h"

void RandomMovingPointLight::update( float dt )
{
    mCurrentTime += dt;
    mCurrentTime = glm::mod( mCurrentTime, 100.f );
    glm::vec3 offset = glm::vec3(
            glm::sin(mCurrentTime*2*0.06f)*glm::cos(mCurrentTime*7*0.04f)*0.5f,
            glm::sin(mCurrentTime*3*0.04f)*glm::sin(mCurrentTime*5*0.02f)*0.5f,
            glm::cos(mCurrentTime*5*0.02f)*glm::cos(mCurrentTime*9*0.06f)*0.5f
    );
    
    glm::vec3 pos = getPosition() - mPrevOffset;
    pos += offset;
    mPrevOffset = offset;
    setPosition( pos );
}
