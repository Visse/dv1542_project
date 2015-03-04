#include "Entity.h"
#include "Mesh.h"
#include "Material.h"
#include "GpuProgram.h"
#include "LowLevelRenderer.h"
#include "UniformBlockDefinitions.h"
#include "Root.h"
#include "ResourceManager.h"

#include "GLinclude.h"

#include <glm/gtc/type_ptr.hpp>


Entity::Entity( Root *root, const SharedPtr<Mesh> &mesh ) :
    mMesh(mesh)
{
    setRenderQueue( RQ_DeferredDefault );
    setBoundingSphere( mMesh->getBoundingSphere() );
    
    const auto &submeshes = mMesh->getSubMeshes();
    
    mSubMeshes.reserve( submeshes.size() );
    
//     ResourceManager *resourceMgr = 
    SharedPtr<Material> missingMaterial;
    const UniformBlockLayout &expectedLayout = EntityUniforms::GetUniformBlockLayout();
    
    for( const SubMesh &submesh : submeshes ) {
        SubMeshInfo info;
            info.material = submesh.material;
            info.vertexStart = submesh.vertexStart;
            info.vertexCount = submesh.vertexCount;
        
        if( !info.material ) {
            info.material = missingMaterial;
        }
        
        SharedPtr<GpuProgram> program = info.material->getProgram();
        info.blockLoc = program->getUniformBlockLocation("Entity");
        
        const UniformBlockLayout &layout = program->getUniformBlockLayout("Entity");
        if( !expectedLayout.canBeUsedAs(layout) ) {
            throw std::runtime_error( "Entity uniform block doesn't match the expected layout!" );
        }
        
        mSubMeshes.push_back( info );
    }
    
}

void Entity::queueRenderable( LowLevelRenderer &renderer )
{
    unsigned int renderQueue = getRenderQueue();
    
    LowLevelOperationParams params;
        params.indexBuffer = mMesh->getIndexBuffer().get();
        params.vao = mMesh->getVertexArrayObject().get();
        params.drawMode = DrawMode::Triangles;
        params.renderQueue = renderQueue;
        
    UniformBuffer buffer = renderer.aquireUniformBuffer( sizeof(EntityUniforms) );
    EntityUniforms uniforms;
        uniforms.modelMatrix = getTransform();
    
    buffer.setRawContent( 0, &uniforms, sizeof(EntityUniforms) );
    
    params.uniforms[0] = buffer;
        
    for( const SubMeshInfo &info : mSubMeshes ) {
        params.material = info.material.get();
        params.vertexStart = info.vertexStart;
        params.vertexCount = info.vertexCount;
        params.uniforms[0].setIndex( info.blockLoc );
        
        renderer.queueOperation( params );
    }
}
