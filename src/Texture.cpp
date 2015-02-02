#include "Texture.h"
#include "StringUtils.h"
#include "GLinclude.h"

#include <stdexcept>
#include <cerrno>
#include <cassert>

#include <FreeImagePlus.h>

GLint getBoundTexture()
{
    GLint boundTexture;
    glGetIntegerv( GL_TEXTURE_BINDING_2D, &boundTexture );
    return boundTexture;
}

TextureType stringToTextureType( const std::string &str )
{
    if( StringUtils::equalCaseInsensitive(str,"Red") ) {
        return TextureType::Red;
    }
    if( StringUtils::equalCaseInsensitive(str,"RG") ) {
        return TextureType::RG;
    }
    if( StringUtils::equalCaseInsensitive(str,"RGB") ) {
        return TextureType::RGB;
    }
    if( StringUtils::equalCaseInsensitive(str,"RGBA") ) {
        return TextureType::RGBA;
    }
    if( StringUtils::equalCaseInsensitive(str,"Depth") ) {
        return TextureType::Depth;
    }
    
    throw std::runtime_error( StringUtils::strjoin("String isn't a TextureType (\"",str,"\")") );
}

GLint typeToGLFormat( TextureType type )
{
    switch( type )
    {
    case( TextureType::Red ):
        return GL_R8;
    case( TextureType::RG ):
        return GL_RG8;
    case( TextureType::RGB ):
        return GL_RGB8;
    case( TextureType::RGBA ):
        return GL_RGBA8;
    case( TextureType::Depth ):
        return GL_DEPTH_COMPONENT16;
    }
    return GL_RGBA8;
}

GLint mipmapForSize( const glm::ivec2 &size )
{
    int tmp = size.x | size.y;
    int levels = 1;
    while( tmp >>= 1 ) levels++;
    return levels;
    
}

Texture::Texture( TextureType type, const glm::ivec2& size, GLuint glTexture ) :
    mGLTexture(glTexture),
    mType(type),
    mSize(size)
{
}

Texture::~Texture()
{
    glDeleteTextures( 1, &mGLTexture );
}

void Texture::bindTexture( int unit )
{
    assert( unit >= 0 && unit < 15 );
    glActiveTexture( GL_TEXTURE0 + unit );
    glBindTexture( GL_TEXTURE_2D, mGLTexture );
}

void Texture::unbindTexture( int unit )
{    
    assert( unit >= 0 && unit < 15 );
    glActiveTexture( GL_TEXTURE0 + unit );
#ifdef USE_DEBUG_NORMAL
    assert( getBoundTexture() == mGLTexture );
#endif
    glBindTexture( GL_TEXTURE_2D, 0 );
}

SharedPtr<Texture> createTextureFromFipImage( TextureType type,  fipImage &image )
{
    FreeImage_FlipVertical(image);
    
    glm::ivec2 size( image.getWidth(), image.getHeight() );
    GLuint glTexture;
    glGenTextures( 1, &glTexture );
    glBindTexture( GL_TEXTURE_2D, glTexture );
    
    GLint internalFormat = typeToGLFormat( type );
    
    int mipmaps = mipmapForSize( size );
    
    glTexStorage2D( GL_TEXTURE_2D, mipmaps, internalFormat, size.x, size.y );
    glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, size.x, size.y, GL_BGRA, GL_UNSIGNED_BYTE, image.accessPixels() );
//     glTexImage2D( GL_TEXTURE_2D, 0, internalFormat, size.x, size.y, 0, GL_BGRA, GL_UNSIGNED_BYTE, image.accessPixels() );

    glGenerateMipmap( GL_TEXTURE_2D );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    
    return std::make_shared<Texture>( type, size, glTexture );
}

SharedPtr<Texture> Texture::LoadTexture( TextureType type, const std::string& filename )
{
    fipImage image;

    if( !image.load( filename.c_str() ) )
    {
        throw std::runtime_error( StringUtils::strjoin("Failed to load image from file \"", filename, "\"!") );
    }

    if( !image.convertTo32Bits() )
    {
        throw std::runtime_error( StringUtils::strjoin("Failed to convert image from file \"", filename, "\" to 32 bit!") );
    }
    
    return createTextureFromFipImage( type, image );
}

SharedPtr<Texture> Texture::LoadTextureFromMemory( TextureType type, const void *memory, size_t size )
{
    fipImage image;
    
    struct IOData {
        const void *memory;
        size_t size;
        size_t pos;
    };
    
    fipMemoryIO io( (BYTE*)const_cast<void*>(memory), size );
    if( !image.loadFromMemory(io) ) {
        throw std::runtime_error( "Failed to load image from memory!" );
    }
    if( !image.convertTo32Bits() ) {
        throw std::runtime_error( "Failed to convert image from memory!" );
    }
    return createTextureFromFipImage( type, image );
}

SharedPtr<Texture> Texture::CreateTexture( TextureType type, const glm::ivec2& size, GLuint mipmaps )
{
    GLuint glTexture;
    GLint internalFormat = typeToGLFormat( type );

    glGenTextures( 1, &glTexture );
    glBindTexture( GL_TEXTURE_2D, glTexture );
    
    if( mipmaps == 0 ) {
        mipmaps = mipmapForSize( size );
    }
    
    glTexStorage2D( GL_TEXTURE_2D, mipmaps, internalFormat, size.x, size.y );
//     glTexImage2D( GL_TEXTURE_2D, 0, format, size.x, size.y, 0, format, GL_UNSIGNED_BYTE, NULL );
//     glGenerateMipmap( GL_TEXTURE_2D );
    
    if( mipmaps > 1 ) {
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    }
    else {
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    }
    
    return std::make_shared<Texture>( type, size, glTexture );
}
