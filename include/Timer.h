#pragma once


#include <chrono>

class Timer {
public:
    typedef std::chrono::high_resolution_clock ClockType;
    typedef ClockType::duration Duration;
    typedef ClockType::time_point TimePoint;
    typedef std::chrono::duration<float> Seconds;
    typedef std::chrono::duration<float,std::milli> Millisecond;

public:
    void restart() {
        mStartTime = ClockType::now();
    }
    
    float getTimeAsSeconds() {
        Duration duration = ClockType::now() - mStartTime;
        return std::chrono::duration_cast<Seconds>(duration).count();
    }
    Duration getTimeAsDuration() {
        return ClockType::now() - mStartTime;
    }
    
private:
    TimePoint mStartTime = ClockType::now();
    
};