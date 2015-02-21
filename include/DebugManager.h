#pragma once

#include "BaseManager.h"
#include "FixedSizeTypes.h"
#include "SharedPtr.h"

#include <map>
#include <glm/mat4x4.hpp>

class Root;
class GpuBuffer;
class GraphicsManager;
class LowLevelRenderer;
class Material;
class VertexArrayObject;

class SceneObject;
class Mesh;


class DebugManager :
    public BaseManager
{
public:
    virtual bool init( Root *root ) override;
    virtual void destroy() override;
    
    virtual void update( float dt );
    virtual bool handleSDLEvent( const SDL_Event &event );
    
    
private:
    void initImGui();
    void destroyImGui();
    void setInputScreenPos(int x, int y);
    
    void showSceneObject( float dt, SceneObject *object );
    void submitDebugDraw();
    
    void paintDebugOverlay();
    void render( LowLevelRenderer &renderer );
    
private:
    struct DebugDrawInfo {
        bool wireFrame = false,
             normals = false,
             bounds = false;
             
        bool debugLight = false;
        
        SharedPtr<Mesh> mesh;
    };
    
    struct UniformBlock {
        glm::mat4 projectionMatrix;
    };
    
    class DebugCamera;
private:
    Root *mRoot = nullptr;
    DebugCamera *mCamera;
    
    Int32 mKeyToogleDebug; 
    
    bool mIsDebugVisible = false,
         mShowSceneBounds = false;
    
    std::map<SceneObject*, DebugDrawInfo> mDebugDrawInfo;
    SharedPtr<GpuBuffer> mVertexBuffer;
    SharedPtr<Material> mMaterial;
    SharedPtr<VertexArrayObject> mVAO;
    
    float mHeight;
    
    UniformBlock mUniforms;
    size_t mUniformBlockLoc;
};