#pragma once

#include "SceneObject.h"

#include "VertexArrayObject.h"
#include "SharedPtr.h"

#include <vector>

class Root;
class GpuProgram;
class GpuBuffer;
class Material;


class ComputeParticleSystem :
    public SceneObject
{
public:
    ComputeParticleSystem( Root *root );
    virtual void update( float dt ) override;
    
    virtual void queueRenderable( LowLevelRenderer &renderer ) override;
    
    float getSpeed() {
        return mSpeed;
    }
    float getDistMod() {
        return mDistMod;
    }
    float getWeightMod() {
        return mWeightMod;
    }
    float getLifeTime() {
        return mLifeTime;
    }
    float getDamping() {
        return mDamping;
    }
    float getIntensity() {
        return mIntensity;
    }
    float getPointSize() {
        return mPointSize;
    }
    unsigned int getParticleGroupCount() {
        return mParticleGroupCount;
    }
	unsigned int getMaxParticleGroupCount() {
        return mMaxParticleGroupCount;
    }
	unsigned int getAttractorCount() {
        return mAttractorCount;
    }
	unsigned int getMaxAttractorCount() {
        return mMaxAttractorCount;
    }
    bool getShowAttractors() {
        return mShowAttractors;
    }
	unsigned int getParticleCount() {
        return mParticleGroupCount * mParticleGroupSize;
    }
    
    void setSpeed( float speed ) {
        mSpeed = speed;
    }
    void setDistMod( float distMod ) {
        mDistMod = distMod;
    }
    void setWeightMod( float weightMod ) {
        mWeightMod = weightMod;
    }
    void setLifeTime( float lifeTime ) {
        mLifeTime = lifeTime;
    }
    void setDamping( float damping ) {
        mDamping = damping;
    }
    void setIntensity( float intensity ) {
        mIntensity = intensity;
    }
    void setPointSize( float pointSize ) {
        mPointSize = pointSize;
    }
    void setParticleGroupCount(unsigned int groupCount) {
        mParticleGroupCount = groupCount;
    }
    void setAttractorCount(unsigned int attractorCount) {
        mAttractorCount = attractorCount;
    }
    void setShowAttractors( bool showAttractors ) {
        mShowAttractors = showAttractors;
    }
    
private:
    struct RenderingUniformBlock {
        glm::mat4 modelMatrix;
        glm::vec2 intensityAndSize;
    };
    struct AttractorUniformBlock {
        glm::mat4 modelMatrix;
    };
    
private:
    Root *mRoot;
    
    VertexArrayObject mVAO, mAttractorVAO;
    SharedPtr<GpuProgram> mSimulation,
                          mShader;
    SharedPtr<GpuBuffer> mParticleBuffer,
                         mStartPositionBuffer,
                         mAttractorBuffer;
    SharedPtr<Material> mMaterial, mAttractorMaterial;
    
    struct {
        GLint dt, distMod, weightMod, lifeTime,
              damping, attractorCount, modelMatrix;
    } mSimulationLoc;

    size_t mRenderingUniformLoc,
           mAttractorUniformLoc;
    
    
    std::vector<float> mAttractorWeights;
    
    float mCurrentTime = 0.f,
          mSpeed = 1.f,
          mDistMod = 5.f,
          mWeightMod = 7.f,
          mLifeTime = 10.f,
          mDamping = 0.25f;
    float mIntensity = 1.f,
          mPointSize = 5.f;
          
	unsigned int mParticleGroupCount,
                 mParticleGroupSize,
                 mMaxParticleGroupCount,
                 mAttractorCount,
                 mMaxAttractorCount;
    
    bool mShowAttractors = false;
};