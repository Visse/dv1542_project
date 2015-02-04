#include "Material.h"
#include "GpuProgram.h"
#include "Texture.h"
#include "GLinclude.h"
#include "ResourceManager.h"
#include "GlmStream.h"
#include "GpuBuffer.h"
#include "DefaultGpuProgramLocations.h"

#include <glm/gtc/epsilon.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <yaml-cxx/YamlCxx.h>

#include <iostream>

/*
struct TextureUnitLookup {
    bool operator () ( int u1, const std::pair<int,SharedPtr<Texture>> &u2 ) const {
        return u1 < u2.first;
    }
    bool operator () ( const std::pair<int,SharedPtr<Texture>> &u1, int u2 ) const {
        return u1.first < u2;
    }
};
*/


BlendMode blendModeFromString( const std::string &str )
{
    if( StringUtils::equalCaseInsensitive(str,"Replace") ) {
        return BlendMode::Replace;
    }
    else if( StringUtils::equalCaseInsensitive(str, "AlphaBlend") ) {
        return BlendMode::AlphaBlend;
    }
    else if( StringUtils::equalCaseInsensitive(str, "AddjectiveBlend") ) {
        return BlendMode::AddjectiveBlend;
    }
    throw std::string( StringUtils::strjoin("String (\"",str,"\") isn't a valid BlendMode!") );
}

DepthCheck depthCheckFromString( const std::string &str )
{
    if( StringUtils::equalCaseInsensitive(str,"None") ) {
        return DepthCheck::None;
    }
    else if( StringUtils::equalCaseInsensitive(str,"Less") ) {
        return DepthCheck::Less;
    }
    else if( StringUtils::equalCaseInsensitive(str,"Equal") ) {
        return DepthCheck::Equal;
    }
    else if( StringUtils::equalCaseInsensitive(str,"Greater") ) {
        return DepthCheck::Greater;
    }
    throw std::string( StringUtils::strjoin("String (\"",str,"\") isn't a valid DepthCheck!") );
}

Material::Material( const SharedPtr<GpuProgram> &program, const SharedPtr<GpuBuffer> &materialUniforms ) :
    mProgram(program),
    mMaterialUnifoms(materialUniforms)
{
}

void Material::setProgram( const SharedPtr<GpuProgram> &program )
{
    mProgram = program;
}

void Material::setTexture( GLint loc, GLint unit, const SharedPtr<Texture> &texture  )
{
    if( loc < 0 ) return;
    
    auto iter = std::lower_bound( mTextures.begin(), mTextures.end(), unit );
    if( iter != mTextures.end() && iter->loc == loc ) {
        iter->texture = texture;
    }
    else {
        mTextures.emplace( iter, TextureInfo{loc, unit, texture} );
    }
}

void Material::setTexture( const std::string &name, GLint unit, const SharedPtr<Texture> &texture  )
{
    GLint loc = mProgram->getUniformLocation( name );
    setTexture( loc, unit, texture );
}

void Material::setBlendMode( BlendMode mode )
{
    mBlend = mode;
}

void Material::setDepthWrite( bool depthWrite )
{
    mDepthWrite = depthWrite;
}

void Material::setDepthCheck( DepthCheck check )
{
    mDepthCheck = check;
}

void Material::bindTextureUnit( const std::string &name, GLint unit )
{
    GLint loc = mProgram->getUniformLocation( name );
    setTexture( loc, unit, SharedPtr<Texture>() );
}

SharedPtr<GpuProgram> Material::getProgram()
{
    return mProgram;
}

void Material::bindMaterial()
{
    mProgram->bindProgram();
    for( const TextureInfo &texEntry : mTextures ) {
        glUniform1i( texEntry.loc, texEntry.unit );
        const SharedPtr<Texture> &texture = texEntry.texture;
        if( texture ) {
            texture->bindTexture( texEntry.unit );
        }
    }
    
    switch( mBlend ) {
    case( BlendMode::Replace ):
        glBlendFunc( GL_ONE, GL_ZERO );
        break;
    case( BlendMode::AlphaBlend ):
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        break;
    case( BlendMode::AddjectiveBlend ):
        glBlendFunc( GL_ONE, GL_ONE );
        break;
    }
    switch( mDepthCheck ) {
    case( DepthCheck::None ):
        glDepthFunc( GL_ALWAYS );
        break;
    case( DepthCheck::Less ):
        glDepthFunc( GL_LESS );
        break;
    case( DepthCheck::Equal ):
        glDepthFunc( GL_EQUAL );
        break;
    case( DepthCheck::Greater ):
        glDepthFunc( GL_GREATER );
        break;
    }
    glDepthMask( mDepthWrite ? GL_TRUE : GL_FALSE );
    
    if( mMaterialUnifoms ) {
        mMaterialUnifoms->bindIndexed( getDefaultUniformBlockBinding(DefaultUniformBlockLocations::Material) );
    }
}

