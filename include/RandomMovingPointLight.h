#include "LightObject.h"


class RandomMovingPointLight :
    public PointLight
{
public:
    using PointLight::PointLight;
    
    virtual void update( float dt );
    
    
private:
    float mCurrentTime;
    glm::vec3 mPrevOffset;
};