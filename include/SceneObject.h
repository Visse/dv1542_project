#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

class LowLevelRenderer;
class Renderable;


class SceneObject {
public:
    virtual ~SceneObject() = default;
   
    virtual void update( float dt ) {}
    
    virtual void queueRenderable( LowLevelRenderer &renderer ) {}
    
    void setPosition( const glm::vec3 &position );
    void setOrientation( const glm::quat &orientation );
    void setRenderQueue( unsigned int queue ) {
        mRenderQueue = queue;
    }
    const glm::vec3& getPosition() {
        return mPosition;
    }
    const glm::quat& getOrientation() {
        return mOrientation;
    }
    const glm::mat4& getTransform() {
        return mTransform;
    }
    unsigned int getRenderQueue() {
        return mRenderQueue;
    }
    
    bool isDirty() {
        return mDirty;
    }
    void _updateTransform();
    
private:
    glm::vec3 mPosition;
    glm::quat mOrientation;
    
    glm::mat4 mTransform;
    
    unsigned int mRenderQueue = 0;
    bool mDirty = false;
};