SharedPtr<Material> Material::LoadFromFile( const std::string &filename, ResourceManager *resourceMgr )
{
    Yaml::MappingNode settings = Yaml::Node::LoadFromFile(filename).asMapping();
    std::string programName = settings.getFirstValue("Program", false).asValue().getValue();
    
    SharedPtr<GpuProgram> program = resourceMgr->getGpuProgramAutoPack( programName );
    if( !program ) {
        return SharedPtr<Material>();
    }
    
    GLuint glProgram = program->getGLProgram();
    
    struct UniformInfo {
        std::string name;
        
        GLint type, typeSize;
        // max size of value is a 4x4 matrix (16 elements)
        uint32_t value[16];
    };
    
    auto fillUniformValue = []( UniformInfo &info, const std::string &type, const std::string &value ) {
        if( StringUtils::equalCaseInsensitive(type,"Float") ) {
            info.type = GL_FLOAT;
            info.typeSize = 4;
            float tmp = StringUtils::fromString<float>(value);
            memcpy( info.value, &tmp, 4 );
        }
        else if( StringUtils::equalCaseInsensitive(type,"Vec2") ) {
            info.type = GL_FLOAT_VEC2;
            info.typeSize = 8;
            glm::vec2 tmp = StringUtils::fromString<glm::vec2>(value);
            memcpy( info.value, glm::value_ptr(tmp), 4*2 );
        }
        else if( StringUtils::equalCaseInsensitive(type,"Vec3") ) {
            info.type = GL_FLOAT_VEC3;
            info.typeSize = 12;
            glm::vec3 tmp = StringUtils::fromString<glm::vec3>(value);
            memcpy( info.value, glm::value_ptr(tmp), 4*3 ); 
        }
        else if( StringUtils::equalCaseInsensitive(type,"Vec4") ) {
            info.type = GL_FLOAT_VEC4;
            info.typeSize = 16;
            glm::vec4 tmp = StringUtils::fromString<glm::vec4>(value);
            memcpy( info.value, glm::value_ptr(tmp), 4*4 );
        }
    };
    
    std::vector<UniformInfo> uniforms;
    SharedPtr<GpuBuffer> uniformBuffer;
    
    GLuint blockIndex = glGetUniformBlockIndex(glProgram, "Material");
    
    if( blockIndex != GL_INVALID_INDEX ) {
    
        { // Load uniforms settings
            auto uniformList = settings.getValues("Uniform");
            
            for( Yaml::Node uniformNode : uniformList ) {
                Yaml::MappingNode settings = uniformNode.asMapping();
                
                std::string name = settings.getFirstValue("Name",false).asValue().getValue(),
                            type = settings.getFirstValue("Type",false).asValue().getValue(),
                            value = settings.getFirstValue("Value",false).asValue().getValue();
                
                UniformInfo info;
                    info.name = name;
                fillUniformValue( info, type, value );
                
                uniforms.push_back( info );
            }
        }
        
        std::vector<GLuint> uniformIndex;
        { // Find offsets of uniforms in the program
            std::vector<const char*> uniformNames;
            uniformNames.reserve(uniforms.size());
            uniformIndex.resize(uniforms.size());
            
            for( UniformInfo info : uniforms ) {
                uniformNames.push_back( info.name.c_str() );
            }
            
            glGetUniformIndices( glProgram, uniforms.size(), uniformNames.data(), &uniformIndex[0] );
        }
        
        GLint blockSize;
        glGetActiveUniformBlockiv( glProgram, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize );
        
        uniformBuffer = makeSharedPtr<GpuBuffer>();
        uniformBuffer->setType( BufferType::Uniforms );
        uniformBuffer->setSize( blockSize );
        
        {
            std::vector<GLint> uniformType, uniformSize, uniformOffset;
            uniformType.resize( uniforms.size() );
            uniformSize.resize( uniforms.size() );
            uniformOffset.resize( uniforms.size() );
                
            glGetActiveUniformsiv( glProgram, uniforms.size(), uniformIndex.data(), GL_UNIFORM_TYPE, &uniformType[0] );
            glGetActiveUniformsiv( glProgram, uniforms.size(), uniformIndex.data(), GL_UNIFORM_OFFSET, &uniformOffset[0] );
            glGetActiveUniformsiv( glProgram, uniforms.size(), uniformIndex.data(), GL_UNIFORM_SIZE, &uniformSize[0] );
            
            uint8_t *data = uniformBuffer->mapBuffer<uint8_t>(BufferUsage::WriteOnly);
            
            for( size_t i=0, count=uniforms.size(); i < count; ++i ) {
                if( uniformIndex[i] != GL_INVALID_INDEX ) {
                    if( uniformType[i] != uniforms[i].type ) {
                        throw std::runtime_error( "Mismatching uniform type!" );
                    }
                    memcpy( data + uniformOffset[i], uniforms[i].value, uniforms[i].typeSize );
                }
            }
            
            uniformBuffer->unmapBuffer();
        }
    }
    
    SharedPtr<Material> material = makeSharedPtr<Material>( program, uniformBuffer );
    
    auto depthWrite = settings.getFirstValue("DepthWrite");
    if( depthWrite ) {
        material->setDepthWrite( depthWrite.asValue().getValue<bool>() );
    }
    
    auto depthCheck = settings.getFirstValue("DepthCheck");
    if( depthCheck ) {
        std::string depthCheckStr = depthCheck.asValue().getValue();
        material->setDepthCheck( depthCheckFromString(depthCheckStr) );
    }
    
    auto blendMode = settings.getFirstValue("BlendMode");
    if( blendMode ) {
        std::string blendModeStr = blendMode.asValue().getValue();
        material->setBlendMode( blendModeFromString(blendModeStr) );
    }
    
    auto textures = settings.getValues("Texture");
    
    int textureUnit = 0;
    for( Yaml::Node textureNode : textures ) {
        Yaml::MappingNode settings = textureNode.asMapping();
        
        std::string name = settings.getFirstValue("Name").asValue().getValue();
        std::string textureName = settings.getFirstValue("Texture").asValue().getValue();
        
        SharedPtr<Texture> texture = resourceMgr->getTextureAutoPack( textureName );
        if( texture ) {
            material->setTexture( name, textureUnit, texture );
            textureUnit++;
        }
        else {
            // @todo add propper logging for missing texture
            std::cerr << "Missing texture \"" << textureName << "\" for material \"" << filename;
        }
    }
    
    return material;
}










