#include "Renderer.h"
#include "Root.h"
#include "ResourceManager.h"
#include "Texture.h"
#include "Config.h"
#include "FrameBuffer.h"
#include "Scene.h"
#include "Camera.h"
#include "SceneObject.h"
#include "GpuProgram.h"
#include "UniformBufferAllocator.h"
#include "UniformBlockDefinitions.h"
#include "Mesh.h"
#include "Renderable.h"

Renderer::Renderer( Root *root ) :
    mRoot(root)
{
    initGBuffer();
    initSSAO();
    initDeferred();
    
    const Config *config = root->getConfig();
    mWindowSize = glm::uvec2( config->windowWidth, config->windowHeight );
    
    mAllocator = new UniformBufferAllocator;
}

Renderer::~Renderer()
{
    delete mAllocator;
}

void Renderer::renderScene( Scene *scene, Camera *camera )
{
    mCurrentScene = scene;
    
    scene->quarySceneObjects( camera->getFrustrum(), mVisibleObjects );
    
    for( SceneObject *object : mVisibleObjects ) {
        object->submitRenderer( *this );
    }
    
    SceneRenderUniforms sceneUniforms = camera->getSceneUniforms();
    mSceneUniforms = aquireUniformBuffer( sceneUniforms );
    
    AmbientUniforms ambientUniforms = scene->getAmbientUniforms();
    mAmbientUniforms = aquireUniformBuffer( ambientUniforms );
    
    render();
    
    mCurrentScene = nullptr;
}

void Renderer::addMesh( const SharedPtr<Mesh> &mesh, const DeferredMaterial &material, const glm::mat4 &modelMatrix )
{
    EntityInfo info;
        info.mesh = mesh;
        info.material = material;
    
    auto result = mAllocator->getMemory( sizeof(EntityUniforms) );
    info.buffer = result.buffer;
    info.offset = result.offset;
    
    EntityUniforms *uniform = reinterpret_cast<EntityUniforms*>( result.memory );
    uniform->modelMatrix = modelMatrix;
    
    mEntities.push_back( info );
}

void Renderer::addPointLight( UniformBuffer uniforms, const glm::vec3 &pos, float radius, bool shadows )
{
    PointLightInfo info;
        info.uniforms = uniforms;
        info.pos = pos;
        info.radius = radius;
        
    if( shadows ) {
        mPointLights.push_back( info );
    }
    else {
        mPointLightsNoShadow.push_back( info );
    }
}


void Renderer::render()
{   
    mAllocator->flushAndReset();
    
    bindUniforms( 0, mSceneUniforms.getBuffer(), mSceneUniforms.getOffset(), mSceneUniforms.getSize() );
    
    renderDeferred();
//     renderSSAO();
    renderLights();
    renderCustom();
    
    mEntities.clear();
    mVisibleObjects.clear();
    mCustomRenderable.clear();
    mPointLights.clear();
    mPointLightsNoShadow.clear();
}

UniformBuffer Renderer::aquireUniformBuffer( size_t size )
{
    auto result = mAllocator->getMemory( size );
    return UniformBuffer( result.buffer, result.memory, size, result.offset );
}

void Renderer::addCustomRenderable( const CustomRenderableSettings &settings, Renderable *renderable )
{
    CustomRenderable custom;
    custom.renderable = renderable;
    custom.program = settings.program;
    custom.vao = settings.vao;
    custom.indexbuffer = settings.indexbuffer;
    custom.blendMode = settings.blendMode;
    
    std::copy( std::begin(settings.textures), std::end(settings.textures), custom.textures );
    
    for( int i=0; i < 8; ++i ) {
        custom.uniforms[i].buffer = settings.uniforms[i].getBuffer();
        custom.uniforms[i].offset = settings.uniforms[i].getOffset();
        custom.uniforms[i].size = settings.uniforms[i].getSize();
    }
    
    mCustomRenderable.push_back( custom );
}


void Renderer::initGBuffer()
{
    const Config *config = mRoot->getConfig();
    
    glm::uvec2 gbufferSize( config->defferedBufferWidth, config->defferedBufferHeight );
    
    mGBuffer.diffuseTexture = Texture::CreateTexture( TextureType::RGBA, gbufferSize, 1 );
    mGBuffer.normalTexture = Texture::CreateTexture( TextureType::RGB, gbufferSize, 1 );
    mGBuffer.depthTexture = Texture::CreateTexture( TextureType::Depth, gbufferSize, 1 );
    
    mGBuffer.framebuffer = makeSharedPtr<FrameBuffer>();
    
    mGBuffer.framebuffer->attachColorTexture( mGBuffer.diffuseTexture, 0 );
    mGBuffer.framebuffer->attachColorTexture( mGBuffer.normalTexture, 1 );
    mGBuffer.framebuffer->setDepthTexture( mGBuffer.depthTexture );
    
    mGBuffer.frameBufferSize = gbufferSize;
}

