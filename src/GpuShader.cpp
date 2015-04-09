#include "GpuShader.h"
#include "GLinclude.h"
#include "FileUtils.h"
#include "StringUtils.h"

ShaderType stringToShaderType( const std::string &str )
{
    if( StringUtils::equalCaseInsensitive(str,"Vertex") ) {
        return ShaderType::Vertex;
    }
    if( StringUtils::equalCaseInsensitive(str,"Fragment") ) {
        return ShaderType::Fragment;
    }
    if( StringUtils::equalCaseInsensitive(str,"Compute") ) {
        return ShaderType::Compute;
    }
    if( StringUtils::equalCaseInsensitive(str,"Geometry") ) {
        return ShaderType::Geometry;
    }
    if( StringUtils::equalCaseInsensitive(str,"TessControl") ) {
        return ShaderType::TessControl;
    }
    if( StringUtils::equalCaseInsensitive(str,"TessEval") ) {
        return ShaderType::TessEval;
    }
    
    throw std::runtime_error( StringUtils::strjoin("String isn't a ShaderType (\"",str,"\")") );
}

GLenum shaderTypeToGL( ShaderType type )
{
    switch( type ) {
    case( ShaderType::Vertex ):
        return GL_VERTEX_SHADER;
    case( ShaderType::Fragment ):
        return GL_FRAGMENT_SHADER;
    case( ShaderType::Geometry ):
        return GL_GEOMETRY_SHADER;
    case( ShaderType::Compute ):
        return GL_COMPUTE_SHADER;
    case( ShaderType::TessControl ):
        return GL_TESS_CONTROL_SHADER;
    case( ShaderType::TessEval ):
        return GL_TESS_EVALUATION_SHADER;
    }
    
    throw std::runtime_error( "ShaderType isn't valid!" );
}


GpuShader::GpuShader( ShaderType type, GLuint shader ) :
    mType(type),
    mShader(shader)
{
}

GpuShader::~GpuShader()
{
    glDeleteShader( mShader );
}

SharedPtr<GpuShader> loadShader( ShaderType type, const void *source, size_t size, const std::string &sourceName );

SharedPtr<GpuShader> GpuShader::LoadShaderFromFile( ShaderType type, const std::string &filename )
{
    std::string source = FileUtils::getFileContent( filename );
    return loadShader(type, source.data(), source.size(), filename );
}

SharedPtr<GpuShader> GpuShader::LoadShaderFromMemory( ShaderType type, const void *source, size_t size )
{
    return loadShader( type, source, size, "" );
}

SharedPtr<GpuShader> loadShader( ShaderType type, const void *source, size_t size, const std::string &sourceName )
{
    GLenum glType = shaderTypeToGL( type );
    GLuint shader = glCreateShader( glType );
    
    GLint length = size;
    const GLchar *string = static_cast<const GLchar*>( source );
    glShaderSource( shader, 1, &string, &length );
    
    glCompileShader( shader );
    
    GLint compileStatus;
    glGetShaderiv( shader, GL_COMPILE_STATUS, &compileStatus );
    
    if( compileStatus != GL_TRUE ) {
        GLint logSize;
        glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logSize );
        std::string log;
        log.resize( logSize );
        
        glGetShaderInfoLog( shader, logSize, nullptr, &log[0] );
        glDeleteShader( shader );
        throw std::runtime_error( StringUtils::strjoin("Failed to compile shader \"",sourceName,"\", reason: ",log) );
    }
    
    return makeSharedPtr<GpuShader>( type, shader );
}
