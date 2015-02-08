#pragma once

#include "BaseManager.h"

#include <vector>

class Root;
class FrameListener;
class LowLevelRenderer;
class Camera;

typedef void* SDL_GLContext;
typedef struct SDL_Window SDL_Window;

class GraphicsManager :
    public BaseManager
{
public:
    virtual bool init( Root *root ) override;
    virtual void destroy() override;
    virtual void postInit() override;
    
    virtual void update( float dt ) override;
    virtual bool handleSDLEvent( const SDL_Event &event ) override;
    void render();
    
    void addFrameListener( FrameListener *listener );
    void removeFrameListener( FrameListener *listener );
    
    void addCamera( Camera *camera );
    void removeCamera( Camera *camera );
    
private:
    void fireFrameBegun();
    void fireFrameEnded();
    
    void firePreCamera( Camera *camera );
    void firePostCamera( Camera *camera );
    
private:
    Root *mRoot = nullptr;
    SDL_Window *mWindow = nullptr;
    SDL_GLContext mGLContext = nullptr;
    
    LowLevelRenderer *mRenderer;
    
    std::vector<FrameListener*> mFrameListeners;
    std::vector<Camera*> mCameras;
};