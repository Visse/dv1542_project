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
#include <glm/gtc/matrix_transform.hpp>

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
        uniforms.color = glm::vec4(getColor(),mIntensity);
    
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

AmbientLight::AmbientLight( Root *root ) :
    mRoot(root)
{
    ResourceManager *resourceMgr = root->getResourceManager();
    
    mMaterial = resourceMgr->getMaterialAutoPack("AmbientMaterial");
    
    SharedPtr<GpuProgram> program = mMaterial->getProgram();

    mBlockLoc = program->getUniformBlockLocation("AmbientLight");
}

void AmbientLight::queueRenderable( LowLevelRenderer &renderer )
{
    QueueOperationParams params;
        params.material = mMaterial.get();
        params.drawMode = DrawMode::Points;
        params.renderQueue = RQ_LightFirst;
        params.uniforms[0] = renderer.aquireUniformBuffer( mBlockLoc, getColor() );
        params.vertexCount = 1;
        
    renderer.queueOperation( params );
}

SpotLight::SpotLight( Root *root )
{
    ResourceManager *resourceMgr = root->getResourceManager();
    
    mMesh = resourceMgr->getMeshAutoPack("Cone");
    mMaterial = resourceMgr->getMaterialAutoPack("SpotLightMaterial");
        
    SharedPtr<GpuProgram> program = mMaterial->getProgram();
    
    const UniformBlockLayout &block = program->getUniformBlockLayout("SpotLight");
    const UniformBlockLayout &expectedLayout = SpotLightUniforms::GetUniformBlockLayout();
    
    if( !expectedLayout.canBeUsedAs(block) )  {
        throw std::runtime_error( "SpotLight uniform block doesn't match the expected layout!" );
    }
    
    mBlockLoc = program->getUniformBlockLocation("SpotLight");
}

void SpotLight::queueRenderable( LowLevelRenderer &renderer )
{
    float radie = glm::tan(mOuterAngle)*mOuterDistance;
    
    SpotLightUniforms uniforms;
        uniforms.modelMatrix = glm::scale(getTransform(), glm::vec3(radie,radie,mOuterDistance));
        uniforms.color = glm::vec4(getColor(),mIntensity);
        uniforms.distance = glm::vec2( mInnerDistance, mOuterDistance );
        uniforms.angle = glm::cos(glm::vec2(mInnerAngle, mOuterAngle));
        
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

