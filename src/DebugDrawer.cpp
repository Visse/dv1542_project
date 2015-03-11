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
#include "Renderer.h"
#include "FrameListener.h"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

class DebugDrawer::DebugFrameListener :
    public FrameListener
{
public:
    DebugFrameListener( DebugDrawer *drawer, const SharedPtr<GpuProgram> &wireFrameShader, const SharedPtr<GpuProgram> &normalShader ) :
        mWireFrameShader(wireFrameShader),
        mNormalShader(normalShader)
    {
        mGraphicsMgr = drawer->mRoot->getGraphicsManager();
        mRenderer = mGraphicsMgr->getRenderer();
        
        mGraphicsMgr->addFrameListener( this );
        
        mWireRenderable.drawer = drawer;
        mNormalRenderable.drawer = drawer;
    }
    virtual ~DebugFrameListener()
    {
        mGraphicsMgr->removeFrameListener( this );
    }
    
    virtual void onFrameBegun() {
        CustomRenderableSettings settings;
            settings.blendMode = BlendMode::Replace;
            settings.program = mWireFrameShader;
        mRenderer->addCustomRenderable( settings, &mWireRenderable );
        
        settings.program = mNormalShader;
        mRenderer->addCustomRenderable( settings, &mNormalRenderable );
    }
    
private:
    struct : public Renderable {
        virtual void render( Renderer &renderer ) {
            drawer->renderWireFrames();
        }
        
        DebugDrawer *drawer;
    } mWireRenderable;
    struct : public Renderable {
        virtual void render( Renderer &renderer ) {
            drawer->renderNormals();
        }
        
        DebugDrawer *drawer;
    } mNormalRenderable;
    
private:
    GraphicsManager *mGraphicsMgr;
    Renderer *mRenderer;
    SharedPtr<GpuProgram> mWireFrameShader,
                          mNormalShader;
};


bool DebugDrawer::init( Root *root )
{
    mRoot = root;
    
    ResourceManager *resourceMgr = mRoot->getResourceManager();

    mSphereMesh = resourceMgr->getMeshAutoPack( "Sphere" );
    mConeMesh = resourceMgr->getMeshAutoPack( "Cone" );
    mBoxMesh = resourceMgr->getMeshAutoPack( "Cube" );
    
    return true;
}

void DebugDrawer::postInit()
{
    ResourceManager *resourceMgr = mRoot->getResourceManager();
    
    SharedPtr<GpuProgram> wireFrameShader = resourceMgr->getGpuProgramAutoPack( "DebugDrawerWireShader" );
    SharedPtr<GpuProgram> normalShader = resourceMgr->getGpuProgramAutoPack( "DebugDrawerNormalShader" );;
    
    mRenderer = mRoot->getGraphicsManager()->getRenderer();
    mFrameListener = new DebugFrameListener( this, wireFrameShader, normalShader );
}

void DebugDrawer::destroy()
{
    delete mFrameListener;
    mFrameListener = nullptr;
    
    mRoot = nullptr;
    
    mWireFramesDraws.clear();
    mNormalDraws.clear();
}

void DebugDrawer::update( float dt )
{
    mWireFramesDraws.clear();
    mNormalDraws.clear();
}

void DebugDrawer::drawWireFrame( const SharedPtr<Mesh> &mesh, const glm::mat4 &transform, const glm::vec4 &color )
{
    WireDrawUniformBlock uniforms;
        uniforms.modelMatrix = transform;
        uniforms.color = color;
    
    DebugWireDraw draw;
        draw.mesh = mesh;
        draw.uniforms = mRenderer->aquireUniformBuffer( uniforms );
        
    mWireFramesDraws.push_back( draw );
}

void DebugDrawer::drawVertexNormals( const SharedPtr<Mesh> &mesh, const glm::mat4 &transform, float length, 
                            const glm::vec4 &normalColor, const glm::vec4 &tangentColor, const glm::vec4 &bitangentColor
                          )
{
    NormalDrawUniformBlock uniforms;
        uniforms.modelMatrix = transform;
        uniforms.normalColor = normalColor;
        uniforms.tangentColor = tangentColor;
        uniforms.bitangentColor = bitangentColor;
        uniforms.length = length;
    
    DebugNormalDraw draw;
        draw.mesh = mesh;
        draw.uniforms = mRenderer->aquireUniformBuffer( uniforms );
        
    mNormalDraws.push_back( draw );
}

void DebugDrawer::drawWireSphere( float radius, const glm::mat4 &transform, const glm::vec4 &color )
{
    glm::mat4 t = glm::scale( transform, glm::vec3(glm::max(radius,0.001f)) );
    drawWireFrame( mSphereMesh, t, color );
}

void DebugDrawer::drawWireCone( float height, float radius, const glm::mat4 &transform, const glm::vec4 &color )
{
    glm::mat4 t = glm::scale( transform, glm::vec3(radius,radius,height) );
    drawWireFrame( mConeMesh, t, color );
}

void DebugDrawer::drawWireConeAngle( float height, float angle, const glm::mat4 &transform, const glm::vec4 &color )
{
    float radius = glm::tan(angle) * height;
    drawWireCone( height, radius, transform, color );
}

void DebugDrawer::drawWireBox( const glm::vec3 &hsize, const glm::mat4 &transform, const glm::vec4 &color )
{
    glm::mat4 t = glm::scale( transform, hsize );
    drawWireFrame( mBoxMesh, t, color );
}

void DebugDrawer::renderWireFrames()
{
    for( const DebugWireDraw &draw : mWireFramesDraws ) {
        glBindBufferRange( GL_UNIFORM_BUFFER, 1, draw.uniforms.getBuffer(), draw.uniforms.getOffset(), draw.uniforms.getSize() );
        renderMesh( draw.mesh, GL_TRIANGLES );
    }
}

void DebugDrawer::renderNormals()
{
    for( const DebugNormalDraw &draw : mNormalDraws ) {
        glBindBufferRange( GL_UNIFORM_BUFFER, 1, draw.uniforms.getBuffer(), draw.uniforms.getOffset(), draw.uniforms.getSize() );
        renderMesh( draw.mesh, GL_POINTS );
    }
}

void DebugDrawer::renderMesh( const SharedPtr<Mesh> &mesh, GLenum mode )
{
    mesh->getVertexArrayObject()->bindVAO();
    auto indexBuffer = mesh->getIndexBuffer();
    
    if( indexBuffer ) {
        indexBuffer->bindBuffer();
    }
    
    for( const SubMesh &submesh : mesh->getSubMeshes() )
    {
        if( indexBuffer ) {
            glDrawElements( mode, submesh.vertexCount, GL_UNSIGNED_INT, reinterpret_cast<GLvoid*>(sizeof(GLint)*submesh.vertexStart) );
        }
        else {
            glDrawArrays( mode, submesh.vertexStart, submesh.vertexCount );
        }
    }
    
}


           