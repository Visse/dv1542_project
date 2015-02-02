#pragma once

typedef struct SDL_KeyboardEvent SDL_KeyboardEvent;
typedef struct SDL_MouseMotionEvent SDL_MouseMotionEvent;
typedef struct SDL_MouseWheelEvent SDL_MouseWheelEvent;

class InputListener {
public:
    virtual ~InputListener() = default;
    
    virtual void onKeyDown( const SDL_KeyboardEvent &event ) {}
    virtual void onKeyUp( const SDL_KeyboardEvent &event ) {}
    
    virtual void onMouseMove( const SDL_MouseMotionEvent &event ) {}
    virtual void onMouseScroll( const SDL_MouseWheelEvent &event ) {}
};