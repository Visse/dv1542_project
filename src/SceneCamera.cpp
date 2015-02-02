#include "SceneCamera.h"
#include "LowLevelRenderer.h"
#include "GpuBuffer.h"
#include "SceneRenderInfo.h"
#include "Controller.h"
#include "Scene.h"

#include <glm/glm.hpp>


SceneCamera::SceneCamera( const SharedPtr<Scene> &scene, const SharedPtr<Controller> &controller ) :
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
    
    mSceneUniformBuffer->setContent( &sceneUniforms, 1 );
    
    Frustrum frustrum = getFrustrum();
    
    mScene->quarySceneObjects( frustrum, mCulledObjects );
    
    for( SceneObject *sceneObject : mCulledObjects ) {
        sceneObject->queueRenderable( renderer );
    }
}

