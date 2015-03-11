#include "GpuProgram.h"
#include "GpuShader.h"
#include "GLinclude.h"
#include "StringUtils.h"
#include "DefaultGpuProgramLocations.h"
#include "UniformBlockDefinitions.h"
#include "UniformBlock.h"
#include "yaml-cxx/YamlCxx.h"

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

SharedPtr<GpuProgram> GpuProgram::LoadProgram( const std::string &filename )
{
    GpuProgramBuilder builder;
    
    Yaml::MappingNode config = Yaml::Node::LoadFromFile(filename).asMapping();
    
    if( !config ) {
        throw std::runtime_error( StringUtils::strjoin("Failed to load file \"",filename,"\".")  );
    }
    
    std::string resourcePrefix;
    {
        size_t index = filename.find_last_of( '/' );
        if( index != std::string::npos ) {
            resourcePrefix = filename.substr( 0, index+1 );
        }
    }
    
    auto shaders = config.getValues("Shader");
    for( const auto entry : shaders ) {
        Yaml::MappingNode config = entry.asMapping();  
        
        std::string strType = config.getFirstValue("Type",false).asValue().getValue();
        std::string source  = config.getFirstValue("Source",false).asValue().getValue();
        
        ShaderType type = stringToShaderType(strType);
        
        std::string filename = resourcePrefix + source;
        SharedPtr<GpuShader> shader = GpuShader::LoadShaderFromFile(type,filename);
        
        builder.addShader( shader );
    }
    
    auto samplers = config.getValues( "Sampler" );
    for( const auto entry : samplers ) {
        Yaml::MappingNode config = entry.asMapping();
        
        std::string name = config.getFirstValue("Name",false).asValue().getValue();
        int unit = config.getFirstValue("Unit",false).asValue().getValue<int>();
        
        if( !name.empty() && unit >= 0 ) {
            builder.setSampler( name, unit );
        }
    }
    
    auto uniformBlocks = config.getValues( "UniformBlock" );
    for( const auto entry : uniformBlocks ) {
        Yaml::MappingNode config = entry.asMapping();
        
        std::string name = config.getFirstValue("Name",false).asValue().getValue();
        int index = config.getFirstValue("Index",false).asValue().getValue<int>();
        
        if( !name.empty() && index >= 0 ) {
            builder.setUniformBlock( name, index );
        }
    }
    
    auto outputs = config.getValues( "Output" );
    for( const auto entry : outputs ) {
        Yaml::MappingNode config = entry.asMapping();
        
        std::string name = config.getFirstValue("Name",false).asValue().getValue();
        int index = config.getFirstValue("Index",false).asValue().getValue<int>();
        
        if( !name.empty() && index >= 0 ) {
            builder.setOutput( name, index );
        }
    }
    
    auto attributes = config.getValues( "Attribute" );
    for( const auto entry : attributes ) {
        Yaml::MappingNode config = entry.asMapping();
        
        std::string name = config.getFirstValue("Name",false).asValue().getValue();
        int index = config.getFirstValue("Index",false).asValue().getValue<int>();
        
        if( !name.empty() && index >= 0 ) {
            builder.setAttribute( name, index );
        }
    }
    
    return builder.buildProgram();
}


SharedPtr<GpuProgram> GpuProgramBuilder::buildProgram()
{
    GLuint glprogram = glCreateProgram();
    SharedPtr<GpuProgram> program = makeSharedPtr<GpuProgram>( glprogram );
    
    for( const SharedPtr<GpuShader> &shader : mShaders ) {
        glAttachShader( glprogram, shader->getGLShader() );
    }
    
    for( const auto &entry : mOutputs ) {
        const std::string &name = entry.first;
        GLuint loc = entry.second;
        glBindFragDataLocation( glprogram, loc, name.c_str() );
    }
    for( const auto &entry : mAttributes ) {
        const std::string &name = entry.first;
        GLuint loc = entry.second;
        glBindAttribLocation( glprogram, loc, name.c_str() );
    }

    glLinkProgram( glprogram );

    GLint linkStatus;
    glGetProgramiv( glprogram, GL_LINK_STATUS, &linkStatus );
    if( linkStatus != GL_TRUE ) {
        GLint logSize;
        glGetProgramiv( glprogram, GL_INFO_LOG_LENGTH, &logSize );
        std::string log;
        log.resize( logSize );
        
        glGetProgramInfoLog( glprogram, logSize, NULL, &log[0] );
        glDeleteProgram( glprogram );
        throw std::runtime_error( StringUtils::strjoin("Failed to link program, reason: ",log) );
    }
    
    for( const auto &entry : mUniformBlocks ) {
        const std::string &name = entry.first;
        GLuint binding = entry.second;
        
        GLuint index = glGetUniformBlockIndex( glprogram, name.c_str() );
        if( index != GL_INVALID_INDEX ) {
            glUniformBlockBinding( glprogram, index, binding );
        }
    }
    
    glUseProgram( glprogram );
    for( const auto &entry : mSamplers ) {
        const std::string &name = entry.first;
        GLuint unit = entry.second;
        
        GLint loc = glGetUniformLocation( glprogram, name.c_str() );
        if( loc != -1 ) {
            glUniform1i( loc, unit );
        }
    }
    glUseProgram( 0 );
    
    return program;
}

