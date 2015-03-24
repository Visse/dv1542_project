#include "LogManager.h"
#include "Config.h"
#include "Root.h"
#include "Log.h"


LogManager::LogManager( Root *root )
{
    const Config *config = root->getConfig();
    
    mDefaultLog = new Log;
    
    mDefaultLog->setLogToFile( config->logToFile, config->logFileName );
    mDefaultLog->setLogToStdLog( config->logToStdLog );
}

LogManager::~LogManager()
{
    delete mDefaultLog;
    mDefaultLog = nullptr;
}



