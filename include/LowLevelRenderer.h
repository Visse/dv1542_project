#pragma once

#include <vector>
#include <array>

#include "RenderQueue.h"
#include "UniquePtr.h"
#include "SharedPtr.h"
#include "GLTypes.h"

class FrameBuffer;
class Renderable;
class VertexArrayObject;
class Material;
class GpuBuffer;
class Texture;
class Root;
class Camera;

struct LowLevelRenderOperation 
{    
    Material *material = nullptr;
    VertexArrayObject *vao = nullptr;
    GpuBuffer *indexBuffer = nullptr;
    
    Renderable *renderable = nullptr;
    
    // if null, its filled with from the current camera
    GpuBuffer *sceneUniforms = nullptr;
};

class LowLevelRenderer {
public:
    LowLevelRenderer( Root *root );
    ~LowLevelRenderer();
    
    void queueOperation( const LowLevelRenderOperation &operation, unsigned int queue = RQ_DeferedDefault );
    void flush();
    
    void clearFrame();
    void displayFrame();
    
    void _setCurrentCamera( Camera *camera ) {
        mCurrentCamera = camera;
    }
private:
    void sortRenderQueues();
    void renderQueue( unsigned int queue );
    
private:
    typedef std::vector<LowLevelRenderOperation> OperationQueue;
    typedef std::array<OperationQueue,RQ_Count> RenderQueue;
    
private:
    Root *mRoot;
    RenderQueue mQueue;
    Camera *mCurrentCamera = nullptr;
    GLuint mDefaultSceneInfoBinding;
    
    UniquePtr<FrameBuffer> mDefferedFrameBuffer;
    UniquePtr<VertexArrayObject> mVAO;
    
    SharedPtr<Texture> mDefferedDiffuseTexture,
                       mDefferedDepthTexture,
                       mDefferedNormalTexture,
                       mDefferedSpecularTexture;
    
    SharedPtr<Material> mDefferedMaterial;
    struct {
        GLint nearPlane, farPlane;
    } mDefferedShaderLoc;
};