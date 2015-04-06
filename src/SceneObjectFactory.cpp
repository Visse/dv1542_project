#include "SceneObjectFactory.h"
#include "DeferredEntity.h"
#include "GlmStream.h"
#include "Root.h"
#include "ResourceManager.h"
#include "ComputeParticleSystem.h"
#include "LightObject.h"
#include "RandomMovingObjects.h"
#include "PulsingObject.h"
#include "SceneManager.h"

#include <yaml-cxx/YamlCxx.h>

#include <iostream>
#include <cassert>

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
    
    throw std::runtime_error( StringUtils::strjoin("Failed to load mesh \"",meshName,"\" for entity!") );
}

SceneObject* DeferredEntityFactory::cloneObject( SceneObject *object )
{
    DeferredEntity *entity = dynamic_cast<DeferredEntity*>( object );
    assert( entity );
    
    DeferredEntity *clone = new DeferredEntity( this, mRoot, entity->getMesh(), entity->getMaterial() );
    clone->setCastShadow( entity->getCastShadow() );
    clone->setOrientation( entity->getOrientation() );
    clone->setPosition( entity->getPosition() );
    return clone;
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

SceneObject* ComputeParticleFactory::cloneObject( SceneObject *object )
{
    ComputeParticleSystem *particleSys = dynamic_cast<ComputeParticleSystem*>( object );
    assert( particleSys );
    
    ComputeParticleSystem *clone = new ComputeParticleSystem( this, mRoot );
    
    clone->setSpeed( particleSys->getSpeed() );
    clone->setDistMod( particleSys->getDistMod() );
    clone->setWeightMod( particleSys->getWeightMod() );
    clone->setLifeTime( particleSys->getLifeTime() );
    clone->setDamping( particleSys->getDamping() );
    clone->setIntensity( particleSys->getIntensity() );
    clone->setPointSize( particleSys->getPointSize() );
    clone->setParticleGroupCount( particleSys->getParticleGroupCount() );
    clone->setAttractorCount( particleSys->getAttractorCount() );
    clone->setShowAttractors( particleSys->getShowAttractors() );
    
    clone->setOrientation( particleSys->getOrientation() );
    clone->setPosition( particleSys->getPosition() );
    
    return clone;
}


LightFactory::LightFactory( Root *root ) :
    mRoot(root)
{
}

SceneObject* LightFactory::createObject( const Yaml::Node &node )
{
    Yaml::MappingNode config = node.asMapping();
    std::string lightType = config.getFirstValue("LightType",false).asValue().getValue();
    glm::vec3 color = config.getFirstValue("Color",false).asValue().getValue<glm::vec3>(glm::vec3(1.f));
    bool castShadow = config.getFirstValue("CastShadow",false).asValue().getValue<bool>(true);
    
    if( StringUtils::equalCaseInsensitive(lightType,"Point") ) {
        PointLight *light = new PointLight( this, mRoot );
        
        float innerRadius = config.getFirstValue("InnerRadius",false).asValue().getValue<float>(light->getInnerRadius());
        float outerRadius = config.getFirstValue("OuterRadius",false).asValue().getValue<float>(light->getOuterRadius());
        float intensity = config.getFirstValue("Intensity",false).asValue().getValue<float>(light->getIntensity());
        
        light->setColor( color );
        light->setOuterRadius( outerRadius );
        light->setInnerRadius( innerRadius );
        light->setIntensity( intensity );
        light->setCastShadow( castShadow );
        
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
        light->setCastShadow( castShadow );
        
        return light;
    }
    
    throw std::runtime_error( StringUtils::strjoin("Failed to create light: Unknown light type \"",lightType,"\".") );
}

SceneObject* LightFactory::cloneObject( SceneObject *object )
{
    if( PointLight *light = dynamic_cast<PointLight*>(object) ) {
        PointLight *clone = new PointLight( this, mRoot );
        
        clone->setColor( light->getColor() );
        clone->setOuterRadius( light->getOuterRadius() );
        clone->setInnerRadius( light->getInnerRadius() );
        clone->setIntensity( light->getIntensity() );
        clone->setCastShadow( light->getCastShadow() );
        clone->setPosition( light->getPosition() );
        
        return clone;
    }
    if( SpotLight *light = dynamic_cast<SpotLight*>(object) ) {
        SpotLight *clone = new SpotLight( this, mRoot );
        
        clone->setColor( light->getColor() );
        clone->setInnerAngle( light->getInnerAngle() );
        clone->setOuterAngle( light->getOuterAngle() );
        clone->setInnerDistance( light->getInnerDistance() );
        clone->setOuterDistance( light->getOuterDistance() );
        clone->setIntensity( light->getIntensity() );
        clone->setCastShadow( light->getCastShadow() );
        clone->setPosition( light->getPosition() );
        clone->setOrientation( light->getOrientation() );
        
        return clone;
    }
    
    throw std::runtime_error( "Can't clone object, not a light! (are you sure it was created by this factory?)" );
}

RandomMovingObjectFactory::RandomMovingObjectFactory( Root *root ) :
    mRoot(root)
{
}

SceneObject *RandomMovingObjectFactory::createObject( const Yaml::Node &node )
{
    Yaml::MappingNode config = node.asMapping();
    
    RandomMovingObjects *randomMovingObjects = new RandomMovingObjects( this, mRoot );
    
    float radius = config.getFirstValue("Radius", false).asValue().getValue<float>( randomMovingObjects->getRadius() );
    randomMovingObjects->setRadius( radius );
    
    float timeMultipler = config.getFirstValue("TimeMultipler", false).asValue().getValue<float>( randomMovingObjects->getTimeMultipler() );
    randomMovingObjects->setTimeMultipler( timeMultipler );
    
    Yaml::Node templateNode = config.getFirstValue( "Template", false );
    std::string templateType = templateNode.asMapping().getFirstValue("Type", false).asValue().getValue();
    
    SceneManager *sceneMgr = mRoot->getSceneManager();
    SceneObjectFactory *factory = sceneMgr->getFactory( templateType );
    
    if( !factory ) {
        throw std::runtime_error( StringUtils::strjoin("No factory for template type \"",templateType,"\"!") );
    }
    
    SceneObject *templateObject = factory->createObject( templateNode );
    randomMovingObjects->setTemplate( templateObject );
    
    unsigned int objectCount = config.getFirstValue("ObjectCount", false).asValue().getValue<unsigned int>(1);
    randomMovingObjects->setObjectCount( objectCount );
    
    return randomMovingObjects;
    
}

SceneObject *RandomMovingObjectFactory::cloneObject( SceneObject *object )
{
    RandomMovingObjects *randomMovingObjects = dynamic_cast<RandomMovingObjects*>( object );
    RandomMovingObjects *clone = new RandomMovingObjects( this, mRoot );
    
    clone->setTemplate( randomMovingObjects->getTemplate()->clone() );
    clone->setPosition( randomMovingObjects->getPosition() );
    clone->setObjectCount( randomMovingObjects->getObjectCount() );
    clone->setTimeMultipler( randomMovingObjects->getTimeMultipler() );
    
    return clone;
}

PulsingObjectFactory::PulsingObjectFactory( Root *root ) : 
    mRoot(root)
{
}

SceneObject *PulsingObjectFactory::createObject( const Yaml::Node &node )
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
        PulsingObject *object = new PulsingObject( this, mRoot, mesh, material );
        
        float maxScale = config.getFirstValue( "MaxScale", false ).asValue().getValue<float>( object->getMaxScale() );
        float minScale = config.getFirstValue( "MinScale", false ).asValue().getValue<float>( object->getMinScale() );
        
        object->setMaxScale( maxScale );
        object->setMinScale( minScale );
        
        return object;
    }
    
    throw std::runtime_error( StringUtils::strjoin("Failed to load mesh \"",meshName,"\" for entity!") );
}

SceneObject *PulsingObjectFactory::cloneObject( SceneObject *object )
{
    PulsingObject *pulsingObject = dynamic_cast<PulsingObject*>( object );
    assert( pulsingObject );
    
    PulsingObject *clone = new PulsingObject( this, mRoot, pulsingObject->getMesh(), pulsingObject->getMaterial() );
    
    clone->setMaxScale( pulsingObject->getMaxScale() );
    clone->setMinScale( pulsingObject->getMinScale() );
    
    return clone;
}






