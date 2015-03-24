#pragma once

#include <map>
#include <string>
#include <vector>

#include "SharedPtr.h"
#include "BaseManager.h"

class Mesh;
class Texture;
class Material;
class GpuProgram;
class Log;


/** class ResourceManager
 *      This class handles, as the name suggests, resources :)
 *      Loading is done thrue resources packages. The reason 
 *      being that I don't have to deal with resource dependencies,
 *      to the extent as individual loading would bring, and that 
 *      I, if needed, can optimize the loading of the pack more
 *      easy than I could have optimized individual loading.
 *      The last reason is simply that I wanted to try something new :)
 */
class ResourceManager :
    public BaseManager
{
public:
    virtual bool init( Root* root ) override;
    virtual void destroy() override;
    
    void loadResourcePack( const std::string &name );
    void unloadResourcePack( const std::string &name );
    
    void unloadAllResourcePacks();
    
    SharedPtr<GpuProgram> getGpuProgram( const std::string &pack, const std::string &name );
    SharedPtr<Texture> getTexture( const std::string &pack, const std::string &name );
    SharedPtr<Mesh> getMesh( const std::string &pack, const std::string &name );
    
    // automaticly detect resource pack (to specify a pack, prefix the name with 'pack:')
    SharedPtr<GpuProgram> getGpuProgramAutoPack( const std::string &name );
    SharedPtr<Texture> getTextureAutoPack( const std::string &name );
    SharedPtr<Mesh> getMeshAutoPack( const std::string &name );
    
private:
    void loadCompressedResourcePack( const std::string &name, const std::string &path );
    void loadUncompressedResourcePack( const std::string &name, const std::string &path );
    
private:
    typedef SharedPtr<GpuProgram> GpuProgramPtr;
    typedef SharedPtr<Texture> TexturePtr;
    typedef SharedPtr<Mesh> MeshPtr;
    typedef SharedPtr<Material> MaterialPtr;
    
    struct ResourcePack {
        std::map<std::string,GpuProgramPtr> programs;
        std::map<std::string,TexturePtr> textures;
        std::map<std::string,MeshPtr> meshes;
    };
    typedef std::map<std::string,ResourcePack> ResourcePackMap;
    
private:
    Root *mRoot;
    Log *mLog;
    std::vector<std::string> mResourcePaths;
    ResourcePackMap mResourcePacks;
};