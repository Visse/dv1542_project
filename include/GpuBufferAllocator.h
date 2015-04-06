#pragma once

#include "GLTypes.h"
#include "StackAllocationPattern.h"

#include <list>

struct GpuBufferResource {
    GLuint glBuffer = 0;
    // if not null, the buffer is mapped
    void *mapped = nullptr;
    
    size_t size = 0;
    size_t flushedFromOffset = 0,
           allocatedToOffset = 0,
           lastUsed = 0;
};

class GpuSubBuffer 
{
    friend class GpuBufferAllocator;
    GpuSubBuffer( GpuBufferResource *buffer, size_t offset, size_t size );

public:
    GpuSubBuffer() = default;
    GpuSubBuffer( const GpuSubBuffer& ) = default;
    GpuSubBuffer& operator = ( const GpuSubBuffer& ) = default;
    
    void setContent( const void *content, size_t size );
    
    template< typename Type >
    void setContent( const Type &content ) {
        setContent( &content, sizeof(Type) );
    }
    
private:
    GpuBufferResource *mBuffer = nullptr;
    size_t mOffset=0, mSize=0;
};

class GpuBufferAllocator {
    GpuBufferAllocator() = delete;
    GpuBufferAllocator( const GpuBufferAllocator& ) = delete;
    GpuBufferAllocator( GpuBufferAllocator&& ) = delete;
    GpuBufferAllocator& operator = ( const GpuBufferAllocator& ) = delete;
    GpuBufferAllocator& operator = ( GpuBufferAllocator&& ) = delete;
    
public:
    GpuBufferAllocator( GLenum bufferType, GLenum usage, size_t defaultBufferSize );
    ~GpuBufferAllocator();
    
    // flush allocated buffers, 
    // any buffers allocated before this is uploaded to the gpu and is no longer changable
    void flush();
    
    // clear any allocations, all allocations before this is no longer valid
    void clear();
    
    GpuSubBuffer allocateBuffer( size_t size, size_t allignment = 1 );
    
protected:
    struct BufferInfo {
        GpuBufferResource buffer;
        StackAllocationPattern allocator;
    };
    
protected:
    void mapBuffer( BufferInfo &info );
    void unmapBuffer( BufferInfo &info );
    void flushBuffer( BufferInfo &info );
    void clearBuffer( BufferInfo &info );
    bool tryAllocateFromBuffer( BufferInfo &info, size_t size, size_t allignment, GpuSubBuffer &result );
    
    void createBuffer( BufferInfo &info, size_t size );
    void destroyBuffer( BufferInfo &info );
    
protected:
    GLenum mBufferType, mUsage;
    size_t mDefaultBufferSize;
    
    // I'm using list since we keep points to the buffers, so we don't want them relocated.
    std::list<BufferInfo> mBuffers;
};

