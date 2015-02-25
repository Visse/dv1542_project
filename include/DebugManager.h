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
class SceneNode;
class Mesh;
class Frustrum;


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
    void showSceneNode( SceneNode *node, const glm::vec4 &color );
    void showObjectBounds( SceneObject *object, const glm::vec4 &color );
    void showFrustrumTests( const Frustrum &frustrum, SceneNode *node );
    void showFrustrumTestNode( const Frustrum &frustrum, SceneNode *node );
    void showFrustrumTestOutside( SceneNode *node );
    
    void submitDebugDraw();
    
    void paintDebugOverlay();
    void render( LowLevelRenderer &renderer );
    
private:
    struct DebugDrawInfo {
        bool wireFrame = false,
             normals = false,
             bounds = false,
             parentSceneNodes = false;
             
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
         mShowSceneBounds = false,
         mShowSceneGraph = false,
         mShowSceneNodesAsBoxes = true,
         mShowFrustrumTests = false,
         mShowFrustrumTestsOnlyObjects = true,
         mShowFrustrumTestsParents = false,
         mUseSavedFrustrum = false,
         mShowSavedFrustrum = false;
    
    std::map<SceneObject*, DebugDrawInfo> mDebugDrawInfo;
    SharedPtr<GpuBuffer> mVertexBuffer;
    SharedPtr<Material> mMaterial;
    SharedPtr<VertexArrayObject> mVAO;
    
    float mHeight;
    
    UniformBlock mUniforms;
    size_t mUniformBlockLoc;
    
    glm::mat4 mSavedViewProjMatrix;
};