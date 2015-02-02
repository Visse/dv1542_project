#pragma once

#include "SceneObject.h"
#include "Renderable.h"

#include "VertexArrayObject.h"
#include "SharedPtr.h"

#include <vector>

class Root;
class ComputeParticleSystem;
class GpuProgram;
class GpuBuffer;


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
    uint getParticleGroupCount() {
        return mParticleGroupCount;
    }
    uint getMaxParticleGroupCount() {
        return mMaxParticleGroupCount;
    }
    uint getAttractorCount() {
        return mAttractorCount;
    }
    uint getMaxAttractorCount() {
        return mMaxAttractorCount;
    }
    bool getShowAttractors() {
        return mShowAttractors;
    }
    uint getParticleCount() {
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
    void setParticleGroupCount( uint groupCount ) {
        mParticleGroupCount = groupCount;
    }
    void setAttractorCount( uint attractorCount ) {
        mAttractorCount = attractorCount;
    }
    void setShowAttractors( bool showAttractors ) {
        mShowAttractors = showAttractors;
    }
private:
    struct ParticleSystemRenderer :
        public Renderable
    {
        ComputeParticleSystem *particleSys;
        virtual void render() override;
    };
    struct AttractorRenderer :
        public Renderable
    {
        ComputeParticleSystem *particleSys;
        virtual void render() override;
    };
    
    
private:
    Root *mRoot;
    ParticleSystemRenderer mRenderer;
    AttractorRenderer mAttractorRenderer;
    
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
    struct {
        GLint modelMatrix, intensity, pointSize;
    } mRenderingLoc;
    struct {
        GLint modelMatrix = -1;
    } mAttractorRenderingLoc;
    
    std::vector<float> mAttractorWeights;
    
    float mCurrentTime = 0.f,
          mSpeed = 1.f,
          mDistMod = 5.f,
          mWeightMod = 7.f,
          mLifeTime = 10.f,
          mDamping = 0.25f;
    float mIntensity = 1.f,
          mPointSize = 5.f;
          
    uint mParticleGroupCount,
         mParticleGroupSize,
         mMaxParticleGroupCount,
         mAttractorCount,
         mMaxAttractorCount;
    
    bool mShowAttractors = false;
};