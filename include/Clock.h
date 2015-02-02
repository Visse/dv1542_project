#pragma once

#include "FixedSizeTypes.h"

class Clock {
public:
    typedef UInt64 TimeType;
    
public:
    Clock();
    void reset();
    
    TimeType getTime();
    
    static void Sleep( TimeType amount );
private:
    TimeType mTime;
};