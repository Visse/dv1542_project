#pragma once
#include <glm/mat4x4.hpp>


class Controller {
public:
    virtual ~Controller() = default;
    
    virtual void update( float dt ) = 0;
    
    virtual glm::mat4 getTransformation() = 0;
};