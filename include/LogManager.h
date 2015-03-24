#pragma once

class Root;
class Log;

class LogManager
{
public:
    LogManager( Root *root );
    virtual ~LogManager();
    
    Log* getDefaultLog() {
        return mDefaultLog;
    }
    
private:
    Log *mDefaultLog = nullptr;
};