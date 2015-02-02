#include "GpuProgram.h"
#include "GpuShader.h"
#include "GLinclude.h"
#include "StringUtils.h"
#include "DefaultGpuProgramLocations.h"

GpuProgram::GpuProgram( GLuint program ) :
    mProgram(program)
{
}

GpuProgram::~GpuProgram()
{
    glDeleteProgram( mProgram );
}

void GpuProgram::bindProgram()
{
    glUseProgram( mProgram );
}

void GpuProgram::unbindProgram()
{
    glUseProgram( 0 );
}

GLint GpuProgram::getAttribLocation( const std::string &name )
{
    auto iter = mAttribLoc.find( name );
    if( iter == mAttribLoc.end() ) {
        GLint loc = glGetAttribLocation( mProgram, name.c_str() );
        mAttribLoc.emplace( name, loc );
        return loc;
    }
    return iter->second;
}

GLint GpuProgram::getUniformLocation( const std::string &name )
{
    auto iter = mUniformLoc.find( name );
    if( iter == mUniformLoc.end() ) {
        GLint loc = glGetUniformLocation( mProgram, name.c_str() );
        mUniformLoc.emplace( name, loc );
        return loc;
    }
    return iter->second;
}

SharedPtr<GpuProgram> GpuProgram::CreateProgram( const std::vector<SharedPtr<GpuShader>> &shaders )
{
    GLuint program = glCreateProgram();
    
    for( const SharedPtr<GpuShader> &shader : shaders ) {
        glAttachShader( program, shader->getGLShader() );
    }
    
    
    for( int i=0, c=static_cast<int>(DefaultAttributeLocations::COUNT); i < c; ++i ) 
    { // set up default attributes
        GLuint loc = getDefaultAttributeLocation( static_cast<DefaultAttributeLocations>(i) );
        const char *name = getDefaultAttributeName( static_cast<DefaultAttributeLocations>(i) );
        glBindAttribLocation( program, loc, name );
    }
    
    for( int i=0, c=static_cast<int>(DefaultOutputLocations::COUNT); i < c; ++i )
    { // set up default output
        GLuint loc = getDefaultOutputLocation( static_cast<DefaultOutputLocations>(i) );
        const char *name = getDefaultOutputName( static_cast<DefaultOutputLocations>(i) );
        glBindFragDataLocation( program, loc, name );
        
    }
    
    glLinkProgram( program );
    
    for( int i=0, c=static_cast<int>(DefaultUniformBlockLocations::COUNT); i < c; ++i )
    { // set up default uniform blocks
        GLuint binding = getDefaultUniformBlockBinding( static_cast<DefaultUniformBlockLocations>(i) );
        const char *name = getDefaultUniformBlockName( static_cast<DefaultUniformBlockLocations>(i) );
        GLuint index = glGetUniformBlockIndex( program, name );
        if( index != GL_INVALID_INDEX ) {
            glUniformBlockBinding( program, index, binding );
        }
    }
    
    GLint linkStatus;
    glGetProgramiv( program, GL_LINK_STATUS, &linkStatus );
    if( linkStatus != GL_TRUE ) {
        GLint logSize;
        glGetProgramiv( program, GL_INFO_LOG_LENGTH, &logSize );
        std::string log;
        log.resize( logSize );
        
        glGetProgramInfoLog( program, logSize, NULL, &log[0] );
        glDeleteProgram( program );
        throw std::runtime_error( StringUtils::strjoin("Failed to link program, reason: ",log) );
    }
    
    return makeSharedPtr<GpuProgram>(program);
}

