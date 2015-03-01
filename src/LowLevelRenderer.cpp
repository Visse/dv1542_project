#include "LowLevelRenderer.h"
#include "Material.h"
#include "VertexArrayObject.h"
#include "GpuBuffer.h"
#include "DefaultGpuProgramLocations.h"
#include "Texture.h"
#include "Root.h"
#include "Config.h"
#include "FrameBuffer.h"
#include "GLinclude.h"
#include "ResourceManager.h"
#include "Camera.h"
#include "GpuProgram.h"
#include "UniformBufferAllocator.h"

#include <glm/gtc/type_ptr.hpp>

#include <cassert>

GLenum drawModeToGL( DrawMode mode )
{
    switch( mode ) {
    case( DrawMode::Points ):
        return GL_POINTS;
    case( DrawMode::Triangles ):
        return GL_TRIANGLES;
    }
    
    assert( false && "Invalid mode!" );
    return GL_POINTS;
}

LowLevelRenderer::LowLevelRenderer( Root *root ) :
    mRoot(root)
{
    const Config *config = mRoot->getConfig();
    unsigned int width = config->defferedBufferWidth,
                 height = config->defferedBufferHeight;
         
    glm::uvec2 size( width, height );
         
    mDeferredDiffuseTexture  = Texture::CreateTexture( TextureType::RGBA, size, 1 );
    mDeferredNormalTexture   = Texture::CreateTexture( TextureType::RGB_SNORM, size, 1 );
    mDeferredDepthTexture    = Texture::CreateTexture( TextureType::Depth, size, 1 );
    mDeferredPositionTexture = Texture::CreateTexture( TextureType::RGBF, size, 1 );
    
    mDeferredFrameBuffer = makeUniquePtr<FrameBuffer>();
    mDeferredFrameBuffer->attachColorTexture( mDeferredDiffuseTexture, getDefaultOutputLocation(DefaultOutputLocations::Diffuse) );
    mDeferredFrameBuffer->attachColorTexture( mDeferredNormalTexture, getDefaultOutputLocation(DefaultOutputLocations::Normal) );
    mDeferredFrameBuffer->attachColorTexture( mDeferredPositionTexture, getDefaultOutputLocation(DefaultOutputLocations::Position) );
    mDeferredFrameBuffer->setDepthTexture( mDeferredDepthTexture );
    
    mBufferAllocator = makeSharedPtr<UniformBufferAllocator>();
    
    mSceneUniforms.buffer = 0;
    
    mDrawCountHistory.setSize( config->valueHistoryLenght );
    mVertexCountHistory.setSize( config->valueHistoryLenght );
}

LowLevelRenderer::~LowLevelRenderer() = default;

void LowLevelRenderer::queueOperation( const QueueOperationParams &params )
{
    LowLevelRenderOperation operation;
        operation.material = params.material;
        operation.vao = params.vao;
        operation.indexBuffer = params.indexBuffer;
        operation.drawMode = params.drawMode;
        operation.vertexStart = params.vertexStart;
        operation.vertexCount = params.vertexCount;
        operation.faceCulling = params.faceCulling;
        operation.scissorTest = params.scissorTest;
        operation.scissorPos  = params.scissorPos;
        operation.scissorSize = params.scissorSize;
        
    size_t cur = 0;
    
    size_t id = params.sceneUniforms.getId();
    size_t index = params.sceneUniforms.getIndex();
    if( id == 0 && mSceneUniforms.buffer > 0) {
        operation.uniforms[cur] = mSceneUniforms;
        cur++;
    }
    else if( id > 0 && index != (size_t)GL_INVALID_INDEX ) {
        operation.uniforms[cur] = mBlockInfo[id-1];
        cur++;
    }
    
    for( size_t i=0; i < MAX_UNIFORM_BLOCK_COUNT; ++i ) {
        id = params.uniforms[i].getId();
        index = params.uniforms[i].getIndex();
        if( id > 0 && index != (size_t)GL_INVALID_INDEX ) {
            operation.uniforms[cur] = mBlockInfo[id-1];
            operation.uniforms[cur].index = index;
            cur++;
        }
    }
    
    operation.uniformCount = cur;
    
    mQueue[params.renderQueue].push_back( operation );
}

