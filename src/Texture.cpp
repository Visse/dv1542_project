#include "Texture.h"
#include "StringUtils.h"
#include "GLinclude.h"

#include <stdexcept>
#include <cerrno>
#include <cassert>

#include <FreeImage.h>

GLint getBoundTexture2D()
{
    GLint boundTexture;
    glGetIntegerv( GL_TEXTURE_BINDING_2D, &boundTexture );
    return boundTexture;
}
GLint getBoundTextureCube()
{
    GLint boundTexture;
    glGetIntegerv( GL_TEXTURE_BINDING_CUBE_MAP, &boundTexture );
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
    if( StringUtils::equalCaseInsensitive(str,"Red16") ) {
        return TextureType::Red16;
    }
    if( StringUtils::equalCaseInsensitive(str,"RG16") ) {
        return TextureType::RG16;
    }
    if( StringUtils::equalCaseInsensitive(str,"RGB16") ) {
        return TextureType::RGB16;
    }
    if( StringUtils::equalCaseInsensitive(str,"RGBA16") ) {
        return TextureType::RGBA16;
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
    if( StringUtils::equalCaseInsensitive(str,"CubeMap_RGB") ) {
        return TextureType::CubeMap_RGB;
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
    case( TextureType::CubeMap_RGB ):
        return GL_RGB8;
    case( TextureType::RGBA ):
        return GL_RGBA8;
        
    case( TextureType::Red16 ):
        return GL_R16;
    case( TextureType::RG16 ):
        return GL_RG16;
    case( TextureType::RGB16 ):
        return GL_RGB16;
    case( TextureType::RGBA16 ):
        return GL_RGBA16;
        
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

bool isCubeMap( TextureType type )
{
    switch( type ) {
    case( TextureType::CubeMap_RGB ):
        return true;
    default:
        return false;
    }
}

GLint mipmapForSize( const glm::uvec2 &size )
{
    int tmp = size.x | size.y;
    int levels = 1;
    while( tmp >>= 1 ) levels++;
    return levels;
    
}

Texture::Texture( TextureType type, const glm::uvec2& size, GLuint glTexture ) :
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
    if( isCubeMap(mType) ) {
        glBindTexture( GL_TEXTURE_CUBE_MAP, mGLTexture );
    }
    else {
        glBindTexture( GL_TEXTURE_2D, mGLTexture );
    }
}

void Texture::unbindTexture( int unit )
{    
    assert( unit >= 0 && unit < 15 );
    glActiveTexture( GL_TEXTURE0 + unit );
    if( isCubeMap(mType) ) {
#ifdef USE_DEBUG_NORMAL
        assert( getBoundTextureCube() == mGLTexture );
#endif
        glBindTexture( GL_TEXTURE_CUBE_MAP, mGLTexture );
    }
    else {
#ifdef USE_DEBUG_NORMAL
        assert( getBoundTexture2D() == mGLTexture );
#endif
        glBindTexture( GL_TEXTURE_2D, 0 );
    }
}

/// @todo FIXME this code needs some serius clean up....

bool loadImageToGLTexture( GLint target, FIBITMAP *image, const glm::uvec2 &size )
{    
    // not 100% sure that this is needed, but it makes it easier to know that the pixels is in bgra format :)
    image = FreeImage_ConvertTo32Bits(image);
    if( !image )
    {
        return false;
    }
    FreeImage_FlipVertical( image );
    BYTE *bits = FreeImage_GetBits( image );
    
    glTexSubImage2D( target, 0, 0, 0, size.x, size.y, GL_BGRA, GL_UNSIGNED_BYTE, bits );
    FreeImage_Unload( image );
    
    return true;
}

SharedPtr<Texture> createTextureFromFipImage( TextureType type, FIBITMAP *image )
{
    glm::uvec2 size( FreeImage_GetWidth(image), FreeImage_GetHeight(image) );
    GLint mipmaps = mipmapForSize( size );
    GLint internalFormat = typeToGLFormat( type );
    
    GLuint glTexture;
    glGenTextures( 1, &glTexture );
    glBindTexture( GL_TEXTURE_2D, glTexture );
    
    glTexStorage2D( GL_TEXTURE_2D, mipmaps, internalFormat, size.x, size.y );
    
    if( !loadImageToGLTexture(GL_TEXTURE_2D, image, size) ) {
        glDeleteTextures( 1, &glTexture );
        return SharedPtr<Texture>();
    }
    
    glGenerateMipmap( GL_TEXTURE_2D );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    
    return std::make_shared<Texture>( type, size, glTexture );
}

FIBITMAP* loadTexture( const std::string &filename ) 
{
    FREE_IMAGE_FORMAT filetype = FreeImage_GetFileType( filename.c_str() );
    if( filetype == FIF_UNKNOWN ){
        filetype = FreeImage_GetFIFFromFilename( filename.c_str() );
    }
    if( filetype == FIF_UNKNOWN ) {
        throw std::runtime_error( StringUtils::strjoin("Failed to detect image format for file \"",filename,"\"") );
    }
    
    FIBITMAP *texture = FreeImage_Load( filetype, filename.c_str() );
    if( !texture ) {
        throw std::runtime_error( StringUtils::strjoin("Failed to load texture from file \"",filename,"\"") );
    }
    
    return texture;
}

SharedPtr<Texture> createCubeMapTexture( TextureType type, const std::string &filename )
{
    std::string imageName, extension;
    
    {
        size_t nameEnd = filename.find_last_of('.');
        if( nameEnd != std::string::npos ) {
            extension = filename.substr( nameEnd );
        }
        imageName = filename.substr( 0, nameEnd );
    }
    
    
    static const std::string FACE_NAMES[6] = 
        {"Right", "Left", "Top", "Bottom", "Front", "Back"};
        
    GLuint texture;
    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_CUBE_MAP, texture );
    
    bool createdTexture = false;
    glm::uvec2 size;
    
    GLint internalFormat = typeToGLFormat( type );
    
    for( int i=0; i < 6; ++i ) {
        std::string textureFaceName = StringUtils::strjoin( imageName, FACE_NAMES[i], extension );
        
        FIBITMAP *image = loadTexture( textureFaceName );
        
        if( !createdTexture ) {
            size.x = FreeImage_GetWidth( image );
            size.y = FreeImage_GetHeight( image );
            
            if( size.x != size.y ) {
                FreeImage_Unload( image );
                glDeleteTextures( 1, &texture );
                throw std::runtime_error( "Invalid image size! - a cube map only accepts square textures." );
            }
            
            GLint mipmaps = mipmapForSize( size );
            glTexStorage2D( GL_TEXTURE_CUBE_MAP, mipmaps, internalFormat, size.x, size.y );
            createdTexture = true;
        }
        
        if( size.x != FreeImage_GetWidth(image) || size.y != FreeImage_GetHeight(image) ) {
            FreeImage_Unload( image );
            glDeleteTextures( 1, &texture );
            throw std::runtime_error( "Invalid image size! - all images in the cubemap must have the same size." );
        }
        
        if( !loadImageToGLTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, image, size) ) {
            FreeImage_Unload( image );
            glDeleteTextures( 1, &texture );
            throw std::runtime_error( "Failed to load image to cube face!" );
        }
        
        FreeImage_Unload( image );
    }
    
    glGenerateMipmap( GL_TEXTURE_CUBE_MAP );
    return makeSharedPtr<Texture>( type, size, texture );
}