void Renderer::initSSAO()
{
    ResourceManager *resourceMgr = mRoot->getResourceManager();
    const Config *config = mRoot->getConfig();
    
    mSSAO.scenePassProgram = resourceMgr->getGpuProgramAutoPack( "SSAOScenePassShader" );
    mSSAO.ssaoProgram = resourceMgr->getGpuProgramAutoPack( "SSAOShader" );
    mSSAO.ssaoBlur = resourceMgr->getGpuProgramAutoPack( "SSAOBlurProgram" );
    
    glm::uvec2 ssaoSize( config->ssaoBufferWidth, config->ssaoBufferHeight );
    
    mSSAO.normalTexture = Texture::CreateTexture( TextureType::RGB, ssaoSize, 1 );
    mSSAO.depthTexture = Texture::CreateTexture( TextureType::Depth, ssaoSize, 1 );
    
    mSSAO.normalTexture->bindTexture(0);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    
    mSSAO.depthTexture->bindTexture(0);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    
    mSSAO.framebuffer = makeSharedPtr<FrameBuffer>();
    
    mSSAO.framebuffer->attachColorTexture( mSSAO.normalTexture, 0 );
    mSSAO.framebuffer->setDepthTexture( mSSAO.depthTexture );
    
    mSSAO.randTexture = resourceMgr->getTextureAutoPack( "NoiseImage" );
    mSSAO.ssaoTexture = Texture::CreateTexture( TextureType::Red, ssaoSize, 1 );
    mSSAO.ssaoBlured = Texture::CreateTexture( TextureType::Red, ssaoSize, 1 );
    
    mSSAO.ssaoFrameBuffer = makeSharedPtr<FrameBuffer>();
    mSSAO.ssaoFrameBuffer->attachColorTexture( mSSAO.ssaoTexture, 0 );
    
    mSSAO.frameBufferSize = ssaoSize;
}

void Renderer::initDeferred()
{
    ResourceManager *resourceMgr = mRoot->getResourceManager();

    mDeferred.entityDeferredProgram = resourceMgr->getGpuProgramAutoPack( "DeferredMeshShader" );
    mDeferred.pointLightProgram = resourceMgr->getGpuProgramAutoPack( "DeferredPointLightShader" );
    mDeferred.pointLightNoShadowProgram = resourceMgr->getGpuProgramAutoPack( "DeferredPointLightNoShadowShader" );
    mDeferred.ambientLightProgram = resourceMgr->getGpuProgramAutoPack( "DeferredAmbientShader" );
    mDeferred.copyDepthProgram = resourceMgr->getGpuProgramAutoPack( "DeferredCopyDepthShader" );
    
    mDeferred.sphereMesh = resourceMgr->getMeshAutoPack( "Sphere" );
}

void Renderer::renderDeferred()
{
    setBlendMode( BlendMode::Replace );
    mGBuffer.framebuffer->bindFrameBuffer();
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    setViewportSize( mGBuffer.frameBufferSize );
    
    mDeferred.entityDeferredProgram->bindProgram();
    
    for( const EntityInfo &info : mEntities ) 
    {
        info.material.diffuseTexture->bindTexture( 0 );
        info.material.normalMap->bindTexture( 1 );
        bindUniforms( 1, info.buffer, info.offset, sizeof(EntityUniforms) );
        drawMesh( info.mesh );
    }
}

