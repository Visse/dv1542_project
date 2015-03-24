#include "SceneObjectFactory.h"
#include "DeferredEntity.h"
#include "GlmStream.h"
#include "Root.h"
#include "ResourceManager.h"
#include "ComputeParticleSystem.h"
#include "LightObject.h"

#include "MovingSpheresLight.h"
#include "RandomMovingPointLight.h"

#include <yaml-cxx/YamlCxx.h>

#include <iostream>

void SceneObjectFactory::destroyObject( SceneObject *object )
{
    delete object;
}

DeferredEntityFactory::DeferredEntityFactory( Root *root ) :
    mRoot(root)
{
}

SceneObject* DeferredEntityFactory::createObject( const Yaml::Node &node )
{
    Yaml::MappingNode config = node.asMapping();
    
    std::string meshName = config.getFirstValue("Mesh",false).asValue().getValue();
    
    std::string diffuseName = config.getFirstValue("DiffuseTexture",false).asValue().getValue();
    std::string normalMapName = config.getFirstValue("NormalMap",false).asValue().getValue();
    
    ResourceManager *resourceMgr = mRoot->getResourceManager();
    
    SharedPtr<Mesh> mesh = resourceMgr->getMeshAutoPack( meshName );
    
    DeferredMaterial material;
    material.diffuseTexture = resourceMgr->getTextureAutoPack( diffuseName );
    material.normalMap = resourceMgr->getTextureAutoPack( normalMapName );
    
    if( mesh ) {
        DeferredEntity *entity = new DeferredEntity( this, mRoot, mesh, material );
        
        return entity;
    }
    /// @todo add proper logging
    std::cerr << "Failed to load mesh \"" << meshName << "\" for entity!" << std::endl;
    return nullptr;
}

ComputeParticleFactory::ComputeParticleFactory( Root *root ) :
    mRoot(root)
{
}

SceneObject* ComputeParticleFactory::createObject( const Yaml::Node &node )
{
    Yaml::MappingNode config = node.asMapping();
    
    ComputeParticleSystem *particleSys = new ComputeParticleSystem( this, mRoot );

    return particleSys;
}

LightFactory::LightFactory( Root *root ) :
    mRoot(root)
{
}

SceneObject *LightFactory::createObject( const Yaml::Node &node )
{
    Yaml::MappingNode config = node.asMapping();
    std::string lightType = config.getFirstValue("LightType",false).asValue().getValue();
    glm::vec3 color = config.getFirstValue("Color",false).asValue().getValue<glm::vec3>(glm::vec3(1.f));
    
    if( StringUtils::equalCaseInsensitive(lightType,"Point") ) {
        PointLight *light = new PointLight( this, mRoot );
        
        float innerRadius = config.getFirstValue("InnerRadius",false).asValue().getValue<float>(light->getInnerRadius());
        float outerRadius = config.getFirstValue("OuterRadius",false).asValue().getValue<float>(light->getOuterRadius());
        float intensity = config.getFirstValue("Intensity",false).asValue().getValue<float>(light->getIntensity());
        
        light->setColor( color );
        light->setOuterRadius( outerRadius );
        light->setInnerRadius( innerRadius );
        light->setIntensity( intensity );
        
        return light;
    }
    if( StringUtils::equalCaseInsensitive(lightType,"Spot") ) {
        SpotLight *light = new SpotLight( this, mRoot );
        
        float innerAngle = config.getFirstValue("InnerAngle",false).asValue().getValue<float>(light->getInnerAngle());
        float outerAngle = config.getFirstValue("OuterAngle",false).asValue().getValue<float>(light->getOuterAngle());
        float innerDistance = config.getFirstValue("InnerDistance",false).asValue().getValue<float>(light->getInnerDistance());
        float outerDistance = config.getFirstValue("OuterDistance",false).asValue().getValue<float>(light->getOuterDistance());
        float intensity = config.getFirstValue("Intensity",false).asValue().getValue<float>(light->getIntensity());
        
        light->setColor( color );
        light->setInnerAngle( innerAngle );
        light->setOuterAngle( outerAngle );
        light->setInnerDistance( innerDistance );
        light->setOuterDistance( outerDistance );
        light->setIntensity( intensity );
        
        return light;
    }
    if( StringUtils::equalCaseInsensitive(lightType,"MovingSpheres") ) {
        MovingSpheresLight *light = new MovingSpheresLight( this, mRoot );
        
        float innerRadius = config.getFirstValue("InnerRadius",false).asValue().getValue<float>(light->getInnerRadius());
        float outerRadius = config.getFirstValue("OuterRadius",false).asValue().getValue<float>(light->getOuterRadius());
        float intensity = config.getFirstValue("Intensity",false).asValue().getValue<float>(light->getIntensity());
        
        light->setColor( color );
        light->setOuterRadius( outerRadius );
        light->setInnerRadius( innerRadius );
        light->setIntensity( intensity );
        
        return light;
    }
    else if( StringUtils::equalCaseInsensitive(lightType,"RandomMovingPoint") ) {
        
        RandomMovingPointLight *light = new RandomMovingPointLight( this, mRoot );
        
        float innerRadius = config.getFirstValue("InnerRadius",false).asValue().getValue<float>(light->getInnerRadius());
        float outerRadius = config.getFirstValue("OuterRadius",false).asValue().getValue<float>(light->getOuterRadius());
        float intensity = config.getFirstValue("Intensity",false).asValue().getValue<float>(light->getIntensity());
        
        light->setColor( color );
        light->setOuterRadius( outerRadius );
        light->setInnerRadius( innerRadius );
        light->setIntensity( intensity );
        
        return light;
    }
    
    /// @todo add proper logging
    std::cerr << "Failed to create light: Unknown light type \"" << lightType << "\".";
    return nullptr;
}
