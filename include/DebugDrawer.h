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
    void drawVertexNormals( const SharedPtr<Mesh> &mesh, const glm::mat4 &transform, float length = 0.1f, 
                            const glm::vec3 &normalColor = glm::vec3(0.5f,0.2f,0.2f), const glm::vec3 &tangentColor = glm::vec3(0.2f,0.5f,0.2f),
                            const glm::vec3 &bitangentColor = glm::vec3(0.2f,0.2f,0.5f)
                          );
    void queueRenderable( LowLevelRenderer &renderer );
    
private:
    class DebugRenderable;
    
    struct DebugDraw {
        SharedPtr<Mesh> mesh;
        glm::mat4 transform;
        glm::vec3 color;
        float length;
    };
    struct DebugNormalDraw {
        SharedPtr<Mesh> mesh;
        glm::mat4 transform;
        glm::vec3 normalColor,
                  tangentColor,
                  bitangentColor;
        float length;
    };
        
    
private:
    void renderWireframe();
    void renderNormals();
    
private:
    Root *mRoot = nullptr;
    DebugRenderable *mWireRenderable = nullptr,
                    *mNormalRenderable = nullptr;
    
    std::vector<DebugDraw> mWireFramesDraws;
    std::vector<DebugNormalDraw> mNormalDraws;
    
    SharedPtr<Material> mWireFrameMaterial;
    struct {
        GLint color, modelMatrix;
    } mWireFrameLoc;
    
    SharedPtr<Material> mNormalMaterial;
    struct {
        GLint normalColor, tangentColor, bitangentColor, length, modelMatrix;
    } mNormalLoc;
    
};