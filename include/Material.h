#pragma once

#include <vector>
#include <map>
#include <string>

#include <glm/vec3.hpp>

#include "SharedPtr.h"
#include "GLTypes.h"

class GpuBuffer;
class GpuProgram;
class Texture;
class ResourceManager;

enum class BlendMode {
    Replace,
    AlphaBlend,
    AddjectiveBlend
};
BlendMode blendModeFromString( const std::string &str );

enum class DepthCheck {
    None,
    Less,
    Equal,
    Greater
};
DepthCheck depthCheckFromString( const std::string &str );

class Material {
public:
    static SharedPtr<Material> LoadFromFile( const std::string &filename, ResourceManager *resourceMgr );
    
public:
    Material( const SharedPtr<GpuProgram> &program, const SharedPtr<GpuBuffer> &materialUniforms );
    
    void setProgram( const SharedPtr<GpuProgram> &program );
    void setTexture( const std::string &name, GLint unit, const SharedPtr<Texture> &texture );
    void setBlendMode( BlendMode mode );
    void setDepthWrite( bool depthWrite );
    void setDepthCheck( DepthCheck check );
    
    SharedPtr<GpuProgram> getProgram();
    
    void bindMaterial();
    
private:
    void setTexture( GLint loc, GLint unit, const SharedPtr<Texture> &texture );
    
private:
    struct TextureInfo {
        GLint loc, unit;
        SharedPtr<Texture> texture;
        
        friend bool operator < ( GLint loc, const TextureInfo &info ) {
            return loc < info.loc;
        }
        friend bool operator < ( const TextureInfo &info, GLint loc ) {
            return info.loc < loc;
        }
    };
    
private:
    BlendMode mBlend = BlendMode::Replace;
    
    SharedPtr<GpuProgram> mProgram;
    SharedPtr<GpuBuffer> mMaterialUnifoms;
    
    std::vector<TextureInfo> mTextures;
    
    bool mDepthWrite = true;
    DepthCheck mDepthCheck = DepthCheck::Less;
};