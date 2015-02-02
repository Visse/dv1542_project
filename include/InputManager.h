#pragma once

#include <vector>

#include "BaseManager.h"
#include "FixedSizeTypes.h"

typedef struct SDL_KeyboardEvent SDL_KeyboardEvent;
typedef struct SDL_MouseMotionEvent SDL_MouseMotionEvent;
typedef struct SDL_MouseWheelEvent SDL_MouseWheelEvent;

class InputListener;

class InputManager :
    public BaseManager
{
public:
    virtual bool init( Root *root );
    virtual void destroy();
    
    virtual void update(float dt);
    
    virtual bool handleSDLEvent( const SDL_Event& event );
    
    void addListener( InputListener *listener, int priority=0 );
    void removeListener( InputListener *listener );
    
private:
    bool fireKeyDown( const SDL_KeyboardEvent &event );
    bool fireKeyUp( const SDL_KeyboardEvent &event );
    
    bool fireMouseMove( const SDL_MouseMotionEvent &event );
    bool fireMouseScroll( const SDL_MouseWheelEvent &event );
    
    void cleanUpDeadListeners();
//     void fireMouseMoved( const );
private:
    struct ListenerInfo {
        InputListener *listener;
        int priority;
        bool alive;
    };
    
private:
    Root *mRoot;
    Int32 mKeyToogleMouse;
    
    std::vector<ListenerInfo> mListeners;
    bool mIsActive = true;
};