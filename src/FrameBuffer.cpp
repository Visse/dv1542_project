#include "FrameBuffer.h"
#include "Texture.h"

#include "GLinclude.h"
#include "DebugLevel.h"

#include <cassert>

GLuint getBoundFrameBuffer() {
    GLint framebuffer;
    glGetIntegerv( GL_FRAMEBUFFER_BINDING, &framebuffer );
    return framebuffer;
}

FrameBuffer::FrameBuffer()
{
    glGenFramebuffers( 1, &mFrameBuffer );
}

FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers( 1, &mFrameBuffer );
}


void FrameBuffer::attachColorTexture( const SharedPtr<Texture> &texture, int index )
{
    assert( index >= 0 && index < 8 );
    glBindFramebuffer( GL_FRAMEBUFFER, mFrameBuffer );
    glFramebufferTexture( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+index, texture->getGLTexture(), 0 );
    
    GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
    
    glColorMaski( index, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
    mAttachedTextures[index] = texture;
    
    updateDrawBuffers();
    
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    
    if( status != GL_FRAMEBUFFER_COMPLETE ) {
        throw std::runtime_error( "Failed to attach color texture to framebuffer!" );
    }
    
}

void FrameBuffer::setDepthTexture( const SharedPtr<Texture> &texture )
{
    glBindFramebuffer( GL_FRAMEBUFFER, mFrameBuffer );
    glFramebufferTexture( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture->getGLTexture(), 0 );
    
    GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
    
    updateDrawBuffers();
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    
    if( status != GL_FRAMEBUFFER_COMPLETE ) {
        throw std::runtime_error( "Failed to attach depth texture to framebuffer!" );
    }
    
    mDepthTexture = texture;
}


void FrameBuffer::bindFrameBuffer()
{
    glBindFramebuffer( GL_FRAMEBUFFER, mFrameBuffer );
}

void FrameBuffer::unbindFrameBuffer()
{
#ifdef USE_DEBUG_NORMAL
    assert( getBoundFrameBuffer() == mFrameBuffer );
#endif
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void FrameBuffer::updateDrawBuffers()
{
    GLenum drawBuffers[8];
    for( int i=0; i < 8; ++i ) {
        if( mAttachedTextures[i] ) {
            drawBuffers[i] = GL_COLOR_ATTACHMENT0+i;
        }
        else {
            drawBuffers[i] = GL_NONE;
        }
    }
    glDrawBuffers( 8, drawBuffers ); 
}
