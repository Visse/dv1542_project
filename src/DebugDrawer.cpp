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

struct DebugDrawer::DebugWireRenderable :
    public Renderable
{
    DebugDrawer *drawer;
    
    virtual void render() override {
        drawer->renderWireframe();
    }
};

bool DebugDrawer::init( Root *root )
{
    mRoot = root;
    mWireRenderable = new DebugWireRenderable;
    mWireRenderable->drawer = this;
    
    ResourceManager *resourceMgr = mRoot->getResourceManager();
    mWireFrameMaterial = resourceMgr->getMaterialAutoPack( "DebugDrawerWireMaterial" );
    
    SharedPtr<GpuProgram> program = mWireFrameMaterial->getProgram();
    mWireFrameLoc.color = program->getUniformLocation("Color");
    mWireFrameLoc.modelMatrix = program->getUniformLocation("ModelMatrix");
    
    
    return true;
}

void DebugDrawer::destroy()
{
    delete mWireRenderable;
    mWireRenderable = nullptr;
    mRoot = nullptr;
}

void DebugDrawer::update( float dt )
{
    mWireFramesDraws.clear();
}

void DebugDrawer::drawWireFrame( const SharedPtr<Mesh> &mesh, const glm::mat4 &transform, const glm::vec3 &color )
{
    WireFrameDraw draw;
        draw.mesh = mesh;
        draw.transform = transform;
        draw.color = color;
        
    mWireFramesDraws.push_back( draw );
}

void DebugDrawer::queueRenderable( LowLevelRenderer &renderer )
{
    LowLevelRenderOperation operation;
    operation.material = mWireFrameMaterial.get();
    operation.renderable = mWireRenderable;
    
    renderer.queueOperation( operation, RQ_Overlay );
}

void DebugDrawer::renderWireframe(  )
{
    mWireFrameMaterial->bindMaterial();
    for( const WireFrameDraw &draw : mWireFramesDraws ) {
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

