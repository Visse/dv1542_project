#pragma once

#include <cstdint>

#include "GLTypes.h"
#include "SharedPtr.h"

// Don't change the values in BufferUsage & BufferUpdate,
// they are used in 'usageAndUpdateToGL' in GpuBuffer.cpp

// enum BufferUsage
//    describes how the buffer data is being used,
//    WriteOnly: The data is only being written to (and used in the gpu)
//    ReadOnly: The data is only being read from (and updated in the gpu)
//    ReadWrite: The data is being written to and read from
enum class BufferUsage {
    WriteOnly = 0,
    ReadOnly  = 1,
    ReadWrite = 2
};

// enum BufferUpdate
//    describes how the buffer is being updated
//    Static: The data is mostly static and won't change
//    Dynamic: The data can change on occation
//    Stream: The data is being updated frekvently
enum class BufferUpdate {
    Static  = 0,
    Dynamic = 1,
    Stream  = 2
};

enum class BufferType {
    Vertexes,
    Indexes,
    Uniforms,
    ShaderStorage
};

class GpuBuffer {
public:
    static SharedPtr<GpuBuffer> CreateBuffer( BufferType type, size_t size, BufferUsage usage, BufferUpdate update );
    
public:
    GpuBuffer() = default;
    GpuBuffer( const GpuBuffer& ) = delete;
    GpuBuffer( GpuBuffer &&move );
    ~GpuBuffer();
    
    GpuBuffer& operator = ( const GpuBuffer& ) = delete;
    GpuBuffer& operator = ( GpuBuffer&& ) = delete;
     
    void setUsage( BufferUsage usage ) {
        mUsage = usage;
    }
    void setUpdate( BufferUpdate update ) {
        mUpdate = update;
    }
    
    void setType( BufferType type ) {
        mType = type;
    }
    BufferType getType() const {
        return mType;
    }
    void setSize( size_t size );
    size_t getSize() {
        return mSize;
    }
    
    void* mapBuffer( BufferUsage access );
    template< typename Type >
    Type* mapBuffer( BufferUsage access ) {
        return static_cast<Type*>( mapBuffer(access) );
    }
    
    void unmapBuffer();
    
    void bindBuffer();
    void unbindBuffer();
    
    void bindBufferAs( BufferType type );
    void unbindBufferAs( BufferType type );
    
    void bindIndexed( GLuint index );
    
    template< typename Type >
    void setContent( const Type *ptr, size_t count ) {
        uploadData( ptr, sizeof(Type)*count );
    }
    template< typename Type >
    void getContent( Type *ptr, size_t offset, size_t count ) {
        downloadData( ptr, offset, sizeof(Type)*count );
    }
    
    GLuint getGLBuffer() {
        return mBuffer;
    }
    
    
private:
    void destoyBuffer();
    void createBuffer();
    void uploadData( const void *data, size_t size );
    void downloadData( void *data, size_t offset, size_t size );
    
private:
    BufferUsage mUsage = BufferUsage::WriteOnly;
    BufferUpdate mUpdate = BufferUpdate::Static;
    BufferType mType = BufferType::Vertexes;
    GLuint mBuffer = 0;
    size_t mSize;
};