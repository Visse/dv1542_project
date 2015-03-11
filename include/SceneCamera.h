#pragma once

#include "Camera.h"
#include "SharedPtr.h"

#include <vector>

class Root;
class Scene;
class SceneObject;
class Controller;

class SceneCamera :
    public Camera
{
public:
    SceneCamera( Root *root, const SharedPtr<Scene> &scene, const SharedPtr<Controller> &controller ); 
    
    virtual void update( float dt ) override;
    virtual void render( Renderer &renderer ) override;
    virtual SceneRenderUniforms getSceneUniforms() override;
    
private:
    Root *mRoot;
    SharedPtr<Scene> mScene;
    SharedPtr<Controller> mController;
    
    std::vector<SceneObject*> mCulledObjects;
};