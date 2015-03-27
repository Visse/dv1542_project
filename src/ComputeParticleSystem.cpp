#include "ComputeParticleSystem.h"
#include "Root.h"
#include "ResourceManager.h"
#include "GpuProgram.h"
#include "GLinclude.h"
#include "Config.h"
#include "GpuBuffer.h"
#include "Material.h"
#include "GraphicsManager.h"
#include "Renderer.h"
#include "Renderable.h"

#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>

class ComputeParticleSystem::ParticleRenderable :
    public Renderable
{
public:
    ComputeParticleSystem *system;
    void (ComputeParticleSystem::*renderFunc)();
    
    virtual void render( Renderer &renderer ) override {
        (system->*renderFunc)();
    }
};


ComputeParticleSystem::ComputeParticleSystem( SceneObjectFactory *factory, Root *root ) :
    SceneObject(factory),
    mRoot(root)
{
    mRenderer = mRoot->getGraphicsManager()->getRenderer();
    
    mParticleRenderable = new ParticleRenderable;
        mParticleRenderable->system = this;
        mParticleRenderable->renderFunc = &ComputeParticleSystem::render;
        
    mAttractorRenderable = new ParticleRenderable;
        mAttractorRenderable->system = this;
        mAttractorRenderable->renderFunc = &ComputeParticleSystem::renderAttractors;
    
    ResourceManager *resourceMgr = mRoot->getResourceManager();
    
    mSimulation = resourceMgr->getGpuProgramAutoPack( "ComputeParticleSimulation" );
    mShader = resourceMgr->getGpuProgramAutoPack( "ComputeParticleShader" );
    mAttractorShader = resourceMgr->getGpuProgramAutoPack( "ComputeAttractorShader" );
    
    {
        GLuint glProgram = mSimulation->getGLProgram();
        
        mSimulationLoc.dt = glGetUniformLocation( glProgram, "dt" );
        mSimulationLoc.weightMod = glGetUniformLocation( glProgram, "weightMod" );
        mSimulationLoc.distMod = glGetUniformLocation( glProgram, "distMod" );
        mSimulationLoc.lifeTime = glGetUniformLocation( glProgram, "lifeTimeMod" );
        mSimulationLoc.damping = glGetUniformLocation( glProgram, "dampingMod" );
        mSimulationLoc.attractorCount = glGetUniformLocation( glProgram, "AttractorCount" );
        mSimulationLoc.modelMatrix = glGetUniformLocation( glProgram, "ModelMatrix");
    }
    
    const Config *config = mRoot->getConfig();
    
    struct Particle {
        glm::vec4 pos, vel;
    };
    
    size_t particleCount = config->computeParticle.localSize * config->computeParticle.maxGroupCount;
    mParticleBuffer = GpuBuffer::CreateBuffer( BufferType::ShaderStorage, sizeof(Particle)*particleCount, BufferUsage::ReadWrite, BufferUpdate::Dynamic );
    mStartPositionBuffer = GpuBuffer::CreateBuffer( BufferType::ShaderStorage, sizeof(glm::vec3)*particleCount, BufferUsage::ReadWrite, BufferUpdate::Static );
    mAttractorBuffer = GpuBuffer::CreateBuffer( BufferType::ShaderStorage, sizeof(glm::vec4)*config->computeParticle.maxAttractorCount, BufferUsage::ReadWrite, BufferUpdate::Dynamic );
    
    Particle *particles = mParticleBuffer->mapBuffer<Particle>( BufferUsage::WriteOnly );
    std::generate( particles, particles + particleCount, 
        [] {
            return Particle{ 
                glm::vec4( glm::ballRand(1.f), glm::linearRand(0.f, 1.f) ),
                glm::vec4(glm::ballRand(0.1f),0.f)
            };
        }
    );
    mParticleBuffer->unmapBuffer();
    
    glm::vec3 *startPositins = mStartPositionBuffer->mapBuffer<glm::vec3>( BufferUsage::WriteOnly  );
    std::generate( startPositins, startPositins+particleCount,
        []{
            return glm::sphericalRand(1.f);
        }
    );
    mStartPositionBuffer->unmapBuffer();
    
    
    mAttractorWeights.resize( config->computeParticle.maxAttractorCount );
    std::generate( mAttractorWeights.begin(), mAttractorWeights.end(), 
        [] {
            return glm::linearRand(4.f,10.f);
        }
    );
    
    mVAO = makeSharedPtr<VertexArrayObject>();
    mVAO->bindVAO();
    mParticleBuffer->bindBufferAs( BufferType::Vertexes );
    mVAO->setVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), offsetof(Particle,pos) );
    
    mVAO->unbindVAO();
    mParticleBuffer->unbindBufferAs( BufferType::Vertexes );
    
    mAttractorVAO = makeSharedPtr<VertexArrayObject>();
    mAttractorVAO->bindVAO();
    mAttractorBuffer->bindBufferAs( BufferType::Vertexes );
    mAttractorVAO->setVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), 0 );
    
    mAttractorVAO->unbindVAO();
    mAttractorBuffer->unbindBufferAs( BufferType::Vertexes );
    
    mAttractorCount = config->computeParticle.defaultAttractorCount;
    mParticleGroupCount = config->computeParticle.defaultGroupCount;
    mParticleGroupSize = config->computeParticle.localSize;
    mMaxParticleGroupCount = config->computeParticle.maxGroupCount;
    mMaxAttractorCount  = config->computeParticle.maxAttractorCount;
}

