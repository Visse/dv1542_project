#pragma once

#include <string>
#include <vector>

#include "FixedSizeTypes.h"

struct KeyBindings
{
    Int32 quitGame    = 0x40000045, // F12
          toogleMouse = 0x4000003A, // F1
          toogleDebug = 0x4000003B; // F2
    
};

struct FlyingControllConfig {
    Int32 forward  = 0x77, // w
          backward = 0x73, // s
          left  = 0x61, // a
          right = 0x64, // d
          up    = 0x20, // spacebar
          down  = 0x400000E1, // left shift
          
          turnUp    = 0x40000052, // Up
          turnDown  = 0x40000051, // Down
          turnLeft  = 0x40000050, // Left
          turnRight = 0x4000004F, // Right
          rollLeft  = 0x71, // q
          rollRight = 0x65, // e
          
          fast = 0x400000E0, // left ctrl
          slow = 0x400000E2, // left alt
          
          toogleMouseControll = 0x4000003C; // F3
};

struct ComputeParticleConfig {
    size_t localSize = 64,
           maxGroupCount = 8192,
           maxAttractorCount = 16,
           
           defaultGroupCount = 512,
           defaultAttractorCount = 4;
};

struct Config 
{
    void load( const std::string &filename );
    
    // general config
    float targetFrameRate = 30.f;
    int valueHistoryLenght = 200;
    
    // logging
    bool logToFile = false,
         logToStdLog = true;
    std::string logFileName;
    
    // graphics config
    unsigned int windowHeight = 720,
                 windowWidth = 1280;
         
    unsigned int defferedBufferWidth = 720,
                 defferedBufferHeight = 1280;
                 
    unsigned int ssaoBufferWidth  = 360,
                 ssaoBufferHeight = 640;
    
    unsigned int shadowMapSize = 1024;
                 
    float fov = 90.f,
          nearPlane = 0.1f,
          farPlane  = 100.f;
            
    std::string windowTitle = "DV1542 Project";
    
    bool openglDebug = false;
    
    // scene config
    std::string startScene;
    
    // resource config
    std::vector<std::string> resourcePaths;
    
    
    KeyBindings keyBindings;    
    FlyingControllConfig freeCamera;
    ComputeParticleConfig computeParticle;
};