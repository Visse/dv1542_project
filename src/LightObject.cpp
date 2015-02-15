#include "LightObject.h"
#include "LowLevelRenderer.h"
#include "Mesh.h"
#include "Root.h"
#include "ResourceManager.h"
#include "Material.h"
#include "GpuProgram.h"
#include "GLinclude.h"
#include "UniformBlockDefinitions.h"

#include <glm/gtc/type_ptr.hpp>

PointLight::PointLight( Root *root ) :
    mRoot(root)
{
    ResourceManager *resourceMgr = root->getResourceManager();
    
    mMesh = resourceMgr->getMeshAutoPack("Sphere");
    mMaterial = resourceMgr->getMaterialAutoPack("PointLightMaterial");
    
    
    SharedPtr<GpuProgram> program = mMaterial->getProgram();
    
    const UniformBlockLayout &block = program->getUniformBlockLayout("PointLight");
    const UniformBlockLayout &expectedLayout = PointLightUniforms::GetUniformBlockLayout();
    
    if( !expectedLayout.canBeUsedAs(block) )  {
        throw std::runtime_error( "PointLight uniform block doesn't match the expected layout!" );
    }
    
    mBlockLoc = program->getUniformBlockLocation("PointLight");
}

void PointLight::queueRenderable( LowLevelRenderer &renderer )
{
    PointLightUniforms uniforms;
        uniforms.modelMatrix = getTransform();
        uniforms.radius = glm::vec2( mInnerRadius, mOuterRadius );
        uniforms.color = glm::vec4(mColor,1.f);
    
    QueueOperationParams params;
        params.indexBuffer = mMesh->getIndexBuffer().get();
        params.vao = mMesh->getVertexArrayObject().get();
        params.material = mMaterial.get();
        params.drawMode = DrawMode::Triangles;
        params.renderQueue = RQ_Light;
        params.uniforms[0] = renderer.aquireUniformBuffer(mBlockLoc, uniforms);

    const auto &submeshes =  mMesh->getSubMeshes();
    
    for( const SubMesh &submesh : submeshes ) {
        params.vertexStart = submesh.vertexStart;
        params.vertexCount = submesh.vertexCount;
        renderer.queueOperation( params );
    }
}