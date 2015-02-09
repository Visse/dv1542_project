#include "DebugDrawer.h"
#include "FrameListener.h"
#include "Root.h"
#include "GraphicsManager.h"
#include "ResourceManager.h"
#include "Camera.h"
#include "Mesh.h"
#include "Material.h"
#include "GLinclude.h"
#include "GpuProgram.h"
#include "Renderable.h"
#include "LowLevelRenderer.h"

#include <glm/gtc/type_ptr.hpp>

struct DebugDrawer::DebugRenderable :
    public Renderable
{
    DebugDrawer *drawer;
    void(DebugDrawer::*renderFunction)();
    
    virtual void render() override {
        (drawer->*renderFunction)();
    }
};

bool DebugDrawer::init( Root *root )
{
    mRoot = root;
    mWireRenderable = new DebugRenderable;
    mWireRenderable->drawer = this;
    mWireRenderable->renderFunction = &DebugDrawer::renderWireframe;
    
    mNormalRenderable = new DebugRenderable;
    mNormalRenderable->drawer = this;
    mNormalRenderable->renderFunction = &DebugDrawer::renderNormals;
    
    ResourceManager *resourceMgr = mRoot->getResourceManager();
    mWireFrameMaterial = resourceMgr->getMaterialAutoPack( "DebugDrawerWireMaterial" );
    mNormalMaterial = resourceMgr->getMaterialAutoPack( "DebugDrawerNormalMaterial" );
    
    SharedPtr<GpuProgram> program = mWireFrameMaterial->getProgram();
    mWireFrameLoc.color = program->getUniformLocation("Color");
    mWireFrameLoc.modelMatrix = program->getUniformLocation("ModelMatrix");
    
    program = mNormalMaterial->getProgram();
    mNormalLoc.normalColor = program->getUniformLocation("NormalColor");
    mNormalLoc.tangentColor = program->getUniformLocation("TangentColor");
    mNormalLoc.bitangentColor = program->getUniformLocation("BitangentColor");
    mNormalLoc.length = program->getUniformLocation("Length");
    mNormalLoc.modelMatrix = program->getUniformLocation("ModelMatrix");
    
    return true;
}

void DebugDrawer::destroy()
{
    delete mWireRenderable;
    mWireRenderable = nullptr;
    delete mNormalRenderable;
    mNormalRenderable = nullptr;
    
    mRoot = nullptr;
}

void DebugDrawer::update( float dt )
{
    mWireFramesDraws.clear();
    mNormalDraws.clear();
}

void DebugDrawer::drawWireFrame( const SharedPtr<Mesh> &mesh, const glm::mat4 &transform, const glm::vec3 &color )
{
    DebugDraw draw;
        draw.mesh = mesh;
        draw.transform = transform;
        draw.color = color;
        
    mWireFramesDraws.push_back( draw );
}

void DebugDrawer::drawVertexNormals( const SharedPtr<Mesh> &mesh, const glm::mat4 &transform, float length, 
                            const glm::vec3 &normalColor, const glm::vec3 &tangentColor, const glm::vec3 &bitangentColor
                          )
{
    DebugNormalDraw draw;
        draw.mesh = mesh;
        draw.transform = transform;
        draw.normalColor = normalColor;
        draw.tangentColor = tangentColor;
        draw.bitangentColor = bitangentColor;
        draw.length = length;
        
    mNormalDraws.push_back( draw );
}

void DebugDrawer::queueRenderable( LowLevelRenderer &renderer )
{
    LowLevelRenderOperation operation;
        operation.material = mWireFrameMaterial.get();
        operation.renderable = mWireRenderable;
    
    renderer.queueOperation( operation, RQ_Overlay );
    
    operation.material = mNormalMaterial.get();
    operation.renderable = mNormalRenderable;
    
    renderer.queueOperation( operation, RQ_Overlay );
}

void DebugDrawer::renderWireframe(  )
{
    for( const DebugDraw &draw : mWireFramesDraws ) {
        draw.mesh->getIndexBuffer()->bindBuffer();
        draw.mesh->getVertexArrayObject()->bindVAO();
        const auto &subMeshes = draw.mesh->getSubMeshes();
        
        glUniformMatrix4fv( mWireFrameLoc.modelMatrix, 1, GL_FALSE, glm::value_ptr(draw.transform) );
        glUniform3fv( mWireFrameLoc.color, 1, glm::value_ptr(draw.color) );
        
        for( const SubMesh &submesh : subMeshes ) {
            glDrawElements( GL_TRIANGLES, submesh.vertexCount, GL_UNSIGNED_INT, reinterpret_cast<GLvoid*>(submesh.vertexStart*4) );
        }
    }
}

void DebugDrawer::renderNormals()
{
    for( const DebugNormalDraw &draw : mNormalDraws ) {
        draw.mesh->getIndexBuffer()->bindBuffer();
        draw.mesh->getVertexArrayObject()->bindVAO();     
        
        const auto &subMeshes = draw.mesh->getSubMeshes();
        
        glUniformMatrix4fv( mNormalLoc.modelMatrix, 1, GL_FALSE, glm::value_ptr(draw.transform) );
        glUniform3fv( mNormalLoc.normalColor, 1, glm::value_ptr(draw.normalColor) );
        glUniform3fv( mNormalLoc.tangentColor, 1, glm::value_ptr(draw.tangentColor) );
        glUniform3fv( mNormalLoc.bitangentColor, 1, glm::value_ptr(draw.bitangentColor) );
        glUniform1f( mNormalLoc.length, draw.length  );
        
        for( const SubMesh &submesh : subMeshes ) {
            glDrawElements( GL_POINTS, submesh.vertexCount, GL_UNSIGNED_INT, reinterpret_cast<GLvoid*>(submesh.vertexStart*4) );
        }
    }
}


