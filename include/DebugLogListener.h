#pragma once

#include "Log.h"

#include <deque>

class DebugLogListener :
    public LogListener
{
public:  
    struct LogMessage {
        LogSeverity severity;
        const char *module;
        std::string msg;
    };
    
public:
    DebugLogListener( size_t historySize = 100 ) :
        mHistorySize(historySize)
    {}

    
    virtual void onMessageLogged( LogSeverity severity, const char* module, const std::string& msg )
    {
        if( mHistorySize > 0 && mMessages.size() >= mHistorySize ) {
            mMessages.pop_front();
        }
        
        mMessages.emplace_back( LogMessage{severity, module, msg} );
    }
    
    const std::deque<LogMessage>& getMessages() {
        return mMessages;
    }
    
private:
    std::deque<LogMessage> mMessages;
    size_t mHistorySize;
};