#pragma once

#include "BaseManager.h"
#include "SharedPtr.h"
#include "GLTypes.h"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <vector>

class LowLevelRenderer;
class Material;
class Camera;
class Mesh;

class Renderable;

class DebugDrawer :
    public BaseManager
{
public:
    virtual bool init( Root *root );
    virtual void destroy();
    
    virtual void update( float dt );
    
    void drawWireFrame( const SharedPtr<Mesh> &mesh, const glm::mat4 &transform, const glm::vec3 &color = glm::vec3(0.1f,1.f,0.5f));
    void queueRenderable( LowLevelRenderer &renderer );
    
private:
    class DebugWireRenderable;
    
    struct WireFrameDraw {
        SharedPtr<Mesh> mesh;
        glm::mat4 transform;
        glm::vec3 color;
    };
    
private:
    void renderWireframe();
    
private:
    Root *mRoot = nullptr;
    DebugWireRenderable *mWireRenderable = nullptr;
    
    std::vector<WireFrameDraw> mWireFramesDraws;
    
    SharedPtr<Material> mWireFrameMaterial;
    struct {
        GLint color, modelMatrix;
    } mWireFrameLoc;
    
};