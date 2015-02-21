#include "SceneCamera.h"
#include "LowLevelRenderer.h"
#include "GpuBuffer.h"
#include "UniformBlockDefinitions.h"
#include "Controller.h"
#include "Scene.h"
#include "Root.h"
#include "DebugDrawer.h"
#include "DefaultGpuProgramLocations.h"

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

void SceneCamera::render( LowLevelRenderer &renderer )
{
    mCulledObjects.clear();
    
    SceneRenderUniforms sceneUniforms;
        sceneUniforms.viewMatrix = getViewMatrix();
        sceneUniforms.projectionMatrix = getProjectionMatrix();
        sceneUniforms.viewProjMatrix = sceneUniforms.projectionMatrix * sceneUniforms.viewMatrix;
        sceneUniforms.inverseViewMatrix = glm::inverse(getViewMatrix());
        sceneUniforms.inverseProjectionMatrix = glm::inverse( sceneUniforms.projectionMatrix );
        sceneUniforms.inverseViewProjMatrix = glm::inverse( sceneUniforms.viewProjMatrix );
        sceneUniforms.clippingPlanes = glm::vec2( getNearPlane(), getFarPlane() );
        sceneUniforms.cameraPosition = mController->getPosition();
    
    UniformBuffer uniformBuffer = renderer.aquireUniformBuffer( sizeof(SceneRenderUniforms) );
    uniformBuffer.setIndex( getDefaultUniformBlockBinding(DefaultUniformBlockLocations::SceneInfo) );
    uniformBuffer.setRawContent( 0, &sceneUniforms, sizeof(SceneRenderUniforms) );
    
    renderer.setSceneUniforms( uniformBuffer );
    
    Frustrum frustrum = getFrustrum();
    
    mScene->quarySceneObjects( frustrum, mCulledObjects );
    
    for( SceneObject *sceneObject : mCulledObjects ) {
        sceneObject->queueRenderable( renderer );
    }
    
    DebugDrawer *debugDrawer = mRoot->getDebugDrawer();
    if( debugDrawer ) {
        debugDrawer->queueRenderable( renderer );
    }
}

