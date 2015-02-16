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
#include "LowLevelRenderer.h"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

bool DebugDrawer::init( Root *root )
{
    mRoot = root;
    
    ResourceManager *resourceMgr = mRoot->getResourceManager();
    mWireFrameMaterial = resourceMgr->getMaterialAutoPack( "DebugDrawerWireMaterial" );
    mNormalMaterial = resourceMgr->getMaterialAutoPack( "DebugDrawerNormalMaterial" );
    
    if( !mWireFrameMaterial ) {
        std::cerr << "Failed to load debug wireframe material!" << std::endl;
        return false;
    }
    if( !mNormalMaterial ) {
        std::cerr << "Failed to load debug wireframe material!" << std::endl;
        return false;
    }
    
    SharedPtr<GpuProgram> program = mWireFrameMaterial->getProgram();
    mWireUniformBlockLoc = program->getUniformBlockLocation( "DebugWireframe" );
    
    program = mNormalMaterial->getProgram();
    mNormalUniformBlockLoc = program->getUniformBlockLocation( "DebugNormal" );
    
    mSphereMesh = resourceMgr->getMeshAutoPack( "Sphere" );
    mConeMesh = resourceMgr->getMeshAutoPack( "Cone" );
    
    return true;
}

void DebugDrawer::destroy()
{
    mRoot = nullptr;
    
    mWireFramesDraws.clear();
    mNormalDraws.clear();
    
    mWireFrameMaterial.reset();
    mNormalMaterial.reset();
}

void DebugDrawer::update( float dt )
{
    mWireFramesDraws.clear();
    mNormalDraws.clear();
}

void DebugDrawer::drawWireFrame( const SharedPtr<Mesh> &mesh, const glm::mat4 &transform, const glm::vec4 &color )
{
    DebugDraw draw;
        draw.mesh = mesh;
        draw.transform = transform;
        draw.color = color;
        
    mWireFramesDraws.push_back( draw );
}

void DebugDrawer::drawVertexNormals( const SharedPtr<Mesh> &mesh, const glm::mat4 &transform, float length, 
                            const glm::vec4 &normalColor, const glm::vec4 &tangentColor, const glm::vec4 &bitangentColor
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

void DebugDrawer::drawWireSphere( float radius, const glm::mat4 &transform, const glm::vec4 &color )
{
    glm::mat4 t = glm::scale( transform, glm::vec3(radius) );
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


void DebugDrawer::queueRenderable( LowLevelRenderer &renderer )
{
    queueWireframe( renderer );
    queueNormals( renderer );
}

void DebugDrawer::queueWireframe( LowLevelRenderer &renderer )
{
    QueueOperationParams params;
        params.drawMode = DrawMode::Triangles;
        params.faceCulling = false;
        params.material = mWireFrameMaterial.get();
        params.renderQueue = RQ_Overlay-1;
    
    for( const DebugDraw &draw : mWireFramesDraws ) {
        WireDrawUniformBlock uniforms;
            uniforms.color = glm::vec4(draw.color);
            uniforms.modelMatrix = draw.transform;
            
        params.uniforms[0] = renderer.aquireUniformBuffer( mWireUniformBlockLoc, uniforms );
        
        params.indexBuffer = draw.mesh->getIndexBuffer().get();
        params.vao = draw.mesh->getVertexArrayObject().get();
        
        const auto &subMeshes = draw.mesh->getSubMeshes();
        
        for( const SubMesh &submesh : subMeshes ) {
            params.vertexStart = submesh.vertexStart;
            params.vertexCount = submesh.vertexCount;
            
            renderer.queueOperation( params );
        }
    }
}

void DebugDrawer::queueNormals( LowLevelRenderer &renderer )
{
    QueueOperationParams params;
        params.drawMode = DrawMode::Points;
        params.faceCulling = false;
        params.material = mNormalMaterial.get();
        params.renderQueue = RQ_Overlay-1;
    
    for( const DebugNormalDraw &draw : mNormalDraws ) {
        NormalDrawUniformBlock uniforms;
            uniforms.modelMatrix = draw.transform;
            uniforms.normalColor = draw.normalColor;
            uniforms.tangentColor = draw.tangentColor;
            uniforms.bitangentColor = draw.bitangentColor;
            uniforms.lenght = draw.length;
            
        params.uniforms[0] = renderer.aquireUniformBuffer( mNormalUniformBlockLoc, uniforms );
        params.indexBuffer = draw.mesh->getIndexBuffer().get();
        params.vao = draw.mesh->getVertexArrayObject().get();
        
        const auto &subMeshes = draw.mesh->getSubMeshes();
        
        for( const SubMesh &submesh : subMeshes ) {
            params.vertexStart = submesh.vertexStart;
            params.vertexCount = submesh.vertexCount;
            
            renderer.queueOperation( params );
        }
    }
}


