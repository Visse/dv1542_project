#include "RenderQueue.h"
#include "StringUtils.h"

#include <cstdio>
#include <stdexcept>

unsigned int renderQueueFromString(const std::string &str)
{
    unsigned int queue = (unsigned int)-1;
    
    std::string string = StringUtils::toLowerCase( str );
    
    if( str == "deferredfirst" ) {
        queue = RQ_DeferredFirst;
    }
    else if( str == "deferreddefault" ) {
        queue = RQ_DeferredDefault;
    }
    else if( str == "deferredlast" ) {
        queue = RQ_DeferredLast;
    }
    else if( str == "light" ) {
        queue = RQ_Light;
    }
    else if( str == "overlay" ) {
        queue = RQ_Overlay;
    }
    else if( std::sscanf(string.c_str(), "defered%u", &queue) == 1 ) {
        queue += RQ_DeferredFirst;
        if( queue > RQ_DeferredLast ) queue = RQ_DeferredLast;
    }
    
    if( queue < 0 || queue >= RQ_Count ) {
        throw std::runtime_error( StringUtils::strjoin("String \"",str,"\" isn't a valid RenderQueue!") ); 
    }
    
    return queue;
}
