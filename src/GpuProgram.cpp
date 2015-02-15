#include "GpuProgram.h"
#include "GpuShader.h"
#include "GLinclude.h"
#include "StringUtils.h"
#include "DefaultGpuProgramLocations.h"
#include "UniformBlockDefinitions.h"
#include "UniformBlock.h"

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

GLint GpuProgram::getUniformBlockLocation( const std::string &name )
{
    auto iter = mUniformBlockLoc.find( name );
    if( iter == mUniformBlockLoc.end() ) {
        return GL_INVALID_INDEX;
    }
    return iter->second;
}

const UniformBlockLayout &GpuProgram::getUniformBlockLayout( const std::string &name )
{
    auto iter = mUniformBlocks.find(name);
    if( iter == mUniformBlocks.end() ) {
        iter = mUniformBlocks.emplace(name, UniformBlockLayout::LoadFromProgram(this,name.c_str())).first;
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
    
    SharedPtr<GpuProgram> gpuProgram = makeSharedPtr<GpuProgram>(program);
    
    for( int i=0, c=static_cast<int>(DefaultUniformBlockLocations::COUNT); i < c; ++i )
    { // set up default uniform blocks
        GLuint binding = getDefaultUniformBlockBinding( static_cast<DefaultUniformBlockLocations>(i) );
        const char *name = getDefaultUniformBlockName( static_cast<DefaultUniformBlockLocations>(i) );
        GLuint index = glGetUniformBlockIndex( program, name );
        if( index != GL_INVALID_INDEX ) {
            glUniformBlockBinding( program, index, binding );
            gpuProgram->mUniformBlockLoc.emplace( name, binding );
        }
    }
    
    GLint blockCount;
    int nextBinding = static_cast<int>(DefaultUniformBlockLocations::COUNT);
    
    glGetProgramInterfaceiv( program, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &blockCount );
    for( GLint i=0; i < blockCount; ++i ) {
        std::string name;
        GLint length;
        glGetActiveUniformBlockiv( program, i, GL_UNIFORM_BLOCK_NAME_LENGTH, &length );
        name.resize(length);
        
        glGetActiveUniformBlockName( program, i, length, NULL, &name[0] );
        name.pop_back();
        
        if( gpuProgram->mUniformBlockLoc.find(name) == gpuProgram->mUniformBlockLoc.end() ) {
            glUniformBlockBinding( program, i, nextBinding );
            gpuProgram->mUniformBlockLoc.emplace( name, nextBinding );
            nextBinding++;
        }
    }
    
    {
        const char *name = getDefaultUniformBlockName( DefaultUniformBlockLocations::SceneInfo );
        const UniformBlockLayout &sceneUniforms = SceneRenderUniforms::GetUniformBlockLayout();
        
        UniformBlockLayout programSceneBlock = UniformBlockLayout::LoadFromProgram( gpuProgram.get(), name );
        
        if( !sceneUniforms.canBeUsedAs(programSceneBlock) ) {
            throw std::runtime_error( StringUtils::strjoin("Program has a invalid or outdated UniformBlock \"", name,
                                      "\", please make sure they match the definition of SceneRenderUniforms.") );
        }
    }
    
    return gpuProgram;
}

