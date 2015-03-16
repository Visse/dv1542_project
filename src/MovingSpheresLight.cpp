#include "MovingSpheresLight.h"
#include "Renderer.h"
#include "Root.h"
#include "ResourceManager.h"

#include <glm/gtc/random.hpp>

MovingSpheresLight::MovingSpheresLight( SceneObjectFactory *factory, Root *root ) :
    PointLight( factory, root )
{
    // this is so its respecting its default-constructor assignment
    unsigned int count = mSphereCount;
    mSphereCount = 0;
    setSphereCount( count );
    
    ResourceManager *resourceMgr = root->getResourceManager();
    mSphereMesh = resourceMgr->getMeshAutoPack("LowPolySphere");
    mDiffuseTexture = resourceMgr->getTextureAutoPack("LavaDiffuseImage");
    mNormalTexture = resourceMgr->getTextureAutoPack("LavaNormalImage");
}

void MovingSpheresLight::update( float dt )
{
    mCurrentTime += dt * mTimeMultiplier;
    
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
        info.transform = glm::scale( glm::translate(glm::mat4(), info.position), glm::vec3(info.radius*glm::length(info.position)*mSizeScale) );
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

void MovingSpheresLight::setSphereCount( unsigned int sphereCount )
{
    unsigned int prevCount = mSphereCount;
    mSphereCount = sphereCount;
    mSpheres.resize(  mSphereCount );
    
    for( unsigned int i=prevCount; i < mSphereCount; ++i ) {
        mSpheres[i].radius = glm::linearRand( 0.02f, 0.15f );
    }
}