ComputeParticleSystem::~ComputeParticleSystem()
{
    delete mParticleRenderable;
    delete mAttractorRenderable;
}

void ComputeParticleSystem::update( float dt )
{
    mCurrentTime += dt * mSpeed;
    // to keep our precision up
    mCurrentTime = glm::mod( mCurrentTime, 64.f );
    
    glm::mat4 transform = getTransform();
    glm::vec4 *attractors = mAttractorBuffer->mapBuffer<glm::vec4>( BufferUsage::WriteOnly );
    for (unsigned int i = 0; i < mAttractorCount; ++i) {
        attractors[i] = glm::vec4( 
            glm::sin( mCurrentTime*(i+2)*0.06f+i*0.2f)*glm::cos( mCurrentTime*(i+7)*0.04f ),
            glm::sin( mCurrentTime*(i+3)*0.04f+i*0.5f)*glm::sin( mCurrentTime*(i+5)*0.02f ),
            glm::cos( mCurrentTime*(i+5)*0.02f+i*0.8f)*glm::cos( mCurrentTime*(i+9)*0.06f ),
            mAttractorWeights[i]
        );
    }
    mAttractorBuffer->unmapBuffer();
    
    mSimulation->bindProgram();
    
    mParticleBuffer->bindIndexed( 0 );
    mStartPositionBuffer->bindIndexed( 1 );
    mAttractorBuffer->bindIndexed( 2 );
    
    glUniform1f( mSimulationLoc.dt, dt * mSpeed );
    glUniform1f( mSimulationLoc.distMod, mDistMod );
    glUniform1f( mSimulationLoc.weightMod, mWeightMod );
    glUniform1f( mSimulationLoc.lifeTime, 1.f/mLifeTime );
    glUniform1f( mSimulationLoc.damping, mDamping );
    glUniformMatrix4fv( mSimulationLoc.modelMatrix, 1, GL_FALSE, glm::value_ptr(transform) );
    glUniform1ui( mSimulationLoc.attractorCount, mAttractorCount );
    
    glDispatchCompute( mParticleGroupCount, 1, 1 );
    
    glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );
    
}

void ComputeParticleSystem::submitRenderer( Renderer &renderer )
{
    RenderingUniformBlock uniforms;
        uniforms.modelMatrix = getTransform();
        uniforms.intensityAndSize = glm::vec2( mIntensity, mPointSize );
    
    CustomRenderableSettings settings;
        settings.blendMode = BlendMode::AddjectiveBlend;
        settings.vao = mVAO;
        settings.uniforms[0] = mRenderer->getSceneUniforms();
        settings.uniforms[1] = mRenderer->aquireUniformBuffer( uniforms );
        settings.program = mShader;
        settings.renderable = mParticleRenderable;
    mRenderer->addCustomRenderable( settings );
    
    if( mShowAttractors ) {
        settings.program = mAttractorShader;
        settings.renderable = mAttractorRenderable;
        settings.vao = mAttractorVAO;
        
        mRenderer->addCustomRenderable( settings );
    }
}


void ComputeParticleSystem::render()
{
    glDepthMask( GL_FALSE );
    glDrawArrays( GL_POINTS, 0, mParticleGroupSize * mParticleGroupCount );
    glDepthMask( GL_TRUE );
}

void ComputeParticleSystem::renderAttractors()
{
    glDepthMask( GL_FALSE );
    glDrawArrays( GL_POINTS, 0, mAttractorCount );
    glDepthMask( GL_TRUE );
}
