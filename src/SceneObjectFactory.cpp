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
        float outerRadius = config.getFirstValue("OuterRadius",false).asValue().getValue<float>(1.f);
        float innerRadius = config.getFirstValue("InnerRadius",false).asValue().getValue<float>(0.f);
        
        PointLight *light = new PointLight( mRoot );
        light->setColor( color );
        light->setOuterRadius( outerRadius );
        light->setInnerRadius( innerRadius );
        
        return light;
    }
    /// @todo add proper logging
    std::cerr << "Failed to create light: Unknown light type \"" << lightType << "\".";
    return nullptr;
}


