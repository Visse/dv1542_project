#pragma once

#include "Camera.h"
#include "Entity.h"

#include <vector>

class Root;
class Controller;
class Scene;

class SceneCamera :
    public Camera
{
public:
    SceneCamera( Root *root, const SharedPtr<Scene> &scene, const SharedPtr<Controller> &controller ); 
    
    virtual void update( float dt ) override;
    virtual void render( LowLevelRenderer &renderer ) override;
    virtual glm::vec3 getAmbientColor() override;
    
private:
    Root *mRoot;
    SharedPtr<Scene> mScene;
    SharedPtr<Controller> mController;
    
    std::vector<SceneObject*> mCulledObjects;
};