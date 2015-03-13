#include "GpuBuffer.h"
#include "GLinclude.h"
#include "DebugLevel.h"

#include <cassert>


GLuint getBoundBuffer( GLenum target )
{
    switch( target ) {
    case( GL_ARRAY_BUFFER ):
        target = GL_ARRAY_BUFFER_BINDING;
        break;
    case( GL_ELEMENT_ARRAY_BUFFER ):
        target = GL_ELEMENT_ARRAY_BUFFER_BINDING;
        break;
    case( GL_UNIFORM_BUFFER ):
        target = GL_UNIFORM_BUFFER_BINDING;
        break;
    case( GL_SHADER_STORAGE_BUFFER ):
        target = GL_SHADER_STORAGE_BUFFER_BINDING;
        break;
    default:
        assert( false && "Invalid target!" );
    }
    GLint boundBuffer;
    glGetIntegerv( target, &boundBuffer );
    return boundBuffer;
}

bool isMappedBuffer( GLenum target )
{
    GLint isMapped;
    glGetBufferParameteriv( target, GL_BUFFER_MAPPED, &isMapped );
    return isMapped == GL_TRUE;
}


GLenum usageAndUpdateToGL( BufferUsage usage, BufferUpdate update )
{
    static const GLenum LOOKUP_TABLE[9] = {
        GL_STATIC_DRAW,
        GL_STATIC_READ,
        GL_STATIC_COPY,
        
        GL_DYNAMIC_DRAW,
        GL_DYNAMIC_READ,
        GL_DYNAMIC_COPY,
        
        GL_STREAM_DRAW,
        GL_STREAM_READ,
        GL_STREAM_COPY
    };
    int index = static_cast<int>(usage)*3 + static_cast<int>(update);
    
    assert( index >= 0 && index < 9 );
    return LOOKUP_TABLE[index];
}

GLenum usageToGL( BufferUsage usage )
{
    static const GLenum LOOKUP_TABLE[3] = {
        GL_WRITE_ONLY,
        GL_READ_ONLY,
        GL_READ_WRITE
    };
    int index = static_cast<int>(usage);
    
    assert( index >= 0 && index < 3 );
    return LOOKUP_TABLE[index];
}

GLenum bufferTypeToGL( BufferType type )
{
    switch( type ) {
    case( BufferType::Vertexes ):
        return GL_ARRAY_BUFFER;
    case( BufferType::Indexes ):
        return GL_ELEMENT_ARRAY_BUFFER;
    case( BufferType::Uniforms ):
        return GL_UNIFORM_BUFFER;
    case( BufferType::ShaderStorage ):
        return GL_SHADER_STORAGE_BUFFER;
    }
    assert( false && "Add the type to the switch :)" );
    return 0;
}

GpuBuffer::GpuBuffer( BufferType type, size_t size, BufferUsage usage, BufferUpdate update ) :
    mUsage(usage),
    mUpdate(update),
    mType(type),
    mSize(size)
{
    GLenum bufferType = bufferTypeToGL( mType );
    
    glGenBuffers( 1, &mBuffer );
    glBindBuffer( bufferType, mBuffer );
    
    GLenum bufferUsage = usageAndUpdateToGL( mUsage, mUpdate );
    glBufferData( bufferType, size, NULL, bufferUsage );
    
    glBindBuffer( bufferType, 0 );
}

GpuBuffer::GpuBuffer( GpuBuffer&& move ) :
    mBuffer(move.mBuffer)
{
    move.mBuffer = 0;
}

GpuBuffer::~GpuBuffer()
{
    destoyBuffer();
}

void GpuBuffer::createBuffer()
{
    assert( mBuffer == 0 );
    glGenBuffers( 1, &mBuffer );
}

void GpuBuffer::destoyBuffer()
{
    glDeleteBuffers( 1, &mBuffer );
}

void GpuBuffer::setSize( size_t size )
{
    GLenum bufferType = bufferTypeToGL( mType );
    if( mBuffer == 0 ) {
        createBuffer();
    }
    
    glBindBuffer( bufferType, mBuffer );
    GLenum usage = usageAndUpdateToGL( mUsage, mUpdate );
    glBufferData( bufferType, size, NULL, usage );
    mSize = size;
    glBindBuffer( bufferType, 0 );
}

void *GpuBuffer::mapBuffer( BufferUsage access )
{
    GLenum bufferType = bufferTypeToGL( mType );
    GLenum usage = usageToGL( access );

    glBindBuffer( bufferType, mBuffer );
    return glMapBuffer( bufferType, usage );
}

void GpuBuffer::unmapBuffer()
{
    GLenum bufferType = bufferTypeToGL( mType );
#ifdef USE_DEBUG_NORMAL
    // hmm getBoundBuffer doesn't work if we are mapped :/
    assert( getBoundBuffer(bufferType) == mBuffer && isMappedBuffer(bufferType) );
#endif
    glUnmapBuffer( bufferType );
    glBindBuffer( bufferType, 0 );
}

void GpuBuffer::bindBuffer()
{
    GLenum bufferType = bufferTypeToGL( mType );

    glBindBuffer( bufferType, mBuffer );
}

void GpuBuffer::unbindBuffer()
{
    GLenum bufferType = bufferTypeToGL( mType );
#ifdef USE_DEBUG_NORMAL
    assert( getBoundBuffer(bufferType) == mBuffer );
#endif
    glBindBuffer( bufferType, 0 );
}

void GpuBuffer::bindIndexed( GLuint index )
{
#ifdef USE_DEBUG_NORMAL
    assert( mType == BufferType::Uniforms ||
            mType == BufferType::ShaderStorage
    );
#endif
    bindBuffer();
    GLenum type = bufferTypeToGL( mType  );
    glBindBufferBase( type, index, mBuffer );
    unbindBuffer();
}

void GpuBuffer::bindBufferAs( BufferType type )
{
    GLenum bufferType = bufferTypeToGL(type);
    glBindBuffer( bufferType, mBuffer );   
}

void GpuBuffer::unbindBufferAs( BufferType type )
{
    GLenum bufferType = bufferTypeToGL( type );
#ifdef USE_DEBUG_NORMAL
    assert( getBoundBuffer(bufferType) == mBuffer );
#endif
    glBindBuffer( bufferType, 0 );
}

void GpuBuffer::uploadData( const void *data, size_t size )
{
    if( mBuffer == 0 ) createBuffer();
    
    GLenum usage = usageAndUpdateToGL( mUsage, mUpdate );
    GLenum bufferType = bufferTypeToGL( mType );
    
    glBindBuffer( bufferType, mBuffer );
    glBufferData( bufferType, size, data, usage );
    mSize = size;
    
    glBindBuffer( bufferType, 0 );
}


/// @todo figure out how to download data from the gpu :/
// void GpuBuffer::downloadData( void *data, size_t offset, size_t size )
// {
//     assert( mBuffer != 0 );
//     
//     glBindBuffer( GL_ARRAY_BUFFER, mBuffer );
// }
// 



SharedPtr<GpuBuffer> GpuBuffer::CreateBuffer( BufferType type, size_t size, BufferUsage usage, BufferUpdate update )
{
    return makeSharedPtr<GpuBuffer>( type, size, usage, update );
}

