#include "SceneCamera.h"
#include "GpuBuffer.h"
#include "UniformBlockDefinitions.h"
#include "Controller.h"
#include "Scene.h"
#include "Root.h"
#include "DebugDrawer.h"
#include "DefaultGpuProgramLocations.h"
#include "Renderer.h"

#include <glm/glm.hpp>


SceneCamera::SceneCamera( Root *root, const SharedPtr<Scene> &scene, const SharedPtr<Controller> &controller ) :
    mRoot(root),
    mScene(scene),
    mController(controller)
{
}

void SceneCamera::update( float dt )
{
    if( mController ) {
        mController->update( dt );
        setViewMatrix( glm::inverse(mController->getTransformation()) );
    }
    Camera::update( dt );
}

void SceneCamera::render( Renderer &renderer )
{
    renderer.renderScene( mScene.get(), this );
}

SceneRenderUniforms SceneCamera::getSceneUniforms()
{
    SceneRenderUniforms sceneUniforms;
        sceneUniforms.viewMatrix = getViewMatrix();
        sceneUniforms.projectionMatrix = getProjectionMatrix();
        sceneUniforms.viewProjMatrix = sceneUniforms.projectionMatrix * sceneUniforms.viewMatrix;
        sceneUniforms.inverseViewMatrix = glm::inverse( sceneUniforms.viewMatrix );
        sceneUniforms.inverseProjectionMatrix = glm::inverse( sceneUniforms.projectionMatrix );
        sceneUniforms.inverseViewProjMatrix = glm::inverse( sceneUniforms.viewProjMatrix );
        sceneUniforms.clippingPlanes = glm::vec2( getNearPlane(), getFarPlane() );
        sceneUniforms.cameraPosition = mController->getPosition();
        
    return sceneUniforms;
}
