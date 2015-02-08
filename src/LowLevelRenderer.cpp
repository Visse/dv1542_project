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
    unsigned int width = config->defferedBufferWidth,
                 height = config->defferedBufferHeight;
         
    glm::ivec2 size( width, height );
         
    mDeferredDiffuseTexture  = Texture::CreateTexture( TextureType::RGBA, size, 1 );
    mDeferredNormalTexture   = Texture::CreateTexture( TextureType::RGB, size, 1 );
    mDeferredDepthTexture    = Texture::CreateTexture( TextureType::Depth, size, 1 );
    mDeferredPositionTexture = Texture::CreateTexture( TextureType::RGBF, size, 1 );
    
    mDeferredFrameBuffer = makeUniquePtr<FrameBuffer>();
    mDeferredFrameBuffer->attachColorTexture( mDeferredDiffuseTexture, getDefaultOutputLocation(DefaultOutputLocations::Diffuse) );
    mDeferredFrameBuffer->attachColorTexture( mDeferredNormalTexture, getDefaultOutputLocation(DefaultOutputLocations::Normal) );
    mDeferredFrameBuffer->attachColorTexture( mDeferredPositionTexture, getDefaultOutputLocation(DefaultOutputLocations::Position) );
    mDeferredFrameBuffer->setDepthTexture( mDeferredDepthTexture );
    
    mDefaultSceneInfoBinding = getDefaultUniformBlockBinding( DefaultUniformBlockLocations::SceneInfo );
    
    ResourceManager *resourceMgr = mRoot->getResourceManager();
    mAmbientMaterial = resourceMgr->getMaterialAutoPack("AmbientMaterial");
    
}

LowLevelRenderer::~LowLevelRenderer() = default;

void LowLevelRenderer::queueOperation( const LowLevelRenderOperation &operation, unsigned int queue )
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
    
    mDeferredFrameBuffer->bindFrameBuffer();
    for( unsigned int i=RQ_DeferredFirst; i < RQ_DeferredLast; ++i ) {
        renderDeferredQueue( i );
    }
    mDeferredFrameBuffer->unbindFrameBuffer();
    
    mAmbientMaterial->bindMaterial();
    
        
    GLuint loc = getDefaultGBufferBinding( DefaultGBufferBinding::Diffuse );
    mDeferredDiffuseTexture->bindTexture( loc );
    
    loc = getDefaultGBufferBinding( DefaultGBufferBinding::Normal );
    mDeferredNormalTexture->bindTexture( loc );
    
    loc = getDefaultGBufferBinding( DefaultGBufferBinding::Depth );
    mDeferredDepthTexture->bindTexture( loc );
    
    loc = getDefaultGBufferBinding( DefaultGBufferBinding::Position );
    mDeferredPositionTexture->bindTexture( loc );
    
    glDrawArrays( GL_POINTS, 0, 1 );
    
    renderLightQueue( RQ_Light );
    
    renderOverlay( RQ_Overlay );
}

void LowLevelRenderer::clearFrame()
{
    glDepthMask( GL_TRUE );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    mDeferredFrameBuffer->bindFrameBuffer();
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    mDeferredFrameBuffer->unbindFrameBuffer();
}

void LowLevelRenderer::sortRenderQueues()
{
    // @todo make smarter (sort based on material etc.)
}

void LowLevelRenderer::renderDeferredQueue( unsigned int queueId )
{
    OperationQueue &queue = mQueue[queueId];
    for( const LowLevelRenderOperation &operation : queue ) {
        performOperation( operation );
    }
    queue.clear();
}

void LowLevelRenderer::renderLightQueue( unsigned int queueId )
{
    OperationQueue &queue = mQueue[queueId];

    
    // disable cliping by the near & far planes
    // this fixes the issue of being 'inside' the light volume
    glEnable( GL_DEPTH_CLAMP );
    
    for( const LowLevelRenderOperation &operation : queue )
    {
        performOperation( operation );
    }
    
    glDisable( GL_DEPTH_CLAMP );
    
    queue.clear();
}

void LowLevelRenderer::renderOverlay( unsigned int queueId )
{
    OperationQueue &queue = mQueue[queueId];
    
    for( const LowLevelRenderOperation &operation : queue )
    {
        performOperation( operation );
    }
    
    queue.clear();
}


inline void LowLevelRenderer::performOperation( const LowLevelRenderOperation &operation )
{
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


