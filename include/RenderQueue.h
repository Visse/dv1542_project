#pragma once

#include <string>

enum RenderQueue {
    RQ_DeferedFirst = 0,
    RQ_DeferedDefault = 5,
    RQ_DeferedLast  = 10,
    
    RQ_Light = 15,
    
    RQ_Overlay = 20,
    
    RQ_Count
};

uint renderQueueFromString( const std::string &str );