void Renderer::renderSSAO()
{
    setBlendMode( BlendMode::Replace );
    
    mSSAO.framebuffer->bindFrameBuffer();
    setViewportSize( mSSAO.frameBufferSize );
    
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    mSSAO.scenePassProgram->bindProgram();
    
    for( const EntityInfo &info : mEntities ) 
    {
        bindUniforms( 1, info.buffer, info.offset, sizeof(EntityUniforms) );
        drawMesh( info.mesh );
    }
    
    mSSAO.ssaoFrameBuffer->bindFrameBuffer();
    
    static float SampleRadius = 0.2, DepthEdge = 0.5;
    
    mSSAO.ssaoProgram->bindProgram();
/*    
    ImGui::Begin("Renderer");
    ImGui::SliderFloat( "SampleRadius", &SampleRadius, 1e-4f, 1.0f, "%.4g" );
    ImGui::SliderFloat( "DepthEdge", &DepthEdge, 1e-4f, 1.0f, "%.4g" );
    ImGui::End();
*/
    GLint loc = glGetUniformLocation( mSSAO.ssaoProgram->getGLProgram(), "SampleRadius" );
    glUniform1f( loc, SampleRadius );
    
    loc = glGetUniformLocation( mSSAO.ssaoProgram->getGLProgram(), "DepthEdge" );
    glUniform1f( loc, DepthEdge );
    
    mSSAO.normalTexture->bindTexture( 0 );
    mSSAO.depthTexture->bindTexture( 1 );
    mSSAO.randTexture->bindTexture( 2 );
    
    glDrawArrays( GL_POINTS, 0, 1 );
    
    
    mSSAO.ssaoBlur->bindProgram();
    glBindImageTexture( 0, mSSAO.ssaoTexture->getGLTexture(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R8 );
    glBindImageTexture( 1, mSSAO.ssaoBlured->getGLTexture(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R8 );
    
    glDispatchCompute( mSSAO.frameBufferSize.x / 4, mSSAO.frameBufferSize.y, 1 );
}

void Renderer::renderLights()
{
    setBlendMode( BlendMode::AddjectiveBlend );
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    setViewportSize( mWindowSize );
    
    mGBuffer.diffuseTexture->bindTexture( 0 );
    mGBuffer.normalTexture->bindTexture( 1 );
    mGBuffer.depthTexture->bindTexture( 2 );
    
    { // copy our gbuffer-depth buffer to the default framebuffer
        mDeferred.copyDepthProgram->bindProgram();
        glDrawArrays( GL_POINTS, 0, 1 );
    }
    
    glDepthMask( GL_FALSE );
    glCullFace( GL_FRONT );
    glDisable( GL_DEPTH_TEST );
    
    { // point lights
        mDeferred.pointLightProgram->bindProgram();
        
        for( const PointLightInfo &info : mPointLights ) {
            bindUniforms( 1, info.uniforms.getBuffer(), info.uniforms.getOffset(), info.uniforms.getSize() );
            
            drawMesh( mDeferred.sphereMesh );
        }
        
        mDeferred.pointLightNoShadowProgram->bindProgram();
        
        for( const PointLightInfo &info : mPointLightsNoShadow ) {
            bindUniforms( 1, info.uniforms.getBuffer(), info.uniforms.getOffset(), info.uniforms.getSize() );
            
            drawMesh( mDeferred.sphereMesh );
        }
    }
    
    
    glCullFace( GL_BACK );
    /*draw lights here */
    
    { // ambient
        bindUniforms( 1, mAmbientUniforms.getBuffer(), mAmbientUniforms.getOffset(), mAmbientUniforms.getSize() );
        mDeferred.ambientLightProgram->bindProgram();
        mGBuffer.diffuseTexture->bindTexture( 0 );
        mSSAO.ssaoBlured->bindTexture( 1 );
        
        glDrawArrays( GL_POINTS, 0, 1 );
    }
    
    glDepthMask( GL_TRUE );
    glEnable( GL_DEPTH_TEST );
}

void Renderer::renderCustom()
{
    for( const CustomRenderable &entry : mCustomRenderable ) 
    {
        entry.program->bindProgram();
        if( entry.indexbuffer ) {
            entry.indexbuffer->bindBuffer();
        }
        if( entry.vao ) {
            entry.vao->bindVAO();
        }
        for( int i=0; i < 8; ++i ) {
            if( entry.textures[i] ) {
                entry.textures[i]->bindTexture( i );
            }
        }
        for( int i=0; i < 8; ++i ) {
            if( entry.uniforms[i].buffer != 0 ) {
                bindUniforms( i, entry.uniforms[i].buffer, entry.uniforms[i].offset, entry.uniforms[i].size );
            }
        }
        
        setBlendMode( entry.blendMode );
        entry.renderable->render( *this );
    }
}

void Renderer::drawMesh( const SharedPtr<Mesh> &mesh )
{
    GpuBuffer *indexBuffer = mesh->getIndexBuffer().get();
    VertexArrayObject *vao = mesh->getVertexArrayObject().get();
    vao->bindVAO();
    
    if( indexBuffer ) {
        indexBuffer->bindBuffer();
    }
    
    for( const SubMesh &submesh : mesh->getSubMeshes() )
    {
        if( indexBuffer ) {
            glDrawElements( GL_TRIANGLES, submesh.vertexCount, GL_UNSIGNED_INT, reinterpret_cast<GLvoid*>(sizeof(GLuint)* submesh.vertexStart) );;
        }
        else {
            glDrawArrays( GL_TRIANGLES, submesh.vertexStart, submesh.vertexCount );
        }
    }
    
}

void Renderer::setBlendMode( BlendMode mode )
{
    switch( mode ) {
    case( BlendMode::Replace ):
        glBlendFunc( GL_ONE, GL_ZERO );
        break;
    case( BlendMode::AlphaBlend ):
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        break;
    case( BlendMode::AddjectiveBlend ):
        glBlendFunc( GL_ONE, GL_ONE );
        break;
    }
}

void Renderer::bindUniforms( GLuint index, GLuint buffer, GLuint offset, GLuint size )
{
    glBindBufferRange( GL_UNIFORM_BUFFER, index, buffer, offset, size );
}

void Renderer::setViewportSize( glm::uvec2 size )
{
    glViewport( 0, 0, size.x, size.y );
}



