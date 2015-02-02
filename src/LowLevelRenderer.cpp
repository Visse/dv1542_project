#include "LowLevelRenderer.h"
#include "Material.h"
#include "VertexArrayObject.h"
#include "GpuBuffer.h"
#include "Renderable.h"
#include "DefaultGpuProgramLocations.h"
#include "Texture.h"
#include "Root.h"
#include "Config.h"
#include "FrameBuffer.h"
#include "GLinclude.h"
#include "ResourceManager.h"
#include "Camera.h"
#include "GpuProgram.h"


#include <cassert>

LowLevelRenderer::LowLevelRenderer( Root *root ) :
    mRoot(root)
{
    const Config *config = mRoot->getConfig();
    uint width = config->defferedBufferWidth,
         height = config->defferedBufferHeight;
         
    glm::ivec2 size( width, height );
         
    mDefferedDiffuseTexture  = Texture::CreateTexture( TextureType::RGB, size, 1 );
    mDefferedNormalTexture   = Texture::CreateTexture( TextureType::RGB, size, 1 );
    mDefferedSpecularTexture = Texture::CreateTexture( TextureType::RGB, size, 1 );
    mDefferedDepthTexture    = Texture::CreateTexture( TextureType::Depth, size, 1 );
    
    mDefferedFrameBuffer = makeUniquePtr<FrameBuffer>();
    mDefferedFrameBuffer->attachColorTexture( mDefferedDiffuseTexture, getDefaultOutputLocation(DefaultOutputLocations::Diffuse) );
    mDefferedFrameBuffer->attachColorTexture( mDefferedNormalTexture, getDefaultOutputLocation(DefaultOutputLocations::Normal) );
    mDefferedFrameBuffer->attachColorTexture( mDefferedSpecularTexture, getDefaultOutputLocation(DefaultOutputLocations::Specular) );
    mDefferedFrameBuffer->setDepthTexture( mDefferedDepthTexture );
    
    ResourceManager *resourceMgr = mRoot->getResourceManager();
    mDefferedMaterial = resourceMgr->getMaterialAutoPack( "DefferedMaterial" );
    
    mDefferedMaterial->setTexture( "DiffuseTexture",  0, mDefferedDiffuseTexture );
    mDefferedMaterial->setTexture( "NormalTexture",   1, mDefferedNormalTexture );
    mDefferedMaterial->setTexture( "SpecularTexture", 2, mDefferedSpecularTexture );
    mDefferedMaterial->setTexture( "DepthTexture",    3, mDefferedDepthTexture );
    
    mVAO = makeUniquePtr<VertexArrayObject>();
    
    mDefaultSceneInfoBinding = getDefaultUniformBlockBinding( DefaultUniformBlockLocations::SceneInfo );
    
    SharedPtr<GpuProgram> defferedShader = mDefferedMaterial->getProgram();
    mDefferedShaderLoc.nearPlane = defferedShader->getUniformLocation( "NearPlane" );
    mDefferedShaderLoc.farPlane  = defferedShader->getUniformLocation( "FarPlane" );
}

LowLevelRenderer::~LowLevelRenderer() = default;

void LowLevelRenderer::queueOperation( const LowLevelRenderOperation &operation, uint queue )
{
    assert( queue < RQ_Count );
    
    mQueue[queue].push_back( operation );
    if( !operation.sceneUniforms && mCurrentCamera ) {
        mQueue[queue].back().sceneUniforms = mCurrentCamera->getSceneUniforms();
    }
}

void LowLevelRenderer::flush()
{
    sortRenderQueues();
    
    mDefferedFrameBuffer->bindFrameBuffer();
    for( uint i=RQ_DeferedFirst; i < RQ_DeferedLast; ++i ) {
        renderQueue( i );
    }
    mDefferedFrameBuffer->unbindFrameBuffer();
}

void LowLevelRenderer::clearFrame()
{
    glDepthMask( GL_TRUE );
    mDefferedFrameBuffer->bindFrameBuffer();
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    mDefferedFrameBuffer->unbindFrameBuffer();
}

void LowLevelRenderer::displayFrame()
{
    mDefferedMaterial->bindMaterial();
    mVAO->bindVAO();
    
    float farPlane = 100.f, 
          nearPlane = 0.1f;
        
    if( mCurrentCamera ) {
        farPlane = mCurrentCamera->getFarPlane();
        nearPlane = mCurrentCamera->getNearPlane();
    }
    
    glUniform1f( mDefferedShaderLoc.farPlane, farPlane );
    glUniform1f( mDefferedShaderLoc.nearPlane, nearPlane );
    
    glDrawArrays( GL_POINTS, 0, 1 );
    
    mVAO->unbindVAO();
}

void LowLevelRenderer::sortRenderQueues()
{
}

void LowLevelRenderer::renderQueue( uint queueId )
{
    OperationQueue &queue = mQueue[queueId];
    // @todo make smarter (sort based on material etc.)
    for( const LowLevelRenderOperation &operation : queue ) {
        if( operation.material ) {
            operation.material->bindMaterial();
        }
        if( operation.vao ) {
            operation.vao->bindVAO();
        }
        if( operation.indexBuffer ) {
            operation.indexBuffer->bindBuffer();
        }
        if( operation.sceneUniforms ) {
            operation.sceneUniforms->bindIndexed( mDefaultSceneInfoBinding );
        }
        
        operation.renderable->render();
    }
    queue.clear();
}



