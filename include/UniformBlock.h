#pragma once

#include <string>
#include <map>
#include <vector>

#include "FixedSizeTypes.h"

class GpuProgram;

enum class UniformType {
    Float,
    Vec2,
    Vec3,
    Vec4,
    Mat3,
    Mat4
};

size_t sizeOfUniformType( UniformType type );
UniformType uniformTypeFromString( const std::string &str );

/**
 * Class UniformBlockLayout
 * Helper class to UniformBuffer and is used to load, get and validate
 * uniform blocks for GpuProgram
 */
class UniformBlockLayout {
public:
    static UniformBlockLayout LoadFromProgram( GpuProgram *program, const char *blockName );

public:
    UniformBlockLayout() = default;
    UniformBlockLayout( const UniformBlockLayout& ) = default;
    UniformBlockLayout( UniformBlockLayout &&move ) :
        mSize(move.mSize),
        mUniforms(std::move(move.mUniforms))
    {}
    
    UniformBlockLayout& operator = ( const UniformBlockLayout& ) = default;
    UniformBlockLayout& operator = ( UniformBlockLayout &&move )
    {
        mSize = move.mSize;
        mUniforms = std::move(move.mUniforms);
        return *this;
    }

public:
    // checks whenether we can use this uniform block instead of 'other'
    // we can do so, if 'other' is a subset of this one, 
    // aka all uniforms that exists in other must have a matching one in this one
    bool canBeUsedAs( const UniformBlockLayout &other ) const;
    
    void setSize( size_t size );
    void setUniform( const std::string &name, UniformType type, size_t offset );
    
    size_t getSize() const {
        return mSize;
    }
    size_t getOffset( const std::string &name ) const;
    UniformType getType( const std::string &name ) const;
    
    size_t getOffsetValidate( const std::string &name, UniformType type ) const;
    
private:
    struct UniformInfo {
        UniformType type;
        size_t offset;
    };
    
private:
    size_t mSize = 0;
    std::map<std::string,UniformInfo> mUniforms;
};

