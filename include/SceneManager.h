#pragma once

#include "BaseManager.h"
#include "SharedPtr.h"

#include <map>

class Scene;
class Camera;

class SceneObjectFactory;

class SceneManager :
    public BaseManager
{
public:
    virtual bool init( Root *root ); 
    virtual void postInit();
    virtual void destroy();
    
    virtual void update( float dt );
    
    Scene* getScene() {
        return mScene.get();
    }
    Camera* getCamera() {
        return mCamera;
    }
    
    void addFactory( const std::string &type, SceneObjectFactory *factory, bool takeOwnership );
    void removeFactory( const std::string &type );
    SceneObjectFactory* getFactory( const std::string &type );
    
private:
    struct SceneObjectFactoryInfo {
        SceneObjectFactory *factory;
        bool ownsFactory;
    };
    
private:
    Root *mRoot;
    Camera *mCamera;
    SharedPtr<Scene> mScene;
    
    std::map<std::string,SceneObjectFactoryInfo> mSceneObjectFactories;
};