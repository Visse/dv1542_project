#include "SkyBox.h"
#include "Root.h"
#include "ResourceManager.h"
#include "LowLevelRenderer.h"
#include "Mesh.h"

SkyBox::SkyBox( Root *root, const SharedPtr<Material> &material ) :
    mRoot(root),
    mMaterial(material)
{
    ResourceManager *resourceMgr = mRoot->getResourceManager();
    mMesh = resourceMgr->getMeshAutoPack("Cube");
    
    BoundingSphere bounds( glm::vec3(), std::numeric_limits<float>::infinity() );
    setBoundingSphere( bounds );
}

void SkyBox::queueRenderable( LowLevelRenderer &renderer )
{
    LowLevelOperationParams params;
        params.indexBuffer = mMesh->getIndexBuffer().get();
        params.vao = mMesh->getVertexArrayObject().get();
        params.drawMode = DrawMode::Triangles;
        params.renderQueue = RQ_LightLast;
        params.material = mMaterial.get();
        params.faceCulling = false;
        
    for( const SubMesh &submesh: mMesh->getSubMeshes() ) {
        params.vertexStart = submesh.vertexStart;
        params.vertexCount = submesh.vertexCount;
        
        renderer.queueOperation( params );
    }
}

