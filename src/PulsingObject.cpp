#include "PulsingObject.h"
#include "Mesh.h"

#include <glm/gtc/random.hpp>

PulsingObject::PulsingObject( SceneObjectFactory *factory, Root *root, const SharedPtr< Mesh > &mesh, const DeferredMaterial &material ) : 
    DeferredEntity( factory, root, mesh, material )
{
    mOffset = glm::linearRand( 0.f, glm::pi<float>()*2.f );
}

void PulsingObject::update( float dt )
{
    SceneObject::update( dt );
    
    mOffset += dt;
    mOffset = glm::mod( mOffset, glm::pi<float>()*2.f );
    mScale = glm::sin(mOffset) * (mMaxScale - mMinScale) + mMinScale;
    
    BoundingSphere bounds = getMesh()->getBoundingSphere();
    
    // not correct if bounds.getCenter() != (0,0,0)
    setBoundingSphere( BoundingSphere(glm::vec3(),bounds.getRadius()*mScale) );
}

void PulsingObject::submitRenderer( Renderer &renderer )
{
    renderer.addMesh( getMesh(), getMaterial(), glm::scale(getTransform(),glm::vec3(mScale)) );
}

void PulsingObject::submitShadowCasters(Renderer& renderer)
{
    if( getCastShadow() ) {
        renderer.addShadowMesh( getMesh(), glm::scale(getTransform(),glm::vec3(mScale)) );
    }
}


