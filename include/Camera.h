#pragma once

#include "Frustrum.h"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class GpuBuffer;
class LowLevelRenderer;

class Camera
{
public:
    virtual ~Camera() = default;
    
    virtual void update( float dt );
    virtual void render( LowLevelRenderer &renderer ) = 0;
    
    glm::mat4 getProjectionMatrix() {
        return mProjectionMatrix;
    }
    Frustrum getFrustrum() {
        return mFrustrum;
    }
    
    float getNearPlane() {
        return mNearPlane;
    }
    float getFarPlane() {
        return mFarPlane;
    }
    
    void setAspect( float aspect ) {
        mAspect = aspect;
    }
    void setFOV( float fov ) {
        mFOV = fov;
    }
    void setNearPlane( float nearPlane ) {
        mNearPlane = nearPlane;
    }
    void setFarPlane( float farPlane ) {
        mFarPlane = farPlane;
    }
    
    virtual GpuBuffer* getSceneUniforms() { return nullptr; }
    virtual glm::vec3 getAmbientColor() { return glm::vec3(); }
    
private:
    glm::mat4 mProjectionMatrix;
    Frustrum mFrustrum;
    float mAspect, mFOV;
    float mNearPlane, mFarPlane;
};