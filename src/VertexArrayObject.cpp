#include "VertexArrayObject.h"
#include "GLinclude.h"

#include "DebugLevel.h"

#include <cassert>

GLuint getBoundVAO()
{
    GLint boundVAO;
    glGetIntegerv( GL_VERTEX_ARRAY_BINDING, &boundVAO );
    return boundVAO;
}

VertexArrayObject::VertexArrayObject( VertexArrayObject &&move ) :
    mVAO(move.mVAO)
{
    move.mVAO = 0;
}

VertexArrayObject::~VertexArrayObject()
{
    destroyVAO();
}

void VertexArrayObject::createVAO()
{
    assert( mVAO == 0 );
    glGenVertexArrays( 1, &mVAO );
}

void VertexArrayObject::destroyVAO()
{
    glDeleteVertexArrays( 1, &mVAO );
}

void VertexArrayObject::bindVAO()
{
    if( mVAO == 0 ) createVAO();
    glBindVertexArray( mVAO );
}

void VertexArrayObject::unbindVAO()
{
#ifdef USE_DEBUG_NORMAL
    assert( getBoundVAO() == mVAO );
#endif
    glBindVertexArray( 0 );
}

void VertexArrayObject::setVertexAttribPointer( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint offset )
{
#ifdef USE_DEBUG_NORMAL
    assert( getBoundVAO() == mVAO );
#endif
    
    glEnableVertexAttribArray( index );
    glVertexAttribPointer( index, size, type, normalized, stride, reinterpret_cast<const void*>(offset) );
}
