#pragma once

#include "BaseManager.h"
#include "ValueHistory.h"
#include "FixedSizeTypes.h"
#include "GLTypes.h"

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
    
    const ValueHistory<float> getGpuTimeHistory() {
        return mGpuTimes;
    }
    
private:
    void fireFrameBegun();
    void fireFrameEnded();
    
    void firePreCamera( Camera *camera );
    void firePostCamera( Camera *camera );
    
private:
    Root *mRoot = nullptr;
    SDL_Window *mWindow = nullptr;
    SDL_GLContext mGLContext = nullptr;
    
    LowLevelRenderer *mRenderer = nullptr;
    
    std::vector<FrameListener*> mFrameListeners;
    std::vector<Camera*> mCameras;
    
    std::vector<GLuint> mQuaryObjects;
    int mCurrentQuary = 0;
    
    ValueHistory<float> mGpuTimes;
};