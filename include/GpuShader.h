#pragma once

#include "GLTypes.h"
#include "SharedPtr.h"

#include <string>

enum class ShaderType {
    Vertex, 
    Fragment,
    Geometry,
    TessControl,
    TessEval,
    
    Compute
};

ShaderType stringToShaderType( const std::string &str );

class GpuShader {
public:
    static SharedPtr<GpuShader> LoadShaderFromFile( ShaderType type, const std::string &filename );
    static SharedPtr<GpuShader> LoadShaderFromMemory( ShaderType type, const void *source, size_t size );
    
public:
    GpuShader( ShaderType type, GLuint shader );
    ~GpuShader();
    
    GpuShader( const GpuShader& ) = delete;
    GpuShader( GpuShader&& ) = delete;
    GpuShader& operator = ( const GpuShader& ) = delete;
    GpuShader& operator = ( GpuShader&& ) = delete;
    
    GLuint getGLShader() {
        return mShader;
    }
private:
    ShaderType mType;
    GLuint mShader;
};