#include "Config.h"

#include "yaml-cxx/Node.h"
#include "yaml-cxx/MappingNode.h"
#include "yaml-cxx/ValueNode.h"

#include <SDL2/SDL_keyboard.h>

void loadKeyBindigs( KeyBindings &keybindings, Yaml::MappingNode node );
void loadFlyingControllerConfig( FlyingControllConfig &config, Yaml::MappingNode node );
void loadComputeParticleConfig( ComputeParticleConfig &config, Yaml::MappingNode node );

void Config::load( const std::string &filename )
{
    Yaml::MappingNode node = Yaml::Node::LoadFromFile(filename).asMapping();
    
    for( size_t i=0, count=node.getCount(); i < count; ++i ) {
        auto entry = node.getValue(i);
        auto key = entry.first.asValue().getValue();
        auto value = entry.second;
        
        if( StringUtils::equalCaseInsensitive(key,"TargetFrameRate") ) {
            targetFrameRate = value.asValue().getValue<float>();
        }
        else if( StringUtils::equalCaseInsensitive(key,"WindowWidth") ) {
            windowWidth = value.asValue().getValue<unsigned int>();
        }
        else if( StringUtils::equalCaseInsensitive(key,"WindowHeight") ) {
            windowHeight = value.asValue().getValue<unsigned int>();
        }
        else if( StringUtils::equalCaseInsensitive(key,"DefferedBufferHeight") ) {
            defferedBufferHeight = value.asValue().getValue<unsigned int>();
        }
        else if( StringUtils::equalCaseInsensitive(key,"DefferedBufferWidth") ) {
            defferedBufferWidth = value.asValue().getValue<unsigned int>();
        }
        else if( StringUtils::equalCaseInsensitive(key,"Fov") ) {
            fov = value.asValue().getValue<float>();
        }
        else if( StringUtils::equalCaseInsensitive(key,"NearPlane") ) {
            nearPlane = value.asValue().getValue<float>();
        }
        else if( StringUtils::equalCaseInsensitive(key,"FarPlane") ) {
            farPlane = value.asValue().getValue<float>();
        }
        else if( StringUtils::equalCaseInsensitive(key,"WindowTitle") ) {
            windowTitle = value.asValue().getValue();
        }
        else if( StringUtils::equalCaseInsensitive(key,"OpenGLDebug") ) {
            openglDebug = value.asValue().getValue<bool>();
        }
        else if( StringUtils::equalCaseInsensitive(key,"ValueHistoryLenght") ) {
            valueHistoryLenght = value.asValue().getValue<int>();
        }
        else if( StringUtils::equalCaseInsensitive(key,"ResourcePath") ) {
            resourcePaths.push_back( value.asValue().getValue() );
        }
        else if( StringUtils::equalCaseInsensitive(key,"StartScene") ) {
            startScene = value.asValue().getValue();
        }
        else if( StringUtils::equalCaseInsensitive(key,"KeyBindings") ) {
            loadKeyBindigs( keyBindings, value.asMapping() );
        }
        else if( StringUtils::equalCaseInsensitive(key, "FreeCamera") ) {
            loadFlyingControllerConfig( freeCamera, value.asMapping() );
        }
        else if( StringUtils::equalCaseInsensitive(key, "ComputeParticle") ) {
            loadComputeParticleConfig( computeParticle, value.asMapping() );
        }
    }
}

void loadKeyBindigs( KeyBindings &keybindings, Yaml::MappingNode node )
{
    for( size_t i=0, count=node.getCount(); i < count; ++i ) {
        auto entry = node.getValue(i);
        auto key = entry.first.asValue().getValue();
        auto value = entry.second.asValue().getValue();
        
        if( StringUtils::equalCaseInsensitive(key, "QuitGame") ) {
            keybindings.quitGame = SDL_GetKeyFromName( value.c_str() );
        }
        else if( StringUtils::equalCaseInsensitive(key, "ToogleMouse") ) {
            keybindings.toogleMouse = SDL_GetKeyFromName( value.c_str() );
        }
        else if( StringUtils::equalCaseInsensitive(key, "ToogleDebug") ) {
            keybindings.toogleDebug = SDL_GetKeyFromName( value.c_str() );
        }
    }
}

