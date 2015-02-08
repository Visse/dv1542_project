#include "LightObject.h"
#include "LowLevelRenderer.h"
#include "Mesh.h"
#include "Root.h"
#include "ResourceManager.h"
#include "Material.h"
#include "GpuProgram.h"
#include "GLinclude.h"

#include <glm/gtc/type_ptr.hpp>

PointLight::PointLight( Root *root ) :
    mRoot(root)
{
    ResourceManager *resourceMgr = root->getResourceManager();
    
    mMesh = resourceMgr->getMeshAutoPack("Sphere");
    mMaterial = resourceMgr->getMaterialAutoPack("PointLightMaterial");
    
    mRenderer.init( this );
}

void PointLight::queueRenderable( LowLevelRenderer &renderer )
{
    LowLevelRenderOperation operation;
        operation.indexBuffer = mMesh->getIndexBuffer().get();
        operation.vao = mMesh->getVertexArrayObject().get();
        operation.material = mMaterial.get();
        operation.renderable = &mRenderer;
    
    renderer.queueOperation( operation, RQ_Light);
}

void PointLightRenderer::init( PointLight *light )
{
    mLight = light;
    
    SharedPtr<GpuProgram> shader = mLight->mMaterial->getProgram();
    mModelMatrixLoc = shader->getUniformLocation("ModelMatrix");
    mOuterRadiusLoc = shader->getUniformLocation("OuterRadius");
    mInnerRadiusLoc = shader->getUniformLocation("InnerRadius");
    mColorLoc = shader->getUniformLocation("Color");
}

void PointLightRenderer::render()
{
    const auto &subMeshes = mLight->mMesh->getSubMeshes();
    glm::mat4 modelMatrix = mLight->getTransform();
    
    glUniformMatrix4fv( mModelMatrixLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix) );
    glUniform3fv( mColorLoc, 1, glm::value_ptr(mLight->mColor) );
    glUniform1f( mOuterRadiusLoc, mLight->mOuterRadius );
    glUniform1f( mInnerRadiusLoc, mLight->mInnerRadius );
    
    for( const SubMesh &submesh : subMeshes ) {
        glDrawElements( GL_TRIANGLES, submesh.vertexCount, GL_UNSIGNED_INT, reinterpret_cast<GLvoid*>(sizeof(GLint)*submesh.vertexStart) );
    }
}

