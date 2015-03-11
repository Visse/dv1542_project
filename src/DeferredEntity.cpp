#include "DeferredEntity.h"
#include "Mesh.h"
#include "Material.h"
#include "GpuProgram.h"
#include "Renderer.h"
#include "UniformBlockDefinitions.h"
#include "Root.h"
#include "ResourceManager.h"

#include "GLinclude.h"

#include <glm/gtc/type_ptr.hpp>


DeferredEntity::DeferredEntity( Root *root, const SharedPtr<Mesh> &mesh, const DeferredMaterial &material  ) :
    mMesh(mesh),
    mMaterial(material)
{
    setRenderQueue( RQ_DeferredDefault );
    setBoundingSphere( mMesh->getBoundingSphere() );
    
    const auto &submeshes = mMesh->getSubMeshes();
}

void DeferredEntity::submitRenderer( Renderer &renderer )
{
    renderer.addMesh( mMesh, mMaterial, getTransform() );
}

