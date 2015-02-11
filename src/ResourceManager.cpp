#include "ResourceManager.h"

#include "FileUtils.h"
#include "StringUtils.h"
#include "Texture.h"
#include "GpuShader.h"
#include "GpuProgram.h"
#include "Material.h"
#include "Mesh.h"
#include "StartupMesurements.h"
#include "Timer.h"
#include "Root.h"
#include "Config.h"

#include "yaml-cxx/YamlCxx.h"

#include <iostream>
#include <string>

bool ResourceManager::init( Root *root )
{
    mRoot = root;
    Timer initTimer;
    
    const Config *config = root->getConfig();
    
    mResourcePaths = config->resourcePaths;
    
    
    loadResourcePack( "core" );
    
    StartupMesurements *mesurements = mRoot->getStartupMesurements();
    mesurements->resourceStartup = initTimer.getTimeAsSeconds();
    
    return true;
}

void ResourceManager::destroy()
{
    unloadAllResourcePacks();
}

void ResourceManager::loadResourcePack( const std::string &name )
{
    if( mResourcePacks.count(name) > 0 ) {
        return;
    }
    
    for( const std::string &path : mResourcePaths ) {
        std::string filename = path + name;
        if( FileUtils::isFile(filename) ) {
            loadCompressedResourcePack(name,filename);
            break;
        }
        if( FileUtils::isDirectory(filename) ) {
            loadUncompressedResourcePack(name,filename);
            break;
        }
    }   
}

void ResourceManager::unloadResourcePack( const std::string &name )
{
    mResourcePacks.erase( name );
}

void ResourceManager::unloadAllResourcePacks()
{
    mResourcePacks.clear();
}

SharedPtr<GpuProgram> ResourceManager::getGpuProgram( const std::string &pack, const std::string &name )
{
    auto iterPack = mResourcePacks.find( pack );
    if( iterPack == mResourcePacks.end() ) {
        return GpuProgramPtr();
    }
    ResourcePack &resPack = iterPack->second;
    
    auto iter = resPack.programs.find( name );
    if( iter == resPack.programs.end() ) {
        return GpuProgramPtr();
    }
    return iter->second;
}

SharedPtr<Texture> ResourceManager::getTexture( const std::string &pack, const std::string &name )
{
    auto iterPack = mResourcePacks.find( pack );
    if( iterPack == mResourcePacks.end() ) {
        return TexturePtr();
    }
    ResourcePack &resPack = iterPack->second;
    
    auto iter = resPack.textures.find( name );
    if( iter == resPack.textures.end() ) {
        return TexturePtr();
    }
    return iter->second;
}

SharedPtr<Mesh> ResourceManager::getMesh( const std::string &pack, const std::string &name )
{
    auto iterPack = mResourcePacks.find( pack );
    if( iterPack == mResourcePacks.end() ) {
        return MeshPtr();
    }
    ResourcePack &resPack = iterPack->second;
    
    auto iter = resPack.meshes.find( name );
    if( iter == resPack.meshes.end() ) {
        return MeshPtr();
    }
    return iter->second;
}

SharedPtr<Material> ResourceManager::getMaterial( const std::string &pack, const std::string &name )
{
    auto iterPack = mResourcePacks.find( pack );
    if( iterPack == mResourcePacks.end() ) {
        return MaterialPtr();
    }
    ResourcePack &resPack = iterPack->second;
    
    auto iter = resPack.materials.find( name );
    if( iter == resPack.materials.end() ) {
        return MaterialPtr();
    }
    return iter->second;
}

SharedPtr<GpuProgram> ResourceManager::getGpuProgramAutoPack( const std::string &name )
{
    bool hasGroup = (name.find_first_of(':') != std::string::npos);
    
    if( hasGroup ) {
        auto packAndName = StringUtils::splitInTwo( name, ":" );
        
        GpuProgramPtr program = getGpuProgram( packAndName.first, packAndName.second );
        if( program ) {
            return program;
        }
    }
    
    for( const auto &entry : mResourcePacks ) {
        const ResourcePack &pack = entry.second;
        
        auto iter = pack.programs.find( name );
        if( iter != pack.programs.end() ) {
            return iter->second;
        }
    }
    return GpuProgramPtr();
}

SharedPtr<Texture> ResourceManager::getTextureAutoPack( const std::string &name )
{
    bool hasGroup = (name.find_first_of(':') != std::string::npos);
    
    if( hasGroup ) {
        auto packAndName = StringUtils::splitInTwo( name, ":" );
        
        TexturePtr texture = getTexture( packAndName.first, packAndName.second );
        if( texture ) {
            return texture;
        }
    }
    
    for( const auto &entry : mResourcePacks ) {
        const ResourcePack &pack = entry.second;
        
        auto iter = pack.textures.find( name );
        if( iter != pack.textures.end() ) {
            return iter->second;
        }
    }
    return TexturePtr();
}

SharedPtr<Mesh> ResourceManager::getMeshAutoPack( const std::string &name )
{
    bool hasGroup = (name.find_first_of(':') != std::string::npos);
    
    if( hasGroup ) {
        auto packAndName = StringUtils::splitInTwo( name, ":" );
        
        MeshPtr mesh = getMesh( packAndName.first, packAndName.second );
        if( mesh ) {
            return mesh;
        }
    }
    
    for( const auto &entry : mResourcePacks ) {
        const ResourcePack &pack = entry.second;
        
        auto iter = pack.meshes.find( name );
        if( iter != pack.meshes.end() ) {
            return iter->second;
        }
    }
    return MeshPtr();
}

