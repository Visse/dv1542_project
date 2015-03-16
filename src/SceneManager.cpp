#include "SceneManager.h"
#include "Root.h"
#include "GraphicsManager.h"
#include "SceneCamera.h"
#include "Config.h"
#include "Scene.h"
#include "FlyingController.h"
#include "Timer.h"
#include "StartupMesurements.h"
#include "SceneObjectFactory.h"
#include "StringUtils.h"

#include <glm/gtc/matrix_transform.hpp>

#include <cassert>

bool SceneManager::init( Root *root )
{
    Timer initTimer;
    
    mRoot = root;
    
    addFactory( "DeferredEntity", new DeferredEntityFactory(mRoot), true );
    addFactory( "ComputeParticle", new ComputeParticleFactory(mRoot), true );
    addFactory( "Light", new LightFactory(mRoot), true );
    addFactory( "SkyBox", new SkyBoxFactory(mRoot), true );
    
    StartupMesurements *mesurements = mRoot->getStartupMesurements();
    mesurements->sceneStartup = initTimer.getTimeAsSeconds();
    
    return true;
}

void SceneManager::postInit()
{
    const Config *config = mRoot->getConfig();
    
    mScene = Scene::LoadFromFile( mRoot, config->startScene );
    
    auto controller = makeSharedPtr<FlyingController>( mRoot->getInputManager() );
    
    controller->loadFromConfig( config->freeCamera );
    controller->setMouseControll( true );
    controller->setPosition( glm::vec3(0,1,0) );
    mCamera = new SceneCamera( mRoot, mScene, controller );
    
    float aspect = (float)config->windowWidth / (float)config->windowHeight;
    
    mCamera->setNearPlane( config->nearPlane );
    mCamera->setFarPlane( config->farPlane );
    mCamera->setFOV( glm::radians(config->fov) );
    mCamera->setAspect( aspect );
    
    GraphicsManager *graphicsMgr = mRoot->getGraphicsManager();
    graphicsMgr->addCamera( mCamera );
}

void SceneManager::destroy()
{
    mScene.reset();
    
    GraphicsManager *graphicsMgr = mRoot->getGraphicsManager();
    graphicsMgr->removeCamera( mCamera );
    
    delete mCamera;
    mCamera = nullptr;
    
    mRoot = nullptr;
    
    for( const auto &entry : mSceneObjectFactories ) {
        const SceneObjectFactoryInfo &info = entry.second;
        if( info.ownsFactory) {
            delete info.factory;
        }
    }
    mSceneObjectFactories.clear();
}

void SceneManager::update( float dt )
{
    mScene->update( dt );
    mCamera->update( dt );
}

void SceneManager::addFactory( const std::string &type, SceneObjectFactory *factory, bool takeOwnership )
{
    bool succes = mSceneObjectFactories.emplace( StringUtils::toLowerCase(type), SceneObjectFactoryInfo{factory,takeOwnership} ).second;
    assert( succes );
}

void SceneManager::removeFactory( const std::string &type )
{
    auto iter = mSceneObjectFactories.find( StringUtils::toLowerCase(type) );
    assert( iter != mSceneObjectFactories.end() );
    
    if( iter->second.ownsFactory ) {
        delete iter->second.factory;
    }
    mSceneObjectFactories.erase( iter );
}

SceneObjectFactory* SceneManager::getFactory( const std::string &type )
{
    auto iter = mSceneObjectFactories.find(StringUtils::toLowerCase(type));
    if( iter != mSceneObjectFactories.end() ) {
        return iter->second.factory;
    }
    return nullptr;
}



