#pragma once

#include "GLTypes.h"

/**
 * Class UniformBuffer
 * This class is intendet to help upload uniforms to a GpuBuffer,
 * Its designed to be created with memory obtained from 'mapBufferRange'
 * and should be discarded at the call of unmapBuffer.
 */
class UniformBuffer {
public:
    UniformBuffer() = default;
    UniformBuffer( const UniformBuffer& ) = default;
    
    UniformBuffer& operator = ( const UniformBuffer& ) = default;
    
public:
    UniformBuffer( GLuint buffer, void *memory, GLuint size, GLuint offset );
    
    void setRawContent( GLuint offset, const void *uniform, GLuint size );
    
    GLuint getBuffer() const {
        return mBuffer;
    }
    GLuint getSize() const {
        return mSize;
    }
    GLuint getOffset() const {
        return mOffset;
    }
    
private:
    GLuint mBuffer = 0,
           mSize,
           mOffset;
    
    void *mMemory = nullptr;
};