SharedPtr<Texture> Texture::LoadTexture( TextureType type, const std::string& filename )
{
    if( isCubeMap(type) ) {
        return createCubeMapTexture( type, filename );
    }
    
    FIBITMAP *image = loadTexture( filename );
    
    SharedPtr<Texture> texture = createTextureFromFipImage( type, image );
    FreeImage_Unload( image );
    
    if( !texture ) {
        throw std::runtime_error( StringUtils::strjoin("Failed to load image from file \"", filename,"\".") );
    }
    
    return texture;
}

SharedPtr<Texture> Texture::LoadTextureFromMemory( TextureType type, const void *memory, size_t size )
{
    if( isCubeMap(type) ) {
        throw std::runtime_error( "Cube maps isn't supported to be loaded from memory!" );
    }
    
    FIMEMORY *fimemory = FreeImage_OpenMemory( reinterpret_cast<BYTE*>(const_cast<void*>(memory)), size );
    
    FREE_IMAGE_FORMAT filetype = FreeImage_GetFileTypeFromMemory( fimemory );
    if( filetype == FIF_UNKNOWN ) {
        FreeImage_CloseMemory( fimemory );
        throw std::runtime_error( StringUtils::strjoin("Failed to detect image format for memory!") );
    }
    
    FIBITMAP *image = FreeImage_LoadFromMemory( filetype, fimemory );
    FreeImage_CloseMemory( fimemory );
    
    if( !image ) {
        throw std::runtime_error( "Failed to load texture from memory!" );
    }
    SharedPtr<Texture> texture =  createTextureFromFipImage( type, image );
    FreeImage_Unload( image );
    
    if( !texture ) {
        throw std::runtime_error( "Failed to create texture from memory!" );
    }
    
    return texture;
}

SharedPtr<Texture> Texture::LoadTextureFromRawMemory( TextureType type, const void *pixels, size_t width, size_t height )
{
    GLuint glTexture;
    glGenTextures( 1, &glTexture );
    glBindTexture( GL_TEXTURE_2D, glTexture );
    
    GLint internalFormat = typeToGLFormat( type );
    
    glm::uvec2 size(width,height);
    int mipmaps = mipmapForSize( size );
    
    glTexStorage2D( GL_TEXTURE_2D, mipmaps, internalFormat, size.x, size.y );
    glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, size.x, size.y, GL_RGBA, GL_UNSIGNED_BYTE, pixels );

    glGenerateMipmap( GL_TEXTURE_2D );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    return makeSharedPtr<Texture>( type, size, glTexture );
}

SharedPtr<Texture> Texture::CreateTexture( TextureType type, const glm::uvec2& size, GLuint mipmaps )
{
    GLuint glTexture;
    GLint internalFormat = typeToGLFormat( type );

    glGenTextures( 1, &glTexture );
    glBindTexture( GL_TEXTURE_2D, glTexture );
    
    if( mipmaps == 0 ) {
        mipmaps = mipmapForSize( size );
    }
    
    glTexStorage2D( GL_TEXTURE_2D, mipmaps, internalFormat, size.x, size.y );
    
    if( mipmaps > 1 ) {
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    }
    else {
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    }
    
    return std::make_shared<Texture>( type, size, glTexture );
}
