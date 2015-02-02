#pragma once

#include "Camera.h"
#include "Entity.h"

#include <vector>

class Controller;
class Scene;

class SceneCamera :
    public Camera
{
public:
    SceneCamera( const SharedPtr<Scene> &scene, const SharedPtr<Controller> &controller ); 
    
    virtual GpuBuffer* getSceneUniforms();
    
    virtual void update( float dt ) override;
    virtual void render( LowLevelRenderer &renderer ) override;
    
private:
    SharedPtr<Scene> mScene;
    SharedPtr<Controller> mController;
    SharedPtr<GpuBuffer> mSceneUniformBuffer;
    
    std::vector<SceneObject*> mCulledObjects;
};