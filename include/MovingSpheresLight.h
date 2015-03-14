#pragma once

#include "LightObject.h"

#include <vector>

class Texture;

class MovingSpheresLight :
    public PointLight
{
public:
    MovingSpheresLight( SceneObjectFactory *factory, Root *root );
        
    virtual void update( float dt ) override;
    virtual void submitRenderer( Renderer& renderer ) override;
    virtual void submitShadowCasters( Renderer& renderer ) override;
    
    unsigned int getSphereCount() {
        return mSphereCount;
    }
    float getSizeScale() {
        return mSizeScale;
    }
    float getTimeMultipler() {
        return mTimeMultiplier;
    }
    
    void setSphereCount( unsigned int sphereCount );
    void setSizeScale( float sizeScale ) {
        mSizeScale = sizeScale;
    }
    void setTimeMultipler( float timeMultiplier ) {
        mTimeMultiplier = timeMultiplier;
    }
    
private:
    struct SphereInfo {
        glm::vec3 position;
        float radius;
        
        glm::mat4 transform;
    };
    
private:
    std::vector<SphereInfo> mSpheres;
    SharedPtr<Mesh> mSphereMesh;
    
    SharedPtr<Texture> mDiffuseTexture,
                       mNormalTexture;
                       
    float mCurrentTime = 0.f,
          mTimeMultiplier = 1.0f,
          mSizeScale = 1.0f;
          
    unsigned int mSphereCount = 20;
};