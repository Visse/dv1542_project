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

glm::mat4 PointLight::getInnerLightVolumeMatrix()
{
    return glm::scale( glm::mat4(), glm::vec3(mInnerRadius) );
}

glm::mat4 PointLight::getOuterLightVolumeMatrix()
{
    return glm::scale( glm::mat4(), glm::vec3(mOuterRadius) );
}


AmbientLight::AmbientLight( Root *root ) :
    mRoot(root)
{
    ResourceManager *resourceMgr = root->getResourceManager();
    
    mMaterial = resourceMgr->getMaterialAutoPack("AmbientMaterial");
    
    SharedPtr<GpuProgram> program = mMaterial->getProgram();

    mBlockLoc = program->getUniformBlockLocation("AmbientLight");
    
    BoundingSphere bounds( glm::vec3(), std::numeric_limits<float>::infinity() );
    setBoundingSphere( bounds );
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

glm::mat4 SpotLight::getInnerLightVolumeMatrix()
{
    float radie = glm::tan(mInnerAngle)*mInnerDistance;
    return glm::scale(glm::mat4(), glm::vec3(radie,radie,mInnerDistance));;
}

glm::mat4 SpotLight::getOuterLightVolumeMatrix()
{
    float radie = glm::tan(mOuterAngle)*mOuterDistance;
    return glm::scale(glm::mat4(), glm::vec3(radie,radie,mOuterDistance));;
}

void SpotLight::updateBounds()
{
    float h = 0.5f*mOuterDistance,
          w = mOuterDistance*glm::tan(mOuterAngle);
    
    float radius = glm::sqrt(h*h+w*w);
    
    setBoundingSphere( BoundingSphere(glm::vec3(0,0,-h),radius) );
}

BoxLight::BoxLight( Root *root ) :
    mRoot(root)
{
    ResourceManager *resourceMgr = root->getResourceManager();
    
    mMesh = resourceMgr->getMeshAutoPack("Cube");
    mMaterial = resourceMgr->getMaterialAutoPack("BoxLightMaterial");
        
    SharedPtr<GpuProgram> program = mMaterial->getProgram();
    
    const UniformBlockLayout &block = program->getUniformBlockLayout("BoxLight");
    const UniformBlockLayout &expectedLayout = BoxLightUniforms::GetUniformBlockLayout();
    
    if( !expectedLayout.canBeUsedAs(block) )  {
        throw std::runtime_error( "BoxLight uniform block doesn't match the expected layout!" );
    }
    
    mBlockLoc = program->getUniformBlockLocation("BoxLight");
}

void BoxLight::queueRenderable( LowLevelRenderer &renderer )
{
    glm::vec3 scale(0.25f,0.25f,0.5f);
    glm::mat4 modelMatrix = glm::translate( glm::scale(getTransform(), mOuterSize*scale), glm::vec3(0.f,0.f,-1.f) );
    
    BoxLightUniforms uniforms;
        uniforms.modelMatrix = modelMatrix;
        uniforms.color = glm::vec4(getColor(),mIntensity);
        uniforms.innerSize = mInnerSize*scale;
        uniforms.outerSize = mOuterSize*scale;
        
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

glm::mat4 BoxLight::getInnerLightVolumeMatrix()
{
    glm::vec3 scale = glm::vec3(0.25f,0.25f,0.5f);
    return glm::translate( glm::scale(glm::mat4(), mInnerSize*scale), glm::vec3(0.f,0.f,-1.f) );
}

glm::mat4 BoxLight::getOuterLightVolumeMatrix()
{
    glm::vec3 scale = glm::vec3(0.25f,0.25f,0.5f);
    return glm::translate( glm::scale(glm::mat4(), mOuterSize*scale), glm::vec3(0.f,0.f,-1.f) );
}

void BoxLight::updateBounds()
{
    glm::vec3 scale = glm::vec3(0.25f,0.25f,0.5f);
    glm::vec3 size = mOuterSize * scale;
    
    float radius = glm::length(size);
    
    setBoundingSphere( BoundingSphere(glm::vec3(0,0,-size.z), radius) );
}
