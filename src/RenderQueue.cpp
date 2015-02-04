#include "RenderQueue.h"
#include "StringUtils.h"

#include <cstdio>
#include <stdexcept>

unsigned int renderQueueFromString(const std::string &str)
{
    unsigned int queue = unsigned int(-1);
    
    std::string string = StringUtils::toLowerCase( str );
    
    if( str == "deferedfirst" ) {
        queue = RQ_DeferedFirst;
    }
    else if( str == "defereddefault" ) {
        queue = RQ_DeferedDefault;
    }
    else if( str == "deferedlast" ) {
        queue = RQ_DeferedLast;
    }
    else if( str == "light" ) {
        queue = RQ_Light;
    }
    else if( str == "overlay" ) {
        queue = RQ_Overlay;
    }
    else if( std::sscanf(string.c_str(), "defered%i", &queue) == 1 ) {
        queue += RQ_DeferedFirst;
        if( queue > RQ_DeferedLast ) queue = RQ_DeferedLast;
    }
    
    if( queue < 0 || queue >= RQ_Count ) {
        throw std::runtime_error( StringUtils::strjoin("String \"",str,"\" isn't a valid RenderQueue!") ); 
    }
    
    return queue;
}
