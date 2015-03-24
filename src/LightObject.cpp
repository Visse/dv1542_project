#include "LightObject.h"
#include "Mesh.h"
#include "Root.h"
#include "ResourceManager.h"
#include "Material.h"
#include "GpuProgram.h"
#include "GLinclude.h"
#include "UniformBlockDefinitions.h"
#include "Renderer.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

PointLight::PointLight( SceneObjectFactory* factory, Root *root ) :
    LightObject(factory),
    mRoot(root)
{
    setCastShadow( true );
}

void PointLight::submitRenderer( Renderer &renderer )
{
    PointLightUniforms uniforms;
        uniforms.color = getColor();
        uniforms.intensity = mIntensity;
        uniforms.modelMatrix = glm::scale( getTransform(), glm::vec3(mOuterRadius) );
        uniforms.radius = glm::vec2(mInnerRadius,mOuterRadius);
        
    renderer.addPointLight( renderer.aquireUniformBuffer(uniforms), getTransform(), getPosition(), mOuterRadius, getCastShadow() );
}




SpotLight::SpotLight( SceneObjectFactory* factory, Root *root ) :
    LightObject(factory),
    mRoot(root)
{
}

void SpotLight::updateBounds()
{
    float h = 0.5f*mOuterDistance,
          w = mOuterDistance*glm::tan(mOuterAngle);
    
    float radius = glm::sqrt(h*h+w*w);
    
    setBoundingSphere( BoundingSphere(glm::vec3(0,0,-h),radius) );
}