SharedPtr<Material> ResourceManager::getMaterialAutoPack( const std::string &name )
{
    bool hasGroup = (name.find_first_of(':') != std::string::npos);
    
    if( hasGroup ) {
        auto packAndName = StringUtils::splitInTwo( name, ":" );
        
        MaterialPtr material = getMaterial( packAndName.first, packAndName.second );
        if( material ) {
            return material;
        }
    }
    
    for( const auto &entry : mResourcePacks ) {
        const ResourcePack &pack = entry.second;
        
        auto iter = pack.materials.find( name );
        if( iter != pack.materials.end() ) {
            return iter->second;
        }
    }
    return MaterialPtr();
}

void ResourceManager::loadCompressedResourcePack( const std::string &name, const std::string &path )
{
    /// @todo
}

void ResourceManager::loadUncompressedResourcePack( const std::string &name, const std::string &path )
{
    auto iter = mResourcePacks.emplace(name,ResourcePack()).first;
    ResourcePack &pack = iter->second;
    
    std::string resourcePrefix = path + "/";
    
    Yaml::MappingNode node = Yaml::Node::LoadFromFile(resourcePrefix+"resources.cfg").asMapping();
    
    auto textures = node.getValues( "Texture", false );
    auto programs = node.getValues( "GpuProgram", false );
    auto materials = node.getValues( "Material", false );
    auto meshes   = node.getValues( "Mesh", false );
    
    for( Yaml::Node textureNode : textures ) {
        Yaml::MappingNode settings = textureNode.asMapping();
        
        std::string source = settings.getValues("Source",false).at(0).asValue().getValue(),
                    texName   = settings.getValues("Name",false).at(0).asValue().getValue(),
                    strType   = settings.getValues("Type",false).at(0).asValue().getValue();
        
        try {
            TextureType type = stringToTextureType( strType );
            
            std::string filename = resourcePrefix + source;
            TexturePtr texture = Texture::LoadTexture( type, filename );
            
            if( texture ) {
                pack.textures.emplace( texName, texture );
                
                std::clog << "[ResourceManager] Loaded texture \"" << texName << "\" in resource pack \"" << name << "\"" << std::endl;
            } 
        } catch( const std::exception &e ) {
            std::cerr << "[ResourceManager] Failed to load texture \"" << texName << "\" in resource pack \"" << name << "\", error: " << e.what() << std::endl;
        }
    }
    
    for( Yaml::Node programNode : programs ) {
        Yaml::MappingNode settings = programNode.asMapping();
        
        std::string progName = settings.getValues("Name").at(0).asValue().getValue();
        
        Yaml::SequenceNode sources = settings.getValues("Shaders").at(0).asSequence();
        
        try {
            std::vector<SharedPtr<GpuShader>> shaders;
            for( size_t i=0, count=sources.getCount(); i < count; ++i ) {
                Yaml::MappingNode shaderNode = sources.getValue(i).asMapping();
                
                std::string strType = shaderNode.getValues("Type",false).at(0).asValue().getValue();
                std::string source  = shaderNode.getValues("Source",false).at(0).asValue().getValue();
                
                ShaderType type = stringToShaderType(strType);
                
                std::string filename = resourcePrefix + source;
                SharedPtr<GpuShader> shader = GpuShader::LoadShaderFromFile(type,filename);
                shaders.push_back( shader );
            }
            
            GpuProgramPtr program = GpuProgram::CreateProgram( shaders );
            
            if( program ) {
                pack.programs.emplace( progName, program );
                
                std::clog << "[ResourceManager] Loaded program \"" << progName << "\" in resource pack \"" << name << "\"" << std::endl;
            }
        } catch( const std::exception &e ) {
            std::cerr << "[ResourceManager] Failed to load program \"" << progName << "\" in resource pack \"" << name << "\", error: " << e.what() << std::endl;
        }
    }
    
    for( Yaml::Node materialNode : materials ) {
        Yaml::MappingNode settings = materialNode.asMapping();
        
        std::string matName = settings.getValues("Name").at(0).asValue().getValue();
        std::string source = settings.getValues("Source").at(0).asValue().getValue();
        
        try {
            std::string filename = resourcePrefix + source;
            MaterialPtr material = Material::LoadFromFile( filename, this );
            
            if( material ) {
                pack.materials.emplace( matName, material );
                
                std::clog << "[ResourceManager] Loaded material \"" << matName << "\" in resource pack \"" << name << "\"" << std::endl;
            }
        } catch( const std::exception &e ) {
            std::cerr << "[ResourceManager] Failed to load material \"" << matName << "\" in resource pack \"" << name << "\", error: " << e.what() << std::endl;
        }
    }
    
    for( Yaml::Node meshNode : meshes ) {
        Yaml::MappingNode settings = meshNode.asMapping();
        
        std::string modName = settings.getValues("Name").at(0).asValue().getValue();
        std::string source = settings.getValues("Source").at(0).asValue().getValue();
        
        try {
            std::string filename = resourcePrefix + source;
            MeshPtr mesh = Mesh::LoadMeshFromFile( filename, this );
            mesh->setName( StringUtils::strjoin(name,":",modName) );
        
            if( mesh ) {
                pack.meshes.emplace( modName, mesh );
                
                std::clog << "[ResourceManager] Loaded mesh \"" << modName << "\" in resource pack \"" << name << "\"" << std::endl;
            }
        } catch( const std::exception &e ) {
            std::cerr << "[ResourceManager] Failed to load mesh \"" << modName << "\" in resource pack \"" << name << "\", error: " << e.what() << std::endl;
        }
    }
}