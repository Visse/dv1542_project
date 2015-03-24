#pragma once

#include <sstream>
#include <fstream>
#include <vector>

#include "SharedEnums.h"
#include "StringUtils.h"

class LogListener {
public:
    virtual ~LogListener() = default;
    
    virtual void onMessageLogged( LogSeverity severity, const char *module, const std::string &msg ) = 0;
};

class LogStream;

class Log {
public:
    Log();
    
    void setLogToFile( bool logToFile, const std::string &filename, bool append = false );
    void setLogToStdLog( bool logToStdLog );
    
    LogStream stream( LogSeverity severity, const char *module );
    
    void logMessage( LogSeverity severity, const char *module, const std::string &msg );
    
    template< typename ...Args >
    void logMessageJoin( LogSeverity severity, const char *module,  Args&&... args ) {
        logMessage( severity, module, StringUtils::strjoin(std::forward<Args>(args)...) );
    }
    void addListener( LogListener *listener ) {
        mListeners.push_back( listener );
    }
    void removeListener( LogListener *listener );
    
private:
    uint64_t mCreationTime;
    bool mLogToFile = false;
    std::ofstream mFile;
    
    bool mLogToStdLog = false;
    
    std::vector<LogListener*> mListeners;
};


class LogStream {
public:
    LogStream( Log *log, LogSeverity severity, const char *module ) :
        mLog(log),
        mSeverity(severity),
        mModule(module)
    {}
    LogStream( const LogStream &copy ) :
        mLog(copy.mLog),
        mSeverity(copy.mSeverity),
        mModule(copy.mModule)
    {    
    }
    
    ~LogStream() {
        std::string msg = mStream.str();
        if( !msg.empty() ) {
            mLog->logMessage( mSeverity, mModule, msg );
        }
    }
    
    template< typename Type >
    LogStream& operator << ( const Type &type ) {
        mStream << type;
        return *this;
    }
    
private:
    Log *mLog;
    LogSeverity mSeverity;
    const char *mModule;
    std::ostringstream mStream;
};

