#include "InputManager.h"
#include "InputListener.h"
#include "Root.h"
#include "Config.h"

#include <cassert>
#include <algorithm>

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mouse.h>

bool InputManager::init( Root *root )
{
    mRoot = root;
    const Config *config = mRoot->getConfig();
    mKeyToogleMouse = config->keyBindings.toogleMouse;
    
    SDL_SetRelativeMouseMode( SDL_TRUE );
    
    return true;
}

void InputManager::destroy()
{
}

void InputManager::update( float dt )
{
    cleanUpDeadListeners();
}

void InputManager::addListener( InputListener *listener, int priority )
{
    ListenerInfo info;
        info.listener = listener;
        info.priority = priority;
        info.alive = true;
        
    auto loc = std::upper_bound( mListeners.begin(), mListeners.end(), info, 
        []( const ListenerInfo &i1, const ListenerInfo &i2 ) {
            return i1.priority < i2.priority;
        }
    );
    mListeners.emplace( loc, info );
}

void InputManager::removeListener( InputListener *listener )
{
    assert( !mListeners.empty() );
    auto iter = mListeners.end()-1;
    for( ; iter != mListeners.begin(); --iter ) {
        if( iter->listener == listener ) break;
    }
    assert( iter->listener == listener );
    
    // we don't know if we are called from inside a listener,
    //  (if we are called under one of the fire* functions),
    // so instead of having to deal with a container that can
    // change when we loop over it, we just mark the entry as dead
    // and skip it, later we 'clean' it up inside 'cleanUpDeadListeners'
    iter->alive = false;
}

bool InputManager::fireKeyDown( const SDL_KeyboardEvent &event )
{
    for( const ListenerInfo &info : mListeners ) {
        if( info.alive ) {
            info.listener->onKeyDown( event );
        }
    }
    // @todo change the listener interface to support 'event handled'
    return true;
}

bool InputManager::fireKeyUp( const SDL_KeyboardEvent &event )
{
    for( const ListenerInfo &info : mListeners ) {
        if( info.alive ) {
            info.listener->onKeyUp( event );
        }
    }
    return true;
}

bool InputManager::fireMouseMove( const SDL_MouseMotionEvent &event )
{
    for( const ListenerInfo &info : mListeners ) {
        if( info.alive ) {
            info.listener->onMouseMove( event );
        }
    }
    return true;
}

bool InputManager::fireMouseScroll( const SDL_MouseWheelEvent &event )
{
    for( const ListenerInfo &info : mListeners ) {
        if( info.alive ) {
            info.listener->onMouseScroll( event );
        }
    }
    return true;
}


bool InputManager::handleSDLEvent( const SDL_Event &event )
{
    if( event.type == SDL_KEYDOWN &&
        event.key.keysym.sym == mKeyToogleMouse )
    {
        mIsActive = !mIsActive;
        SDL_SetRelativeMouseMode( mIsActive ? SDL_TRUE : SDL_FALSE );
        return false;
    }
    
    switch( event.type ) {
    case( SDL_KEYDOWN ):
        return fireKeyDown( event.key );
    case( SDL_KEYUP ):
        return fireKeyUp( event.key );
    case( SDL_MOUSEMOTION ):
        return fireMouseMove( event.motion );
    case( SDL_MOUSEWHEEL ):
        return fireMouseScroll( event.wheel );
    }
    
    return false;
}

void InputManager::cleanUpDeadListeners()
{
    auto newEnd = std::remove_if( mListeners.begin(), mListeners.end(), 
        []( const ListenerInfo &i ) {
            return !i.alive;
        }
    );
    mListeners.erase( newEnd, mListeners.end() );
}


