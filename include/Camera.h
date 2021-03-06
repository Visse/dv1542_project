#pragma once

#include "Frustrum.h"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

struct SceneRenderUniforms;
class GpuBuffer;
class Renderer;

class Camera
{
public:
    virtual ~Camera() = default;
    
    virtual void update( float dt );
    virtual void render( Renderer &renderer ) = 0;
    virtual SceneRenderUniforms getSceneUniforms() = 0;
    
    
    glm::mat4 getProjectionMatrix() {
        return mProjectionMatrix;
    }
    glm::mat4 getViewMatrix() {
        return mViewMatrix;
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
    void setViewMatrix( const glm::mat4 &mat ) {
        mViewMatrix = mat;
    }
    
private:
    glm::mat4 mProjectionMatrix, mViewMatrix;
    Frustrum mFrustrum;
    float mAspect, mFOV;
    float mNearPlane, mFarPlane;
};