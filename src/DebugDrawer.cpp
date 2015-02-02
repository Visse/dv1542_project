#include "DebugDrawer.h"
#include "FrameListener.h"
#include "Root.h"
#include "GraphicsManager.h"
#include "ResourceManager.h"
#include "Camera.h"
#include "Mesh.h"
#include "Material.h"
#include "GLinclude.h"

struct DebugDrawer::DebugFrameListener :
    public FrameListener
{
    DebugDrawer *drawer;
    
    virtual void onPostCameraDraw( Camera *camera ) {
        drawer->render( camera );
    }
};

bool DebugDrawer::init( Root *root )
{
    mRoot = root;
    mFrameListener = new DebugFrameListener;
    mFrameListener->drawer = this;
    
    ResourceManager *resourceMgr = mRoot->getResourceManager();
    mWireFrameMaterial = resourceMgr->getMaterialAutoPack( "DebugDrawerWireMaterial" );
    
    GraphicsManager *graphicsMgr = mRoot->getGraphicsManager();
    graphicsMgr->addFrameListener( mFrameListener );
    
    return true;
}

void DebugDrawer::destroy()
{
    GraphicsManager *graphicsMgr = mRoot->getGraphicsManager();
    graphicsMgr->removeFrameListener( mFrameListener );
    
    delete mFrameListener;
    mFrameListener = nullptr;

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


void DebugDrawer::render( Camera *camera )
{
    mWireFrameMaterial->bindMaterial();
    for( const WireFrameDraw &draw : mWireFramesDraws ) {
        draw.mesh->getIndexBuffer()->bindBuffer();
        draw.mesh->getVertexArrayObject()->bindVAO();
        const auto &subMeshes = draw.mesh->getSubMeshes();
        
        for( const SubMesh &submesh : subMeshes ) {
            glDrawElements( GL_LINES, submesh.vertexCount, GL_UNSIGNED_INT, reinterpret_cast<GLvoid*>(submesh.vertexStart*4) );
        }
    }
}