void LowLevelRenderer::flush()
{
    mBufferAllocator->flushAndReset();
    mBlockInfo.clear();
    
    sortRenderQueues();
    
    mDeferredFrameBuffer->bindFrameBuffer();
    glDepthMask( GL_TRUE );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    for( unsigned int i=RQ_DeferredFirst; i <= RQ_DeferredLast; ++i ) {
        renderDeferredQueue( i );
    }
    mDeferredFrameBuffer->unbindFrameBuffer();
  
    GLuint loc = getDefaultGBufferBinding( DefaultGBufferBinding::Diffuse );
    mDeferredDiffuseTexture->bindTexture( loc );
    loc = getDefaultGBufferBinding( DefaultGBufferBinding::Normal );
    mDeferredNormalTexture->bindTexture( loc );
    loc = getDefaultGBufferBinding( DefaultGBufferBinding::Depth );
    mDeferredDepthTexture->bindTexture( loc );
    loc = getDefaultGBufferBinding( DefaultGBufferBinding::Position );
    mDeferredPositionTexture->bindTexture( loc );
    
    for( unsigned int i=RQ_LightFirst; i <= RQ_LightLast; ++i ) {    
        renderLightQueue( i );
    }
    for( unsigned int i=RQ_OverlayFirst; i <= RQ_OverlayLast; ++i ) {
        renderOverlay( i );
    }    
    
    
    memset( &mBoundObjects, 0, sizeof(mBoundObjects) );
    
    mBoundObjects.faceCulling = true;
    mBoundObjects.scissorTest = false;
    glEnable( GL_CULL_FACE );
    glDisable( GL_SCISSOR_TEST );
    
    mDrawCountHistory.pushValue( mCurrentDrawCount );
    mVertexCountHistory.pushValue( mCurrentVertexCount );
    mCurrentDrawCount = 0;
    mCurrentVertexCount = 0;
}

UniformBuffer LowLevelRenderer::aquireUniformBuffer( size_t size )
{
    UniformBufferAllocator::AllocationResult result;
    result = mBufferAllocator->getMemory( size );
    
    UniformBlockInfo info;
        info.buffer = result.buffer;
        info.offset = result.offset;
        info.size = size;
        
    mBlockInfo.push_back( info );
    size_t id = mBlockInfo.size();
    
    return UniformBuffer( id, result.memory, size );
}

void LowLevelRenderer::setSceneUniforms( const UniformBuffer &buffer )
{
    size_t id = buffer.getId();
    
    if( id > 0 ) {
        mSceneUniforms = mBlockInfo[id-1];
        mSceneUniforms.index = buffer.getIndex();
    }
    else {
        mSceneUniforms.buffer = 0;
    }
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
    if( mBoundObjects.material != operation.material && operation.material ) {
        mBoundObjects.material = operation.material;
        mBoundObjects.material->bindMaterial();
    }
    if( mBoundObjects.vao != operation.vao && operation.vao ) {
        mBoundObjects.vao = operation.vao;
        mBoundObjects.vao->bindVAO();
    }
    if( mBoundObjects.indexBuffer != operation.indexBuffer && operation.indexBuffer ) {
        mBoundObjects.indexBuffer = operation.indexBuffer;
        mBoundObjects.indexBuffer->bindBuffer();
    }
    if( mBoundObjects.faceCulling != operation.faceCulling ) {
        mBoundObjects.faceCulling = operation.faceCulling;
        if( mBoundObjects.faceCulling ) {
            glEnable( GL_CULL_FACE );
        }
        else {
            glDisable( GL_CULL_FACE );
        }
    }
    if( mBoundObjects.scissorTest != operation.scissorTest ) {
        mBoundObjects.scissorTest = operation.scissorTest;
        if( mBoundObjects.scissorTest ) {
            glEnable( GL_SCISSOR_TEST );
        }
        else {
            glDisable( GL_SCISSOR_TEST );
        }
    }
    if( operation.scissorTest ) {
        glScissor( operation.scissorPos.x, operation.scissorPos.y, 
                   operation.scissorSize.x, operation.scissorSize.y );
    }
    
    for( size_t i=0; i < operation.uniformCount; ++i ) {
        const UniformBlockInfo &uniform = operation.uniforms[i];
        glBindBufferRange( GL_UNIFORM_BUFFER, uniform.index, uniform.buffer, uniform.offset, uniform.size );
    }
    
    GLenum mode = drawModeToGL(operation.drawMode);
    if( operation.indexBuffer ) {
        glDrawElements( mode, operation.vertexCount, GL_UNSIGNED_INT, reinterpret_cast<GLvoid*>(4*operation.vertexStart) );
    }
    else {
        glDrawArrays( mode, operation.vertexStart, operation.vertexCount );
    }
    
    mCurrentDrawCount++;
    mCurrentVertexCount += operation.vertexCount;
}

