#pragma once

#include <string>
#include <iostream>

enum RenderQueue {
    RQ_DeferredFirst = 0,
    RQ_DeferredDefault = 5,
    RQ_DeferredLast  = 10,
    
    RQ_Light = 15,
    
    RQ_Overlay = 20,
    
    RQ_Count
};

unsigned int renderQueueFromString( const std::string &str );
