#pragma once

#include "BaseManager.h"
#include "FixedSizeTypes.h"

class Root;
class DebugFrameListener;

class SceneObject;


class DebugManager :
    public BaseManager
{
public:
    virtual bool init( Root *root ) override;
    virtual void destroy() override;
    
    virtual void update(float dt);
    virtual bool handleSDLEvent( const SDL_Event &event );
    
    void paintDebugOverlay();
    
private:
    void initImGui();
    void destroyImGui();
    void setInputScreenPos(int x, int y);
    
    void showSceneObject( float dt, SceneObject *object );
    
private:
    Root *mRoot = nullptr;
    Int32 mKeyToogleDebug;
    
    DebugFrameListener *mFrameListener = nullptr;
    
    bool mIsDebugVisible = false;
};