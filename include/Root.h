#pragma once

#include "ValueHistory.h"
#include "FixedSizeTypes.h"

class Log;
class LogManager;
struct Config;
class BaseManager;
class GraphicsManager;
class DebugManager;
class ResourceManager;
class InputManager;
class SceneManager;
class DebugDrawer;

struct StartupMesurements;

class Root {
public:
    bool init();
    void destroy();
    
    void run();
    void quit();
    
    
    Log* getDefaultLog() {
        return mDefaultLog;
    }
    LogManager* getLogManager() {
        return mLogManager;
    }
    GraphicsManager* getGraphicsManager() {
        return mGraphicsManager;
    }
    DebugManager* getDebugManager() {
        return mDebugManager;
    }
    ResourceManager* getResourceManager() {
        return mResourceManager;
    }
    InputManager* getInputManager() {
        return mInputManager;
    }
    SceneManager* getSceneManager() {
        return mSceneManager;
    }
    DebugDrawer* getDebugDrawer() {
        return mDebugDrawer;
    }
    
    const Config* getConfig() {
        return mConfig;
    }
    
    const ValueHistory<float>& getFrameTimeHistory() {
        return mFrameTimeHistory;
    }
    const ValueHistory<float>& getFrameRateHistory() {
        return mFrameRateHistory;
    }
    
    StartupMesurements* getStartupMesurements() {
        return mStartupMesurements;
    }
    
private:
    bool initManagers();
    void destroyManagers();
    void updateManagers( float dt );
    
    void handleSDLEvents();
    
private:
    bool mRunning = false;
    Config *mConfig = nullptr;
    Log *mDefaultLog = nullptr;
    
    Int32 mQuitGameKey;
    
    LogManager *mLogManager = nullptr;
    GraphicsManager *mGraphicsManager = nullptr;
    DebugManager *mDebugManager = nullptr;
    ResourceManager *mResourceManager = nullptr;
    InputManager *mInputManager = nullptr;
    SceneManager *mSceneManager = nullptr;
    DebugDrawer *mDebugDrawer = nullptr;
    
    
    std::vector<BaseManager*> mManagers;
    
    StartupMesurements *mStartupMesurements = nullptr;
    
    ValueHistory<float> mFrameTimeHistory,
                        mFrameRateHistory;
};