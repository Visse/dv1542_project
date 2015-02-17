#include "SceneObjectFactory.h"
#include "Entity.h"
#include "GlmStream.h"
#include "Root.h"
#include "ResourceManager.h"
#include "ComputeParticleSystem.h"
#include "LightObject.h"

#include <yaml-cxx/YamlCxx.h>

#include <iostream>

EntityFactory::EntityFactory( Root *root ) :
    mRoot(root)
{
}

SceneObject* EntityFactory::createObject( const Yaml::Node &node )
{
    Yaml::MappingNode config = node.asMapping();
    
    std::string meshName = config.getFirstValue("Mesh",false).asValue().getValue();
    
    ResourceManager *resourceMgr = mRoot->getResourceManager();
    
    SharedPtr<Mesh> mesh = resourceMgr->getMeshAutoPack( meshName );
    if( mesh ) {
        Entity *entity = new Entity( mRoot, mesh );
        
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
    
    ComputeParticleSystem *particleSys = new ComputeParticleSystem( mRoot );

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
        PointLight *light = new PointLight( mRoot );
        
        float innerRadius = config.getFirstValue("InnerRadius",false).asValue().getValue<float>(light->getInnerRadius());
        float outerRadius = config.getFirstValue("OuterRadius",false).asValue().getValue<float>(light->getOuterRadius());
        float intensity = config.getFirstValue("Intensity",false).asValue().getValue<float>(light->getIntensity());
        
        light->setColor( color );
        light->setOuterRadius( outerRadius );
        light->setInnerRadius( innerRadius );
        light->setIntensity( intensity );
        
        return light;
    }
    if( StringUtils::equalCaseInsensitive(lightType,"Ambient") ) {
        AmbientLight *light = new AmbientLight( mRoot );
        light->setColor( color );
        
        return light;
    }
    if( StringUtils::equalCaseInsensitive(lightType,"Spot") ) {
        SpotLight *light = new SpotLight( mRoot );
        
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
    if( StringUtils::equalCaseInsensitive(lightType,"Box") ) {
        BoxLight *light = new BoxLight( mRoot );
        
        glm::vec3 innerSize = config.getFirstValue("InnerSize",false).asValue().getValue<glm::vec3>(light->getInnerSize());
        glm::vec3 outerSize = config.getFirstValue("OuterSize",false).asValue().getValue<glm::vec3>(light->getOuterSize());
        float intensity = config.getFirstValue("Intensity",false).asValue().getValue<float>(light->getIntensity());
        
        light->setColor( color );
        light->setInnerSize( innerSize );
        light->setOuterSize( outerSize );
        light->setIntensity( intensity );
        
        return light;
    }
    
    /// @todo add proper logging
    std::cerr << "Failed to create light: Unknown light type \"" << lightType << "\".";
    return nullptr;
}


