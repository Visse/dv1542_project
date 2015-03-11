#pragma once

#include "GLTypes.h"
#include "SharedPtr.h"
#include "UniformBlock.h"

#include <vector>
#include <map>
#include <string>

class GpuShader;

class GpuProgramBuilder {
public:
    void addShader( const SharedPtr<GpuShader> &shader ) {
        mShaders.push_back( shader );
    }
    void setOutput( const std::string &name, int index ) {
        mOutputs[name] = index;
    }
    void setAttribute( const std::string &name, int index ) {
        mAttributes[name] = index;
    }
    void setSampler( const std::string &name, int unit ) {
        mSamplers[name] = unit;
    }
    void setUniformBlock( const std::string &name, int index ) {
        mUniformBlocks[name] = index;
    }
    
    SharedPtr<GpuProgram> buildProgram();
    
private:
    std::vector<SharedPtr<GpuShader>> mShaders;
    std::map<std::string,int> mOutputs, 
                              mAttributes, 
                              mSamplers,
                              mUniformBlocks;
};

class GpuProgram {
public:
    static SharedPtr<GpuProgram> LoadProgram( const std::string &filename );
    
public:
    GpuProgram( GLuint program );
    ~GpuProgram();
    
    GpuProgram( const GpuProgram& ) = delete;
    GpuProgram( GpuProgram&& ) = delete;
    GpuProgram& operator = ( const GpuProgram& ) = delete;
    GpuProgram& operator = ( GpuProgram&& ) = delete;
    
    void bindProgram();
    void unbindProgram();
    
    GLuint getGLProgram() {
        return mProgram;
    }
    
private:
    GLuint mProgram;
};