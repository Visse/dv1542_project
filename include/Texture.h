#ifndef GLTEXTURE_H
#define GLTEXTURE_H

#include "GLTypes.h"

#include "SharedPtr.h"

#include <glm/vec2.hpp>

enum class TextureType
{
    Red,
    RG,
    RGB,
    RGBA,
    Depth
};

TextureType stringToTextureType( const std::string &str );
    
class Texture
{
public:
    static SharedPtr<Texture> LoadTexture( TextureType type, const std::string &filename );
    static SharedPtr<Texture> LoadTextureFromMemory( TextureType, const void *memory, size_t size );
    static SharedPtr<Texture> CreateTexture( TextureType type, const glm::ivec2 &size, GLuint mipmaps = 0 );

public:
    Texture( TextureType type, const glm::ivec2 &size, GLuint glTexture );
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
private:
    GLuint mGLTexture;
    TextureType mType;
    glm::ivec2 mSize;
};

#endif
