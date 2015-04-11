#include "ComputeWater.h"
#include "Renderable.h"
#include "Root.h"
#include "ResourceManager.h"
#include "Renderer.h"
#include "GLinclude.h"
#include "GpuBuffer.h"
#include "VertexArrayObject.h"
#include "Texture.h"
#include "GpuProgram.h"

#include <glm/vec3.hpp>

struct ComputeWater::WaterRenderable :
    public Renderable
{
    ComputeWater *computeWater;
    virtual void render( Renderer &renderer ){
        computeWater->render(renderer);
    }
};

ComputeWater::ComputeWater( SceneObjectFactory *factory, Root *root ) :
    SceneObject( factory ),
    mRoot(root)
{
    
    mRenderable = new WaterRenderable;
    mRenderable->computeWater = this;
    
    createSurface();
    setWaterSizeScale( mWaterSizeScale );
    
    
    ResourceManager *resourceMgr = mRoot->getResourceManager();
    
    mWaterShader = resourceMgr->getGpuProgramAutoPack( "WaterShader" );
    mWaterWireShader = resourceMgr->getGpuProgramAutoPack( "WaterWireShader" );
    mWaterSimulation = resourceMgr->getGpuProgramAutoPack( "WaterSimulation" );
    
    mNormalTexture = resourceMgr->getTextureAutoPack( "WaterNormal" );
    
    mSimTexture = Texture::CreateTexture( TextureType::Red, glm::uvec2(512,512), 1 );
    
    mSimTexture->bindTexture(0);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
}

ComputeWater::~ComputeWater()
{
    delete mRenderable;
}

void ComputeWater::update( float dt )
{
    mWaterSimulation->bindProgram();
    glBindImageTexture( 0, mSimTexture->getGLTexture(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R8 );
    
    mCurrentTime += dt;
    GLuint loc = glGetUniformLocation( mWaterSimulation->getGLProgram(), "time" );
    glUniform1f( loc, mCurrentTime );
    
    glDispatchCompute( 512/4, 512/2, 1 );
}

void ComputeWater::setWaterSizeScale( float waterSizeScale )
{
    mWaterSizeScale = waterSizeScale;
    
    float radius = glm::sqrt(mWaterSizeScale*mWaterSizeScale);
    
    setBoundingSphere( BoundingSphere(glm::vec3(), radius) );
}

void ComputeWater::submitRenderer( Renderer &renderer )
{
    RenderingUniforms uniforms;
    uniforms.modelMatrix = glm::scale( getTransform(), glm::vec3(mWaterSizeScale) );
    uniforms.depthFalloff = mDepthFalloff;
    uniforms.heightScale = mHeightScale;
    uniforms.scrollDirection = glm::vec2(0.02,0.02);
    uniforms.currentTime = mCurrentTime;
    uniforms.lodScale = mLODScale;
    uniforms.frensel = mFrensel;
    uniforms.frenselFalloff = mFrenselFalloff;
    uniforms.waterColor = mWaterColor;
    uniforms.lightPos = mLightPosition;
    uniforms.lightColor = mLightColor;
    
    CustomRenderableSettings settings;
        settings.vao = mVAO;
        settings.uniforms[0] = renderer.getSceneUniforms();
        settings.uniforms[1] = renderer.aquireUniformBuffer( uniforms );
        settings.textures[0] = renderer.getGBufferDepthTexture();
        settings.textures[1] = renderer.getGBufferLitDiffuseTexture();
        settings.textures[2] = mSimTexture;
        settings.textures[4] = mNormalTexture;
        settings.renderable = mRenderable;
        settings.blendMode = BlendMode::Replace;
        
    if( mUseWireFrame || renderer.getRenderWireFrame() ) {
        settings.program = mWaterWireShader;
    }
    else {
        settings.program = mWaterShader;
    }
    renderer.addCustomRenderable( settings );
}

void ComputeWater::render( Renderer &renderer )
{
    glDisable( GL_CULL_FACE );
    glPatchParameteri( GL_PATCH_VERTICES, 4 );
    glDrawArrays( GL_PATCHES, 0, 16 );
    glEnable( GL_CULL_FACE );
};

void ComputeWater::createSurface()
{
    struct Vertex {
        glm::vec3 pos;
        glm::vec2 tex;
    } surface[16] = {
        {glm::vec3(-1, 0, 0), glm::vec2(0.0,0.5)},
        {glm::vec3( 0, 0, 0), glm::vec2(0.5,0.5)},
        {glm::vec3( 0, 0,-1), glm::vec2(0.5,0.0)},
        {glm::vec3(-1, 0,-1), glm::vec2(0.0,0.0)},
        
        {glm::vec3( 0, 0, 0), glm::vec2(0.5,0.5)},
        {glm::vec3( 1, 0, 0), glm::vec2(1.0,0.5)},
        {glm::vec3( 1, 0,-1), glm::vec2(1.0,0.0)},
        {glm::vec3( 0, 0,-1), glm::vec2(0.5,0.0)},
        
        {glm::vec3(-1, 0, 1), glm::vec2(0.0,1.0)},
        {glm::vec3( 0, 0, 1), glm::vec2(0.5,1.0)},
        {glm::vec3( 0, 0, 0), glm::vec2(0.5,0.5)},
        {glm::vec3(-1, 0, 0), glm::vec2(0.0,0.5)},
        
        {glm::vec3( 0, 0, 1), glm::vec2(0.5,1.0)},
        {glm::vec3( 1, 0, 1), glm::vec2(1.0,1.0)},
        {glm::vec3( 1, 0, 0), glm::vec2(1.0,0.5)},
        {glm::vec3( 0, 0, 0), glm::vec2(0.5,0.5)},
    };
    
    mWaterSurface = GpuBuffer::CreateBuffer( BufferType::Vertexes, sizeof(Vertex)*16, BufferUsage::WriteOnly, BufferUpdate::Static );
    mWaterSurface->bindBuffer();
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(Vertex)*16, surface );
    
    mVAO = makeSharedPtr<VertexArrayObject>();
    mVAO->bindVAO();
    
    mWaterSurface->bindBuffer();
    mVAO->setVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex,pos) );
    mVAO->setVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex,tex) );
    
    mVAO->unbindVAO();
    mWaterSurface->unbindBuffer();
}
