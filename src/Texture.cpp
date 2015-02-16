#include "Texture.h"
#include "StringUtils.h"
#include "GLinclude.h"

#include <stdexcept>
#include <cerrno>
#include <cassert>

#include <FreeImage.h>

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
    if( StringUtils::equalCaseInsensitive(str,"RGBF") ) {
        return TextureType::RGBF;
    }
    if( StringUtils::equalCaseInsensitive(str,"RGBAF") ) {
        return TextureType::RGBAF;
    }
    if( StringUtils::equalCaseInsensitive(str,"RGB_SNORM") ) {
        return TextureType::RGB_SNORM;
    }
    if( StringUtils::equalCaseInsensitive(str,"RGBA_SNORM") ) {
        return TextureType::RGBA_SNORM;
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
    case( TextureType::RGBF ):
        return GL_RGB16F;
    case( TextureType::RGBAF ):
        return GL_RGBA16F;
    case( TextureType::RGB_SNORM ):
        return GL_RGB8_SNORM;
    case( TextureType::RGBA_SNORM ):
        return GL_RGBA8_SNORM;
    }
    throw std::runtime_error("Invalid texture type!");
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

SharedPtr<Texture> createTextureFromFipImage( TextureType type, FIBITMAP *image, const std::string& source )
{
    // not 100% sure that this is needed, but it makes it easier to know that the pixels is in bgra format :)
    image = FreeImage_ConvertTo32Bits(image);
    if( !image )
    {
        throw std::runtime_error( StringUtils::strjoin("Failed to convert texture from ",source," to 32 bit!") );
    }
    
    FreeImage_FlipVertical(image);
    
    glm::ivec2 size( FreeImage_GetWidth(image), FreeImage_GetHeight(image) );
    
    GLuint glTexture;
    glGenTextures( 1, &glTexture );
    glBindTexture( GL_TEXTURE_2D, glTexture );
    
    GLint internalFormat = typeToGLFormat( type );
    
    int mipmaps = mipmapForSize( size );
    BYTE *bits = FreeImage_GetBits( image );
    
    glTexStorage2D( GL_TEXTURE_2D, mipmaps, internalFormat, size.x, size.y );
    glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, size.x, size.y, GL_BGRA, GL_UNSIGNED_BYTE, bits );
//     glTexImage2D( GL_TEXTURE_2D, 0, internalFormat, size.x, size.y, 0, GL_BGRA, GL_UNSIGNED_BYTE, image.accessPixels() );

    glGenerateMipmap( GL_TEXTURE_2D );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    
    FreeImage_Unload( image );
    
    return std::make_shared<Texture>( type, size, glTexture );
}

SharedPtr<Texture> Texture::LoadTexture( TextureType type, const std::string& filename )
{
    
    FREE_IMAGE_FORMAT filetype = FreeImage_GetFileType( filename.c_str() );
    if( filetype == FIF_UNKNOWN ){
        filetype = FreeImage_GetFIFFromFilename( filename.c_str() );
    }
    if( filetype == FIF_UNKNOWN ) {
        throw std::runtime_error( StringUtils::strjoin("Failed to detect image format for file \"",filename,"\"") );
    }
    
    FIBITMAP *image = FreeImage_Load( filetype, filename.c_str() );
    
    if( !image ) {
        throw std::runtime_error( StringUtils::strjoin("Failed to load texture from file \"",filename,"\"") );
    }
    
    SharedPtr<Texture> texture = createTextureFromFipImage( type, image, StringUtils::strjoin("file \"",filename,"\"") );
    FreeImage_Unload( image );
    
    return texture;
}

SharedPtr<Texture> Texture::LoadTextureFromMemory( TextureType type, const void *memory, size_t size )
{
    FIMEMORY *fimemory = FreeImage_OpenMemory( reinterpret_cast<BYTE*>(const_cast<void*>(memory)), size );
    
    FREE_IMAGE_FORMAT filetype = FreeImage_GetFileTypeFromMemory( fimemory );
    if( filetype == FIF_UNKNOWN ) {
        throw std::runtime_error( StringUtils::strjoin("Failed to detect image format for memory!") );
    }
    
    FIBITMAP *image = FreeImage_LoadFromMemory( filetype, fimemory );
    FreeImage_CloseMemory( fimemory );
    
    
    if( !image ) {
        throw std::runtime_error( "Failed to load texture from memory!" );
    }
    SharedPtr<Texture> texture =  createTextureFromFipImage( type, image, "memory" );
    
    FreeImage_Unload( image );
    return texture;
}

SharedPtr<Texture> Texture::LoadTextureFromRawMemory( TextureType type, const void *pixels, size_t width, size_t height )
{
    GLuint glTexture;
    glGenTextures( 1, &glTexture );
    glBindTexture( GL_TEXTURE_2D, glTexture );
    
    GLint internalFormat = typeToGLFormat( type );
    
    glm::ivec2 size(width,height);
    int mipmaps = mipmapForSize( size );
    
    glTexStorage2D( GL_TEXTURE_2D, mipmaps, internalFormat, size.x, size.y );
    glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, size.x, size.y, GL_RGBA, GL_UNSIGNED_BYTE, pixels );

    glGenerateMipmap( GL_TEXTURE_2D );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    return makeSharedPtr<Texture>( type, size, glTexture );
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
