#include "UniformBlock.h"
#include "StringUtils.h"
#include "GLinclude.h"
#include "GpuProgram.h"

#include <cassert>

size_t sizeOfUniformType( UniformType type )
{
    switch( type ) {
    case( UniformType::Float ):
        return 4;
    case( UniformType::Vec2 ):
        return 8;
    case( UniformType::Vec3 ):
        return 12;
    case( UniformType::Vec4 ):
        return 16;
    case( UniformType::Mat3 ):
        return 36;
    case( UniformType::Mat4 ):
        return 64;
    }
    assert( false && "Invalid UniformType" );
    return 0;
}

UniformType uniformTypeFromString( const std::string &str )
{
    if( StringUtils::equalCaseInsensitive(str,"Float") ) {
        return UniformType::Float;
    }
    else if( StringUtils::equalCaseInsensitive(str,"Vec2") ) {
        return UniformType::Vec2;
    }
    else if( StringUtils::equalCaseInsensitive(str,"Vec3") ) {
        return UniformType::Vec3;
    }
    else if( StringUtils::equalCaseInsensitive(str,"Vec4") ) {
        return UniformType::Vec4;
    }
    else if( StringUtils::equalCaseInsensitive(str,"Mat3") ) {
        return UniformType::Mat3;
    }
    else if( StringUtils::equalCaseInsensitive(str,"Mat4") ) {
        return UniformType::Mat4;
    }
    assert( false && "Invalid UniformType" );
    return UniformType::Float;
}

UniformType glToUniformType( GLint type )
{
    switch( type ) {
    case( GL_FLOAT ):
        return UniformType::Float;
    case( GL_FLOAT_VEC2 ):
        return UniformType::Vec2;
    case( GL_FLOAT_VEC3 ):
        return UniformType::Vec3;
    case( GL_FLOAT_VEC4 ):
        return UniformType::Vec4;
    case( GL_FLOAT_MAT3 ):
        return UniformType::Mat3;
    case( GL_FLOAT_MAT4 ):
        return UniformType::Mat4;
    }
    assert( false && "Invalid UniformType" );
    return UniformType::Float;
}

UniformBlockLayout UniformBlockLayout::LoadFromProgram( GpuProgram *program, const char *blockName )
{
    GLuint glProgram = program->getGLProgram();
    GLuint blockIndex = glGetUniformBlockIndex( glProgram, blockName );
    
    if( blockIndex == GL_INVALID_INDEX ) {
        return UniformBlockLayout();
    }
    
    GLint blockSize, uniformCount;
    
    glGetActiveUniformBlockiv( glProgram, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize );
    glGetActiveUniformBlockiv( glProgram, blockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &uniformCount );
    
    std::vector<GLuint> uniformIndices( uniformCount );
    
    glGetActiveUniformBlockiv( glProgram, blockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, reinterpret_cast<GLint*>(&uniformIndices[0]) );
    
    std::vector<GLint> uniformType(uniformCount), 
                       uniformOffset(uniformCount),
                       uniformNameLenght(uniformCount);

    glGetActiveUniformsiv( glProgram, uniformCount, uniformIndices.data(), GL_UNIFORM_TYPE, &uniformType[0] );
    glGetActiveUniformsiv( glProgram, uniformCount, uniformIndices.data(), GL_UNIFORM_OFFSET, &uniformOffset[0] );
    glGetActiveUniformsiv( glProgram, uniformCount, uniformIndices.data(), GL_UNIFORM_NAME_LENGTH, &uniformNameLenght[0] );
    
    UniformBlockLayout block;
    block.setSize( blockSize );
    
    for( GLint i=0; i < uniformCount; ++i ) {
        std::string name( uniformNameLenght[i], ' ' );
        glGetActiveUniformName( glProgram, uniformIndices[i], uniformNameLenght[i], NULL, &name[0] );
        
        // remove the terminating '\0'
        name.pop_back();
        
        UniformType type = glToUniformType( uniformType[i] );
        size_t offset = uniformOffset[i];
        block.setUniform( name, type, offset );
    }
    
    return block;
}

void UniformBlockLayout::setSize( size_t size )
{
    mSize = size;
}

void UniformBlockLayout::setUniform( const std::string &name, UniformType type, size_t offset )
{
    UniformInfo info;
        info.type = type;
        info.offset = offset;
    
    mUniforms.emplace( name, info );
}

size_t UniformBlockLayout::getOffset( const std::string &name ) const
{
    auto iter = mUniforms.find(name);
    if( iter == mUniforms.end() ) {
        throw std::runtime_error(StringUtils::strjoin("Uniformblock doesn't contain \"",name,"\".") );
    }
    return iter->second.offset;
}

UniformType UniformBlockLayout::getType( const std::string &name ) const
{
    auto iter = mUniforms.find(name);
    if( iter == mUniforms.end() ) {
        throw std::runtime_error(StringUtils::strjoin("UniformBlock doesn't contain \"",name,"\".") );
    }
    return iter->second.type;
}

size_t UniformBlockLayout::getOffsetValidate( const std::string &name, UniformType type ) const
{
    auto iter = mUniforms.find(name);
    if( iter == mUniforms.end() ) {
        throw std::runtime_error(StringUtils::strjoin("UniformBlock doesn't contain \"",name,"\".") );
    }
    const UniformInfo &info = iter->second;
    if( info.type != type ) {
        throw std::runtime_error(StringUtils::strjoin("Uniform \"",name,"\" doesn't match the type in the UniformBlock!") );
    }
    return info.offset;
}


bool UniformBlockLayout::canBeUsedAs( const UniformBlockLayout &other ) const
{
    if( mSize < other.mSize ) {
        return false;
    }
    
    for( const auto &entry : other.mUniforms ) {
        auto iter = mUniforms.find( entry.first );
        if( iter == mUniforms.end() ) {
            // no uniform with that name
            return false;
        }
        
        const UniformInfo &i1 = entry.second;
        const UniformInfo &i2 = iter->second;
        
        if( i1.type != i2.type ) {
            // missmatching types
            return false;
        }
        if( i1.offset != i2.offset ) {
            // missmatching offsets
            return false;
        }
    }
    return true;
}
