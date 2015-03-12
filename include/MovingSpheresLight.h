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
                       
    float mCurrentTime = 0.f;
};