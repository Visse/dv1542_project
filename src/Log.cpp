#include "Log.h"

#include <iostream>
#include <chrono>
#include <cinttypes>
#include <algorithm>
#include <cassert>


#ifdef _MSC_VER
#include <cstdarg>
namespace std {
    int snprintf(char* str, size_t size, const char* format, ...) {
        va_list args;

        va_start(args, format);
        int count = std::vsnprintf( str, size, format, args );
        va_end(args);
        return count; 
    }
}

#endif


typedef std::chrono::system_clock ClockType;

uint64_t timeNow() {
    auto now = ClockType::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
}

void splitTimeStamp( uint64_t time, uint64_t &hours, uint64_t &mins, uint64_t &seconds, uint64_t &millis )
{
    seconds = time / 1000;
    millis = time % 1000;
    
    mins = seconds / 60;
    seconds = seconds % 60;
    
    hours = mins / 60;
    mins = mins % 60;
}

Log::Log()
{
    mCreationTime = timeNow();
}

void Log::setLogToFile( bool logToFile, const std::string &filename, bool append  )
{
    mFile.close();
    mLogToFile = logToFile;
    if( mLogToFile ) {
        mFile.open( filename, append ? std::ios::app : std::ios::trunc );
    }
}

void Log::setLogToStdLog( bool logToStdLog )
{
    mLogToStdLog = logToStdLog;
}

LogStream Log::stream( LogSeverity severity, const char *module )
{
    return LogStream( this, severity, module );
}

void Log::logMessage( LogSeverity severity, const char *module, const std::string &msg )
{
    uint64_t timeStamp = timeNow() - mCreationTime;
    
    uint64_t hours, mins, seconds, millis;
    splitTimeStamp( timeStamp, hours, mins, seconds, millis );
    
    char msgStamp[512];
    std::snprintf( msgStamp, 512, "[%02" PRIu64 ":%02" PRIu64 ":%02" PRIu64 ":%03" PRIu64 "][%s][%s]\t", 
                   hours, mins, seconds, millis, logSeverityToString(severity).c_str(), module );
    
    if( mLogToFile ) {
        mFile << msgStamp << msg << std::endl;
    }
    if( mLogToStdLog ) {
        std::clog << msgStamp << msg << std::endl;
    }
    
    for( LogListener *listener : mListeners ) {
        listener->onMessageLogged( severity, module, msg );
    }
}

void Log::removeListener( LogListener *listener )
{
    auto iter = std::find( mListeners.begin(), mListeners.end(), listener );
    assert( iter != mListeners.end() );
    
    mListeners.erase( iter );
}
