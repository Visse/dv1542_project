#include "MovingSpheresLight.h"
#include "Renderer.h"
#include "Root.h"
#include "ResourceManager.h"

#include <glm/gtc/random.hpp>

MovingSpheresLight::MovingSpheresLight( SceneObjectFactory *factory, Root *root ) :
    PointLight( factory, root )
{
    
    mSpheres.resize( 20 );
    
    for( SphereInfo &info : mSpheres ) {
        info.radius = glm::linearRand( 0.02f, 0.15f );
    }
    
    ResourceManager *resourceMgr = root->getResourceManager();
    mSphereMesh = resourceMgr->getMeshAutoPack("Sphere");
    mDiffuseTexture = resourceMgr->getTextureAutoPack("LavaDiffuseImage");
    mNormalTexture = resourceMgr->getTextureAutoPack("LavaNormalImage");
}

void MovingSpheresLight::update( float dt )
{
    mCurrentTime += dt;
    
    float index = 0;
    for( SphereInfo &info : mSpheres ) {
        info.position = glm::vec3( 
            glm::sin( mCurrentTime*(index+2)*0.06f+index*0.2f)*glm::cos( mCurrentTime*(index+7)*0.04f ),
            glm::sin( mCurrentTime*(index+3)*0.04f+index*0.5f)*glm::sin( mCurrentTime*(index+5)*0.02f ),
            glm::cos( mCurrentTime*(index+5)*0.02f+index*0.8f)*glm::cos( mCurrentTime*(index+9)*0.06f )
        );
        
        index += 1.0;
    }
    for( SphereInfo &info : mSpheres ) {
        info.transform = glm::scale( glm::translate(glm::mat4(), info.position), glm::vec3(info.radius*glm::length(info.position)) );
    }
}

void MovingSpheresLight::submitRenderer( Renderer &renderer )
{
    PointLight::submitRenderer( renderer );
    
    DeferredMaterial material;
        material.diffuseTexture = mDiffuseTexture;
        material.normalMap = mNormalTexture;
        
    for( const SphereInfo &info : mSpheres ) {
        renderer.addMesh( mSphereMesh, material, getTransform() * info.transform );
    }
}

void MovingSpheresLight::submitShadowCasters( Renderer &renderer )
{
    for( const SphereInfo &info : mSpheres ) {
        renderer.addShadowMesh( mSphereMesh, getTransform() * info.transform );
    }
}


