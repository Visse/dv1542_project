#include "Entity.h"
#include "Mesh.h"
#include "Material.h"
#include "GpuProgram.h"
#include "LowLevelRenderer.h"

#include "GLinclude.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

Entity::Entity( const SharedPtr<Mesh> &mesh ) :
    mMesh(mesh)
{
    const auto &subMeshes = mMesh->getSubMeshes();
    
    mRenderers.reserve( subMeshes.size() );
    for( int i=0, c=subMeshes.size(); i < c; ++i ) {
        const SubMesh &subMesh = mMesh->getSubMeshes().at(i);
        mRenderers.emplace_back( this, mMesh, &subMesh);
    }
}

void Entity::queueRenderable( LowLevelRenderer &renderer )
{
    LowLevelRenderOperation operation;
        operation.indexBuffer = mMesh->getIndexBuffer().get();
        operation.vao = mMesh->getVertexArrayObject().get();
        
    
    for( SubMeshRenderer &subMeshRenderer : mRenderers ) {
        const SubMesh *subMesh = subMeshRenderer.getSubMesh();
        operation.material = subMesh->material.get();
        operation.renderable = &subMeshRenderer;
        
        renderer.queueOperation( operation );
    }
}


SubMeshRenderer::SubMeshRenderer( SceneObject *object, const SharedPtr<Mesh> &mesh, const SubMesh *submesh ) :
    mObject(object),
    mMesh(mesh),
    mSubMesh(submesh)
{
    mModelMatrixLoc = submesh->material->getProgram()->getUniformLocation("ModelMatrix");
}

void SubMeshRenderer::render()
{
    glm::mat4 transform = mObject->getTransform();
    glUniformMatrix4fv( mModelMatrixLoc, 1, GL_FALSE, glm::value_ptr(transform) );
    glDrawElements( GL_TRIANGLES, mSubMesh->vertexCount, GL_UNSIGNED_INT, reinterpret_cast<GLvoid*>(( mSubMesh->vertexStart*4)) );
}

const SubMesh* SubMeshRenderer::getSubMesh()
{
    return mSubMesh;
}
