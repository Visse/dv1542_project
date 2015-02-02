#pragma once

#include "GLTypes.h"
#include "SharedPtr.h"

#include <vector>

class Texture;

class FrameBuffer {
    FrameBuffer( const FrameBuffer& ) = delete;
    FrameBuffer( FrameBuffer&& ) = delete; 
    FrameBuffer& operator = ( const FrameBuffer& ) = delete;
    FrameBuffer& operator = ( FrameBuffer&& ) = delete;
    
public:
    FrameBuffer();
    ~FrameBuffer();
    
    void attachColorTexture( const SharedPtr<Texture> &texture, int index );
    void setDepthTexture( const SharedPtr<Texture> &texture );
    
    void bindFrameBuffer();
    void unbindFrameBuffer();
    
private:
    GLuint mFrameBuffer;
    
    SharedPtr<Texture> mAttachedTextures[8];
    SharedPtr<Texture> mDepthTexture;
};