void loadFlyingControllerConfig( FlyingControllConfig &config, Yaml::MappingNode node )
{
    for( size_t i=0, count=node.getCount(); i < count; ++i ) {
        auto entry = node.getValue(i);
        auto key = entry.first.asValue().getValue();
        auto value = entry.second.asValue().getValue();
        
        if( StringUtils::equalCaseInsensitive(key, "Forward") ) {
            config.forward = SDL_GetKeyFromName( value.c_str() );
        }
        else if( StringUtils::equalCaseInsensitive(key, "Backward") ) {
            config.backward = SDL_GetKeyFromName( value.c_str() );
        }
        else if( StringUtils::equalCaseInsensitive(key, "Left") ) {
            config.left = SDL_GetKeyFromName( value.c_str() );
        }
        else if( StringUtils::equalCaseInsensitive(key, "Right") ) {
            config.right = SDL_GetKeyFromName( value.c_str() );
        }
        else if( StringUtils::equalCaseInsensitive(key, "Up") ) {
            config.up = SDL_GetKeyFromName( value.c_str() );
        }
        else if( StringUtils::equalCaseInsensitive(key, "Down") ) {
            config.down = SDL_GetKeyFromName( value.c_str() );
        }
        else if( StringUtils::equalCaseInsensitive(key, "TurnUp") ) {
            config.turnUp = SDL_GetKeyFromName( value.c_str() );
        }
        else if( StringUtils::equalCaseInsensitive(key, "TurnDown") ) {
            config.turnDown = SDL_GetKeyFromName( value.c_str() );
        }
        else if( StringUtils::equalCaseInsensitive(key, "TurnLeft") ) {
            config.turnLeft = SDL_GetKeyFromName( value.c_str() );
        }
        else if( StringUtils::equalCaseInsensitive(key, "TurnRight") ) {
            config.turnRight = SDL_GetKeyFromName( value.c_str() );
        }
        else if( StringUtils::equalCaseInsensitive(key, "RollLeft") ) {
            config.rollLeft = SDL_GetKeyFromName( value.c_str() );
        }
        else if( StringUtils::equalCaseInsensitive(key, "RollRight") ) {
            config.rollRight = SDL_GetKeyFromName( value.c_str() );
        }
        else if( StringUtils::equalCaseInsensitive(key, "Fast") ) {
            config.fast = SDL_GetKeyFromName( value.c_str() );
        }
        else if( StringUtils::equalCaseInsensitive(key, "Slow") ) {
            config.slow = SDL_GetKeyFromName( value.c_str() );
        }
        else if( StringUtils::equalCaseInsensitive(key, "ToogleMouseControll") ) {
            config.toogleMouseControll = SDL_GetKeyFromName( value.c_str() );
        }
    }
}

void loadComputeParticleConfig( ComputeParticleConfig &config, Yaml::MappingNode node )
{
    for( size_t i=0, count=node.getCount(); i < count; ++i ) {
        auto entry = node.getValue(i);
        auto key = entry.first.asValue().getValue();
        auto value = entry.second.asValue();
        
        if( StringUtils::equalCaseInsensitive(key, "LocalSize") ) {
            config.localSize = value.getValue<size_t>();
        }
        else if( StringUtils::equalCaseInsensitive(key, "MaxGroupCount") ) {
            config.maxGroupCount = value.getValue<size_t>();
        }
        else if( StringUtils::equalCaseInsensitive(key, "MaxAttractorCount") ) {
            config.maxAttractorCount = value.getValue<size_t>();
        }
        else if( StringUtils::equalCaseInsensitive(key, "DefaultGroupCount") ) {
            config.defaultGroupCount = value.getValue<size_t>();
        }
        else if( StringUtils::equalCaseInsensitive(key, "DefaultAttractorCount") ) {
            config.defaultAttractorCount = value.getValue<size_t>();
        }
        
    }
}



