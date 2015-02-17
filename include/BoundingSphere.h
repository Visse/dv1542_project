#pragma once

#include <glm/vec3.hpp>

class BoundingSphere {
public:
    BoundingSphere() = default;
    BoundingSphere( const BoundingSphere& ) = default;
    BoundingSphere& operator = ( const BoundingSphere& ) = default;
    
public:
    BoundingSphere( const glm::vec3 &center, float radius ) :
        mCenter(center),
        mRadius(radius)
    {}
    
    const glm::vec3& getCenter() const {
        return mCenter;
    }
    float getRadius() const {
        return mRadius;
    }

private:
    glm::vec3 mCenter;
    float mRadius = 0.f;
};