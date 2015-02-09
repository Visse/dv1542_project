#include "SceneCamera.h"
#include "LowLevelRenderer.h"
#include "GpuBuffer.h"
#include "SceneRenderInfo.h"
#include "Controller.h"
#include "Scene.h"
#include "Root.h"
#include "DebugDrawer.h"

#include <glm/glm.hpp>


SceneCamera::SceneCamera( Root *root, const SharedPtr<Scene> &scene, const SharedPtr<Controller> &controller ) :
    mRoot(root),
    mScene(scene),
    mController(controller)
{
    mSceneUniformBuffer = makeSharedPtr<GpuBuffer>();
    mSceneUniformBuffer->setType( BufferType::Uniforms );
    mSceneUniformBuffer->setSize( sizeof(SceneRenderUniforms) );
}

GpuBuffer* SceneCamera::getSceneUniforms()
{
    return mSceneUniformBuffer.get();
}

void SceneCamera::update( float dt )
{
    Camera::update( dt );
    if( mController ) {
        mController->update( dt );
    }
}

void SceneCamera::render( LowLevelRenderer &renderer )
{
    mCulledObjects.clear();
    
    glm::mat4 cameraTransform;
    if( mController ) {
        cameraTransform = mController->getTransformation();
    }
    
    SceneRenderUniforms sceneUniforms;
        sceneUniforms.viewMatrix = glm::inverse(cameraTransform);
        sceneUniforms.projectionMatrix = getProjectionMatrix();
        sceneUniforms.viewProjMatrix = sceneUniforms.projectionMatrix * sceneUniforms.viewMatrix;
        sceneUniforms.inverseViewMatrix = cameraTransform;
        sceneUniforms.inverseProjectionMatrix = glm::inverse( sceneUniforms.projectionMatrix );
        sceneUniforms.inverseViewProjMatrix = glm::inverse( sceneUniforms.viewProjMatrix );
        sceneUniforms.clippingPlanes = glm::vec2( getNearPlane(), getFarPlane() );
        sceneUniforms.cameraPos = mController->getPosition();
        
    mSceneUniformBuffer->setContent( &sceneUniforms, 1 );
    
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

glm::vec3 SceneCamera::getAmbientColor()
{
    return mScene->getAmbientColor();
}

