#include "Root.h"
#include "Config.h"
#include "StartupMesurements.h"
#include "Timer.h"

#include "LogManager.h"
#include "GraphicsManager.h"
#include "DebugManager.h"
#include "ResourceManager.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "DebugDrawer.h"

#include <SDL2/SDL.h>

#include <chrono>
#include <thread>

bool Root::init()
{
    // @todo add cleanup on failure
    
    Timer startUpTimer;
    
    if( SDL_Init(SDL_INIT_VIDEO) != 0 ) {
        return false;
    }
    mStartupMesurements = new StartupMesurements;
    
    mConfig = new Config;
    mConfig->load( "project.cfg" ); 
    
    mQuitGameKey = mConfig->keyBindings.quitGame;
    
    mFrameTimeHistory.setSize( mConfig->valueHistoryLenght );
    mFrameRateHistory.setSize( mConfig->valueHistoryLenght );
    
    mStartupMesurements->sdlStartup = startUpTimer.getTimeAsSeconds();
    
    mLogManager = new LogManager(this);
    mDefaultLog = mLogManager->getDefaultLog();
    
    mGraphicsManager = new GraphicsManager;
    mResourceManager = new ResourceManager;
    mDebugDrawer = new DebugDrawer;
    mDebugManager = new DebugManager;
    mInputManager = new InputManager;
    mSceneManager = new SceneManager;
    
    mManagers.push_back( mGraphicsManager );
    mManagers.push_back( mResourceManager );
    mManagers.push_back( mDebugDrawer );
    mManagers.push_back( mDebugManager );
    mManagers.push_back( mInputManager );
    mManagers.push_back( mSceneManager );
    
    if( !initManagers() ) {
        return false;
    }
    
    mStartupMesurements->totalTime = startUpTimer.getTimeAsSeconds();
    
    return true;
}

void Root::destroy()
{
    destroyManagers();
    
    for( BaseManager *manager : mManagers ) {
        delete manager;
    }
    mManagers.clear();
    
    delete mLogManager;
    mLogManager = nullptr;
    mDefaultLog = nullptr;
    mGraphicsManager = nullptr;
    mResourceManager = nullptr;
    mDebugManager = nullptr;
    mInputManager = nullptr;
    mSceneManager = nullptr;
    mDebugDrawer = nullptr;
    
    delete mConfig;
    mConfig = nullptr;
    
    SDL_Quit();
}

void Root::run()
{
    mRunning = true;
    float targetFrameRate = mConfig->targetFrameRate;
    Timer frameTimer;
    Timer::Duration targetFrameTime = std::chrono::duration_cast<Timer::Duration>(Timer::Seconds(1.f/targetFrameRate));
    
    float dt = 1.f/targetFrameRate;
    while( mRunning ) {
        frameTimer.restart();
        
        handleSDLEvents();
        updateManagers( dt );
        
        mGraphicsManager->render();
        
        Timer::Duration frameTime = frameTimer.getTimeAsDuration();
        
        mFrameTimeHistory.pushValue( std::chrono::duration_cast<Timer::Millisecond>(frameTime).count() );
        
        if( frameTime < targetFrameTime ) {
            std::this_thread::sleep_for( targetFrameTime-frameTime );
        }
        
        dt = frameTimer.getTimeAsSeconds();
        
        if( dt > 1.f ) {
            dt = targetFrameRate;
        }
        mFrameRateHistory.pushValue( 1.f/dt );
    }
}

void Root::quit()
{
    mRunning = false;
}

bool Root::initManagers()
{
    if( mManagers.empty() ) return true;
    
    size_t i=0, count = mManagers.size();
    bool succes = true;
    for( ; i < count; ++i ) {
        BaseManager *manager = mManagers[i];
        if( !manager->init(this) ) {
            succes = false;
            break;
        }
    }
    
    if( !succes ) {
        // we failed to init :/, destroy the managers we managed to init
        // I'm relaying on the underflow behavior of unsigned ints
        for( --i; i < count; --i ) {
            BaseManager *manager = mManagers[i];
            manager->destroy();
        }
        return false;
    }
    for( size_t i=0; i < count; ++i ) {
        BaseManager *manager = mManagers[i];
        manager->postInit();
    }
    
    return true;
}

void Root::destroyManagers()
{
    if( mManagers.empty() ) return;
    
    for( size_t i=mManagers.size()-1;; --i ) {
        BaseManager *manager = mManagers[i];
        manager->destroy();
        
        // since 'i' is a unsigned, we can't decrement 0
        if( i==0 ) break;
    }
}

void Root::updateManagers( float dt )
{
    for( size_t i=0, count = mManagers.size(); i < count; ++i ) {
        BaseManager *manager = mManagers[i];
        manager->update( dt );
        
    }
}

void Root::handleSDLEvents()
{
    SDL_Event event;
    while( SDL_PollEvent(&event) != 0 ) {
        switch( event.type ) {
        case( SDL_QUIT ):
            quit();
            break; 
        case( SDL_KEYDOWN ):
            if( event.key.keysym.sym == mQuitGameKey ) {
                quit();
            }
            break;
        default:
            break;
        }
        
        for( BaseManager *manager : mManagers ) {
            if( manager->handleSDLEvent(event) ) {
                break;
            }
        }
    }
}
