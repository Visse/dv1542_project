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
    
    void drawWireFrame( const SharedPtr<Mesh> &mesh, const glm::mat4 &transform, const glm::vec4 &color = glm::vec4(0.1f,1.f,0.5f,1.f));
    void drawVertexNormals( const SharedPtr<Mesh> &mesh, const glm::mat4 &transform, float length = 0.1f, 
                            const glm::vec4 &normalColor = glm::vec4(0.5f,0.2f,0.2f,1.f), const glm::vec4 &tangentColor = glm::vec4(0.2f,0.5f,0.2f,1.f),
                            const glm::vec4 &bitangentColor = glm::vec4(0.2f,0.2f,0.5f,1.f)
                          );
    void queueRenderable( LowLevelRenderer &renderer );
    
    void drawWireSphere( float radius, const glm::mat4 &transform, const glm::vec4 &color = glm::vec4(0.1f,1.f,0.5f,1.f) );
    void drawWireCone( float height, float radius, const glm::mat4 &transform, const glm::vec4 &color = glm::vec4(0.1f,1.f,0.5f,1.f) );
    void drawWireConeAngle( float height, float angle, const glm::mat4 &transform, const glm::vec4 &color = glm::vec4(0.1f,1.f,0.5f,1.f) );
    void drawWireBox( const glm::vec3 &size, const glm::mat4 &transform, const glm::vec4 &color = glm::vec4(0.1f,1.f,0.5f,1.f) );
    
private:
    struct DebugDraw {
        SharedPtr<Mesh> mesh;
        glm::mat4 transform;
        glm::vec4 color;
    };
    struct DebugNormalDraw {
        SharedPtr<Mesh> mesh;
        glm::mat4 transform;
        glm::vec4 normalColor,
                  tangentColor,
                  bitangentColor;
        float length;
    };
    struct WireDrawUniformBlock {
        glm::mat4 modelMatrix;
        glm::vec4 color;
    };
    struct NormalDrawUniformBlock {
        glm::mat4 modelMatrix;
        glm::vec4 normalColor,
                  tangentColor,
                  bitangentColor;
        float lenght;
    };
    
private:
    void queueWireframe( LowLevelRenderer &renderer );
    void queueNormals( LowLevelRenderer &renderer );
    
private:
    Root *mRoot = nullptr;
    
    std::vector<DebugDraw> mWireFramesDraws;
    std::vector<DebugNormalDraw> mNormalDraws;
    
    SharedPtr<Material> mWireFrameMaterial;
    SharedPtr<Material> mNormalMaterial;
    
    size_t mWireUniformBlockLoc,
           mNormalUniformBlockLoc;
           
    SharedPtr<Mesh> mSphereMesh,
                    mConeMesh,
                    mBoxMesh;
};