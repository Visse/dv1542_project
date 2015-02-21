#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "BoundingSphere.h"

class Frustrum {
public:
    static Frustrum FromProjectionMatrix( const glm::mat4 &proj ) {
        Frustrum f;
        
        glm::mat4 tmp = glm::transpose(proj);
        
        f.mPlanes[0] = (tmp[3] + tmp[0]);
        f.mPlanes[1] = (tmp[3] - tmp[0]);
        
        f.mPlanes[2] = (tmp[3] + tmp[1]);
        f.mPlanes[3] = (tmp[3] - tmp[1]);
        
        f.mPlanes[4] = (tmp[3] + tmp[2]);
        f.mPlanes[5] = (tmp[3] - tmp[2]);
        
        for( int i=0; i < 6; ++i ) {
            f.mPlanes[i] /= glm::length(glm::vec3(f.mPlanes[i]) );
        }
        
        return f;
    }
    
public:
    
    enum class TestStatus {
        Outside,
        Intersecting,
        Inside
    };
    
    bool isInside( const BoundingSphere &bounds, const glm::mat4 &transform ) const
    {
        glm::vec4 center = transform * glm::vec4( bounds.getCenter(), 1.0 );
        
        float radius = bounds.getRadius();
        
        float distances[6];
        for( int i=0; i < 6; ++i ) {
            distances[i] = glm::dot(mPlanes[i], center);
        }
        
        for( int i=0; i < 6; ++i ) {
            if( distances[i] < -radius ) {
                return false;
            }
        }
        
        return true;
    }
    
private:
    glm::vec4 mPlanes[6];
};