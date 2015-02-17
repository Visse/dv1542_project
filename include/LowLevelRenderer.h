#pragma once

#include <vector>
#include <array>

#include <glm/vec2.hpp>

#include "UniquePtr.h"
#include "SharedPtr.h"
#include "GLTypes.h"
#include "SharedEnums.h"
#include "UniformBlock.h"
#include "ValueHistory.h"

class UniformBuffer;
class FrameBuffer;
class Renderable;
class VertexArrayObject;
class Material;
class GpuBuffer;
class Texture;
class Root;
class Camera;
class UniformBufferAllocator;

static const size_t MAX_UNIFORM_BLOCK_COUNT = 8;


struct QueueOperationParams {
    Material *material = nullptr;
    VertexArrayObject *vao = nullptr;
    GpuBuffer *indexBuffer = nullptr;
    
    UniformBuffer sceneUniforms;
    UniformBuffer uniforms[MAX_UNIFORM_BLOCK_COUNT];
    
    DrawMode drawMode = DrawMode::Points; 
    size_t vertexStart = 0, vertexCount = 0;
    
    RenderQueueId renderQueue = RQ_DeferredDefault;
    
    bool scissorTest = false;
    glm::vec2 scissorPos, scissorSize;
    
    bool faceCulling = true;
};

class LowLevelRenderer {
public:
    LowLevelRenderer( Root *root );
    ~LowLevelRenderer();
    
    void queueOperation( const QueueOperationParams &params );
    void flush();
    
    // returns a temporary buffer that is valid until flush() is called.
    UniformBuffer aquireUniformBuffer( size_t size );    
    void setSceneUniforms( const UniformBuffer &buffer );
    
    template< typename Type >
    UniformBuffer aquireUniformBuffer( size_t index, const Type &content ) {
        UniformBuffer buffer = aquireUniformBuffer( sizeof(Type) );
            buffer.setIndex( index );
            buffer.setRawContent( 0, &content, sizeof(Type) );
        return buffer;
    }
    
    const ValueHistory<int>& getDrawCountHistory () {
        return mDrawCountHistory;
    }
    const ValueHistory<int>& getVertexCountHistory () {
        return mVertexCountHistory;
    }

private:
    struct UniformBlockInfo {
        GLuint buffer;
        size_t size, offset, index;
    };
    struct LowLevelRenderOperation {
        Material *material;
        VertexArrayObject *vao;
        GpuBuffer *indexBuffer = nullptr;
        
        UniformBlockInfo uniforms[MAX_UNIFORM_BLOCK_COUNT+1];
        size_t uniformCount;
        
        DrawMode drawMode;
        size_t vertexStart, vertexCount;
        
        
        bool faceCulling, scissorTest;
        glm::vec2 scissorPos, scissorSize;
    };
    struct RenderOperation {
        size_t vertexStart, vertexCount;
        glm::vec2 scissorPos, scissorSize;
    };
private:
    void sortRenderQueues();
    void renderDeferredQueue( unsigned int queue );
    void renderLightQueue( unsigned int queue );
    void renderOverlay( unsigned int queue );
    
    void performOperation( const LowLevelRenderOperation &operation );
    
private:
    typedef std::vector<LowLevelRenderOperation> OperationQueue;
    typedef std::array<OperationQueue,RQ_Count> RenderQueue;
    
private:
    
private:
    Root *mRoot;
    RenderQueue mQueue;
    
    UniquePtr<FrameBuffer> mDeferredFrameBuffer;
    
    SharedPtr<Texture> mDeferredDiffuseTexture,
                       mDeferredDepthTexture,
                       mDeferredNormalTexture,
                       mDeferredPositionTexture;
    
    SharedPtr<UniformBufferAllocator> mBufferAllocator;
    std::vector<UniformBlockInfo> mBlockInfo;
    
    UniformBlockInfo mSceneUniforms;
    
    struct {
        Material *material = nullptr;
        VertexArrayObject *vao = nullptr;
        GpuBuffer *indexBuffer = nullptr;
        
        bool faceCulling=true, scissorTest=false;
    } mBoundObjects;
    
    
    int mCurrentDrawCount, mCurrentVertexCount;
    ValueHistory<int> mDrawCountHistory,
                      mVertexCountHistory;
};