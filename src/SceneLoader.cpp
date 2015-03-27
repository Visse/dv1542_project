#include "SceneLoader.h"
#include "Scene.h"
#include "Root.h"
#include "Log.h"
#include "SceneManager.h"
#include "SceneObject.h"
#include "SceneGraph.h"
#include "ResourceManager.h"
#include "SceneObjectFactory.h"
#include "GlmStream.h"

#include "yaml-cxx/YamlCxx.h"

SceneLoader::SceneLoader( Root *root ) :
    mRoot(root)
{
    mScene = makeSharedPtr<Scene>( mRoot );
}


SceneObject* createObject( Log *log, SceneManager *sceneMgr, const Yaml::Node &objectNode );

void SceneLoader::loadFile( const std::string &filename )
{
    Log *log = mRoot->getDefaultLog();
    log->stream(LogSeverity::Information, "SceneLoader") << "Loading scene \"" << filename << "\"";
    
    Yaml::MappingNode sceneCfg = Yaml::Node::LoadFromFile(filename).asMapping();
    
    SceneManager *sceneMgr = mRoot->getSceneManager();
    ResourceManager *resourceMgr = mRoot->getResourceManager();
    
    glm::vec3 ambient = sceneCfg.getFirstValue("AmbientColor",false).asValue().getValue<glm::vec3>(mScene->getAmbientColor());
    mScene->setAmbientColor( ambient );
    
    auto resourcePackList = sceneCfg.getValues("ResourcePack");
    for( Yaml::Node packNode : resourcePackList ) {
        resourceMgr->loadResourcePack( packNode.asValue().getValue() );
    }
    
    std::string skyboxName = sceneCfg.getFirstValue("SkyBox",false).asValue().getValue();
    SharedPtr<Texture> skybox = resourceMgr->getTextureAutoPack( skyboxName );
    mScene->setSkyBox( skybox );
    
    auto objectList = sceneCfg.getValues("Object");
    for( Yaml::Node objectNode : objectList ) {
        
        SceneObject *object = createObject( log, sceneMgr, objectNode );
        
        if( object ) {
            mScene->addObject( object, true );
        }
    }
    
    auto tiledList = sceneCfg.getValues("Tiled");
    for( Yaml::Node tiledNode : tiledList ) {
        Yaml::MappingNode config = tiledNode.asMapping();
        
        glm::ivec3 count = config.getFirstValue("Count",false).asValue().getValue<glm::ivec3>();
        glm::vec3 spacing = config.getFirstValue("Spacing",false).asValue().getValue<glm::vec3>();
        
        Yaml::Node objectNode = config.getFirstValue("Object",false);
        
        for( int z=0; z < count.z; ++z ) {
            for( int y=0; y < count.y; ++y ) {
                for( int x=0; x < count.x; ++x ) {
                    SceneObject *object = createObject( log, sceneMgr, objectNode );
                    if( object ) {
                        glm::vec3 pos = object->getPosition();
                        
                        pos += glm::vec3(x,y,z) * spacing;
                        object->setPosition( pos );
                        
                        mScene->addObject( object, true );
                    }
                }
            }
        }
    }
}

SceneObject *createObject( Log *log, SceneManager *sceneMgr, const Yaml::Node &objectNode )
{ 
    Yaml::MappingNode config = objectNode.asMapping();
    std::string type = config.getFirstValue("Type",false).asValue().getValue();
    
    SceneObjectFactory *factory = sceneMgr->getFactory( type );
    if( factory ) {
        SceneObject *object = nullptr;
        try {
            object = factory->createObject( objectNode );
        } catch( const std::exception &e ) {
            log->stream(LogSeverity::Error, "SceneLoader") << "Failed to create object of type \"" << type << "\", error: " << e.what();
        }
        
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
            
            return object;
        }
    }
    else {
        log->stream(LogSeverity::Warning, "SceneLoader") << "No factory registred for type \"" << type << "\"";
    }
    return nullptr;
}
