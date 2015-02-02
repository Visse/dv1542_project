#pragma once

#include "GLTypes.h"
#include "SharedPtr.h"

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
    
    
    GLuint getGLProgram() {
        return mProgram;
    }
private:
    GLuint mProgram;
    
    std::map<std::string,GLint> mUniformLoc,
                                mAttribLoc;
};