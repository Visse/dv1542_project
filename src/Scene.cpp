#include "Scene.h"
#include "SceneObject.h"
#include "Root.h"
#include "ResourceManager.h"
#include "Entity.h"
#include "GlmStream.h"
#include "SceneManager.h"
#include "SceneObjectFactory.h"
#include "RenderQueue.h"


#include "yaml-cxx/YamlCxx.h"

#include <cassert>
#include <algorithm>

#include <iostream>

struct Scene::FindSceneInfo {
    SceneObject *object;
    
    bool operator () ( const Scene::ObjectInfo &info ) const {
        return object == info.object;
    }
};

SharedPtr<Scene> Scene::LoadFromFile( Root *root, const std::string &filename )
{
    Yaml::MappingNode sceneCfg = Yaml::Node::LoadFromFile(filename).asMapping();
    
    SharedPtr<Scene> scene = makeSharedPtr<Scene>(root);
    
    SceneManager *sceneMgr = root->getSceneManager();
    ResourceManager *resourceMgr = root->getResourceManager();
    
    auto resourcePackList = sceneCfg.getValues("ResourcePack");
    for( Yaml::Node packNode : resourcePackList ) {
        resourceMgr->loadResourcePack( packNode.asValue().getValue() );
    }
    
    
    auto objectList = sceneCfg.getValues("Object");
    for( Yaml::Node objectNode : objectList ) {
        Yaml::MappingNode config = objectNode.asMapping();
        std::string type = config.getFirstValue("Type").asValue().getValue();
        
        SceneObjectFactory *factory = sceneMgr->getFactory( type );
        if( factory ) {
            SceneObject *object = factory->createObject( objectNode );
            
            if( object ) {
                Yaml::ValueNode positionNode = config.getFirstValue("Position",false).asValue(),
                                orientationNode = config.getFirstValue("Orientation",false).asValue(),
                                renderqueueNode = config.getFirstValue("RenderQueue",false).asValue();
                bool succes;
                
                glm::vec3 position = positionNode.getValue<glm::vec3>(&succes);
                if( succes ) {
                    object->setPosition( position );
                }
                
                glm::vec3 orientation = orientationNode.getValue<glm::vec3>(&succes);
                if( succes ) {
                    object->setOrientation( glm::quat(orientation) );
                }
                
                std::string renderQueueStr = renderqueueNode.getValue();
                if( !renderQueueStr.empty() ) {
                    uint renderQueue = renderQueueFromString(renderQueueStr);
                    object->setRenderQueue( renderQueue );
                }
                
                           
                scene->addObject( object, true );
            }
        }
    }
    
    return scene;
}

Scene::Scene( Root *root ):
    mRoot(root)
{
}

Scene::~Scene()
{
    for( const ObjectInfo &info : mObjects ) {
        if( info.ownsObject ) {
            delete info.object;
        }
    }
}

void Scene::addObject( SceneObject *object, bool takeOwnership )
{
    if( object->isDirty() ) {
        object->_updateTransform();
    }
    
    ObjectInfo info;
        info.object = object;
        info.ownsObject = takeOwnership;
    mObjects.push_back( info );
}

void Scene::removeObject( SceneObject *object )
{
    auto iter = std::find_if( mObjects.begin(), mObjects.end(), FindSceneInfo{object} );
    assert( iter != mObjects.end() );
    
    // we don't care about the order
    std::swap( *iter, mObjects.back() );
    mObjects.pop_back();
}

void Scene::update( float dt )
{
    for( const ObjectInfo &info : mObjects ) {
        SceneObject *object = info.object;
        object->update( dt );
        if( object->isDirty() ) {
            object->_updateTransform();
        }
    }
}

void Scene::quarySceneObjects( const Frustrum &frustrum, std::vector<SceneObject*> &result )
{
    std::transform( mObjects.begin(), mObjects.end(), std::back_inserter(result), 
        []( const ObjectInfo &info ) {
            return info.object;
        }
    );
}

void Scene::forEachObject( std::function<void(SceneObject*)> callback )
{
    for( const ObjectInfo &info : mObjects ) {
        callback( info.object );
    }
}
