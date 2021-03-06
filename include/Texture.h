#ifndef GLTEXTURE_H
#define GLTEXTURE_H

#include "GLTypes.h"
#include "SharedPtr.h"

#include <glm/vec2.hpp>

#include <string>

enum class TextureType
{
    Red,
    RG,
    RGB,
    RGBA,
    Depth,
    
    // 16 bits precision
    Red16,
    RG16,
    RGB16,
    RGBA16,
    
    // floating point formats
    RGBF,
    RGBAF,
    
    // [-1,1] 8 bits formats
    RGB_SNORM,
    RGBA_SNORM,
    
    CubeMap_RGB,
    CubeMap_Depth
};

TextureType stringToTextureType( const std::string &str );

class Texture
{
public:
    static SharedPtr<Texture> LoadTexture( TextureType type, const std::string &filename );
    static SharedPtr<Texture> LoadTextureFromMemory( TextureType, const void *memory, size_t size );
    static SharedPtr<Texture> LoadTextureFromRawMemory( TextureType, const void *pixels, size_t width, size_t height );
    static SharedPtr<Texture> CreateTexture( TextureType type, const glm::uvec2 &size, GLuint mipmaps = 0 );

public:
    Texture( TextureType type, const glm::uvec2 &size, GLuint glTexture );
    ~Texture();
    
    Texture( const Texture& ) = delete;
    Texture( Texture&& ) = delete;
    Texture& operator = ( const Texture& ) = delete;
    Texture& operator = ( Texture&& ) = delete;

    void bindTexture( int unit );
    void unbindTexture( int unit );
    
    TextureType getType() {
        return mType;
    }
    GLuint getGLTexture() {
        return mGLTexture;
    }
    glm::uvec2 getSize() {
        return mSize;
    }
    
    bool isCubeMap();
    
    void copyTexture( const SharedPtr<Texture> &texture );
    
private:
    GLuint mGLTexture;
    TextureType mType;
    glm::uvec2 mSize;
};

#endif
