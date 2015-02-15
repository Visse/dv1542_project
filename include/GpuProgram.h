#pragma once

#include "GLTypes.h"
#include "SharedPtr.h"
#include "UniformBlock.h"

#include <vector>
#include <map>
#include <string>

class GpuShader;

class GpuProgram {
public:
    static SharedPtr<GpuProgram> CreateProgram( const std::vector<SharedPtr<GpuShader>> &shaders );
    
public:
    GpuProgram( GLuint program );
    ~GpuProgram();
    
    GpuProgram( const GpuProgram& ) = delete;
    GpuProgram( GpuProgram&& ) = delete;
    GpuProgram& operator = ( const GpuProgram& ) = delete;
    GpuProgram& operator = ( GpuProgram&& ) = delete;
    
    void bindProgram();
    void unbindProgram();
    
    GLint getAttribLocation( const std::string &name );
    GLint getUniformLocation( const std::string &name );
    GLint getUniformBlockLocation( const std::string &name );
    const UniformBlockLayout& getUniformBlockLayout( const std::string &name );
    
    GLuint getGLProgram() {
        return mProgram;
    }
    
private:
    GLuint mProgram;
    
    std::map<std::string,GLint> mUniformBlockLoc,
                                mUniformLoc,
                                mAttribLoc;
    
    std::map<std::string,UniformBlockLayout> mUniformBlocks;
};