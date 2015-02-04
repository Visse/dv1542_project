#include "SceneObjectFactory.h"
#include "Entity.h"
#include "GlmStream.h"
#include "Root.h"
#include "ResourceManager.h"
#include "ComputeParticleSystem.h"

#include <yaml-cxx/YamlCxx.h>

#include <iostream>

EntityFactory::EntityFactory( Root *root ) :
    mRoot(root)
{
}

SceneObject* EntityFactory::createObject( const Yaml::Node &node )
{
    Yaml::MappingNode config = node.asMapping();
    
    std::string meshName = config.getFirstValue("Mesh").asValue().getValue();
    
    ResourceManager *resourceMgr = mRoot->getResourceManager();
    
    SharedPtr<Mesh> mesh = resourceMgr->getMeshAutoPack( meshName );
    if( mesh ) {
        Entity *entity = new Entity( mesh );
        
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

