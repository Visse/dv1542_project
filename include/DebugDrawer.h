#pragma once

#include "BaseManager.h"
#include "SharedPtr.h"
#include "GLTypes.h"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <vector>

class Material;
class Camera;
class Mesh;

class DebugDrawer :
    public BaseManager
{
public:
    virtual bool init( Root *root );
    virtual void destroy();
    
    virtual void update( float dt );
    
    void drawWireFrame( const SharedPtr<Mesh> &mesh, const glm::mat4 &transform, const glm::vec3 &color = glm::vec3(0.1f,1.f,0.5f));
    
private:
    struct DebugFrameListener;
    
    struct WireFrameDraw {
        SharedPtr<Mesh> mesh;
        glm::mat4 transform;
        glm::vec3 color;
    };
    
private:
    
    void render( Camera *camera );
    
    
    
private:
    Root *mRoot;
    DebugFrameListener *mFrameListener = nullptr;
    
    std::vector<WireFrameDraw> mWireFramesDraws;
    
    SharedPtr<Material> mWireFrameMaterial;
    struct {
        GLint colorLoc;
    } mWireFrameLoc;
};