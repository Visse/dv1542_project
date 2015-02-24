#include "Scene.h"
#include "SceneObject.h"
#include "Root.h"
#include "ResourceManager.h"
#include "Entity.h"
#include "GlmStream.h"
#include "SceneManager.h"
#include "SceneObjectFactory.h"
#include "SharedEnums.h"
#include "Frustrum.h"
#include "SceneGraph.h"

#include "yaml-cxx/YamlCxx.h"

#include <cassert>
#include <algorithm>

#include <iostream>

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
                    unsigned int renderQueue = renderQueueFromString(renderQueueStr);
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
    mSceneGraph = makeUniquePtr<SceneGraph>(mRoot);
}

void Scene::addObject( SceneObject *object, bool takeOwnership )
{
    if( object->isDirty() ) {
        object->_updateTransform();
    }
    object->_setAutoDelete( takeOwnership );
    mSceneGraph->addObject( object );
}

void Scene::removeObject( SceneObject *object )
{
    mSceneGraph->removeObject( object );
}

void Scene::update( float dt )
{
    mSceneGraph->update( dt );
}

void Scene::quarySceneObjects( const Frustrum &frustrum, std::vector<SceneObject*> &result )
{
    mSceneGraph->quaryObjects( frustrum, result );
}

void Scene::forEachObject( const std::function<void(SceneObject*)> &callback )
{
    mSceneGraph->forEachObject( callback );
}
