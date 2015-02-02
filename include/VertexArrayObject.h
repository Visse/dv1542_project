#pragma once

#include "GLTypes.h"

class VertexArrayObject {
public:
    VertexArrayObject() = default;
    VertexArrayObject( const VertexArrayObject& ) = delete;
    VertexArrayObject( VertexArrayObject &&move );
    ~VertexArrayObject();
    
    VertexArrayObject& operator = ( const VertexArrayObject& ) = delete;
    VertexArrayObject& operator = ( VertexArrayObject &&move ) = delete;
    
    void setVertexAttribPointer( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint offset );
    
    void bindVAO();
    void unbindVAO();
    
private:
    void createVAO();
    void destroyVAO();
    
private:
    GLuint mVAO = 0;
};