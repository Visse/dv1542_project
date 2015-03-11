#pragma once

#include "BaseManager.h"
#include "SharedPtr.h"
#include "GLTypes.h"
#include "UniformBuffer.h"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <vector>

class VertexArrayObject;
class GpuBuffer;
class Material;
class Camera;
class Mesh;
class Renderer;

class Renderable;

class DebugDrawer :
    public BaseManager
{
public:
    virtual bool init( Root *root );
    virtual void postInit();
    virtual void destroy();
    
    virtual void update( float dt );
    
    void drawWireFrame( const SharedPtr<Mesh> &mesh, const glm::mat4 &transform, const glm::vec4 &color = glm::vec4(0.1f,1.f,0.5f,1.f));
    void drawVertexNormals( const SharedPtr<Mesh> &mesh, const glm::mat4 &transform, float length = 0.1f, 
                            const glm::vec4 &normalColor = glm::vec4(0.5f,0.2f,0.2f,1.f), const glm::vec4 &tangentColor = glm::vec4(0.2f,0.5f,0.2f,1.f),
                            const glm::vec4 &bitangentColor = glm::vec4(0.2f,0.2f,0.5f,1.f)
                          );
    
    void drawWireSphere( float radius, const glm::mat4 &transform, const glm::vec4 &color = glm::vec4(0.1f,1.f,0.5f,1.f) );
    void drawWireCone( float height, float radius, const glm::mat4 &transform, const glm::vec4 &color = glm::vec4(0.1f,1.f,0.5f,1.f) );
    void drawWireConeAngle( float height, float angle, const glm::mat4 &transform, const glm::vec4 &color = glm::vec4(0.1f,1.f,0.5f,1.f) );
    void drawWireBox( const glm::vec3 &hsize, const glm::mat4 &transform, const glm::vec4 &color = glm::vec4(0.1f,1.f,0.5f,1.f) );
    
private:    
    struct WireDrawUniformBlock {
        glm::mat4 modelMatrix;
        glm::vec4 color;
    };
    struct NormalDrawUniformBlock {
        glm::mat4 modelMatrix;
        glm::vec4 normalColor,
                  tangentColor,
                  bitangentColor;
        float length;
    };
    struct DebugWireDraw {
        SharedPtr<Mesh> mesh;
        UniformBuffer uniforms;
    };
    struct DebugNormalDraw {
        SharedPtr<Mesh> mesh;
        UniformBuffer uniforms;
    };

    
    class DebugFrameListener;
    
private:
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec4 color;
    };
    
    static std::vector<Vertex> SPHERE_MESH,
                               CONE_MESH,
                               BOX_MESH;
private:
    void renderWireFrames();
    void renderNormals();
    
    void renderMesh( const SharedPtr<Mesh> &mesh, GLenum mode );
  
    
private:
    Root *mRoot = nullptr;
    Renderer *mRenderer = nullptr;
    
    DebugFrameListener *mFrameListener;
    
    std::vector<DebugWireDraw> mWireFramesDraws;
    std::vector<DebugNormalDraw> mNormalDraws;
    
    SharedPtr<Mesh> mSphereMesh,
                    mConeMesh,
                    mBoxMesh;
};