#pragma once

#include "BaseManager.h"
#include "FixedSizeTypes.h"
#include "SharedPtr.h"
#include "Frustrum.h"

#include <map>
#include <glm/mat4x4.hpp>

class Texture;
class GpuProgram;
class Root;
class GpuBuffer;
class GraphicsManager;
class LowLevelRenderer;
class Material;
class VertexArrayObject;

class SceneObject;
class SceneNode;
class Mesh;
class DebugLogListener;


class DebugManager :
    public BaseManager
{
public:
    virtual bool init( Root *root ) override;
    virtual void postInit() override;
    virtual void destroy() override;
    
    virtual void update( float dt );
    virtual bool handleSDLEvent( const SDL_Event &event );
    
private:
    void initImGui();
    void destroyImGui();
    void setInputScreenPos(int x, int y);
    
    void showSceneObject( float dt, SceneObject *object );
    void showSceneNode( SceneNode *node, const glm::vec4 &color );
    void showObjectBounds( SceneObject *object, const glm::vec4 &color );
    
    glm::vec4 getColorFromFrustrumTest( const BoundingSphere &bounds );
    
    void submitDebugDraw();
    
    void paintDebugOverlay();
    void render();
    
private:
    struct DebugDrawInfo {
        bool wireFrame = false,
             normals = false,
             bounds = false,
             parentSceneNodes = false;
             
        bool debugLight = false;
        
        SharedPtr<Mesh> mesh;
    };
    
    struct DebugGuiUniforms {
        glm::mat4 projectionMatrix;
    };
    
    enum class GBufferDebug {
        None,
        Normal,
        Depth
    };
    
    class DebugFrameListener;
    
private:
    Root *mRoot = nullptr;
    DebugFrameListener *mFrameListener;
    DebugLogListener* mLogListener;
    
    Int32 mKeyToogleDebug; 
    
    bool mIsDebugVisible = false,
         mShowSceneBounds = false,
         mShowSceneGraph = false,
         mShowBoundsInFrustrumColor = false,
         mShowSceneNodesAsBoxes = true,
         mAdjustSceneNodesSize = false,
         mUseSavedFrustrum = false,
         mShowSavedFrustrum = false;
    
    std::map<SceneObject*, DebugDrawInfo> mDebugDrawInfo;
    SharedPtr<GpuBuffer> mVertexBuffer;
    
    float mHeight;
    
    DebugGuiUniforms mUniforms;
    
    Frustrum mCurrentFrustrum,
             mSavedFrustrum;
    glm::mat4 mSavedViewProjMatrix;
    
    GBufferDebug mGBufferDebug = GBufferDebug::None;
    SharedPtr<Material> mGBufferNormalMaterial, mGBufferDepthMaterial;
};