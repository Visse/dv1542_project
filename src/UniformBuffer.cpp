#include "UniformBuffer.h"

#include <cstring>
#include <stdexcept>

UniformBuffer::UniformBuffer( GLuint buffer, void *memory, GLuint size, GLuint offset ) :
    mBuffer(buffer), 
    mSize(size),
    mOffset(offset),
    mMemory(memory) 
{
}

void UniformBuffer::setRawContent( GLuint offset, const void *uniform, GLuint size )
{
    if( (offset+size) > mSize ) {
        throw std::runtime_error( "UniformBuffer::setUniform - out of bounds!" );
    }
    
    // if mMemory is null, then mSize is 0 - and the check above failed
    void *dest = (void*)((uintptr_t)mMemory + offset);
    std::memcpy( dest, uniform, size );
}

