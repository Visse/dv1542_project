#pragma once


class Root;
typedef union SDL_Event SDL_Event;

class BaseManager {
public:
    virtual ~BaseManager() = default;
    
    virtual bool init( Root *root ) = 0;
    virtual void destroy() = 0;
    virtual void postInit() {}
    
    virtual void update( float dt ) {}
    // return true if the event was handled
    virtual bool handleSDLEvent( const SDL_Event &event ) { return false; }
};