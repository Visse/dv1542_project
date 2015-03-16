#pragma once

#include "SharedPtr.h"
#include "UniformBuffer.h"
#include "GLTypes.h"
#include "SharedEnums.h"
#include "Material.h"
#include "Frustrum.h"

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <vector>

class Frustrum;
class Renderable;
class GpuBuffer;
class VertexArrayObject;
class Root;
class Texture;
class FrameBuffer;
class SceneObject;
class Camera;
class Mesh;
class Material;
class Scene;
class UniformBufferAllocator;
class GpuProgram;


struct CustomRenderableSettings {
    SharedPtr<GpuProgram> program;
    SharedPtr<Texture> textures[8];
    SharedPtr<VertexArrayObject> vao;
    SharedPtr<GpuBuffer> indexbuffer;
    UniformBuffer uniforms[8];
    
    BlendMode blendMode = BlendMode::Replace;
    
    Renderable *renderable = nullptr;
    
    int queue = 0;
    
    
    friend bool operator < ( const CustomRenderableSettings &s1, const CustomRenderableSettings &s2 ) {
        return s1.queue < s2.queue;
    }
};

class Renderer {
private:
    enum SpecialBuffers {
        SB_SpecailBuffer_Bit = (1<<10),
        SB_SceneUniforms = 1 | SB_SpecailBuffer_Bit
    };
    
public:
    Renderer( Root *root );
    ~Renderer();
    
    void renderScene( Scene *scene, Camera *camera );
    
    void addMesh( const SharedPtr<Mesh> &mesh, const DeferredMaterial &material, const glm::mat4 &modelMatrix );
    void addPointLight( UniformBuffer uniforms, const glm::mat4 &modelMatrix, const glm::vec3 &position, float radius, bool shadows );
    void addCustomRenderable( const CustomRenderableSettings &settings );
    
    void addShadowMesh( const SharedPtr<Mesh> &mesh, const glm::mat4 &modelMatrix );
    
    void render();
    
    UniformBuffer aquireUniformBuffer( size_t size );
    
    UniformBuffer getSceneUniforms() {
        return UniformBuffer(SB_SceneUniforms,nullptr,0,0);
    }
    
    template< typename Type >
    UniformBuffer aquireUniformBuffer( const Type &content ) {
        UniformBuffer buffer = aquireUniformBuffer( sizeof(Type) );
        buffer.setRawContent( 0, &content, sizeof(Type) );
        return buffer;
    }
    
    void setRenderWireFrame( bool renderWireframe ) {
        mRenderWireframe = renderWireframe;
    }
    
private:
    void initGBuffer();
    void initSSAO();
    void initDeferred();
    void initShadows();
    void initOther();
    
    void renderDeferred();
    void renderSSAO();
    void renderLights();
    void renderOther();
    void renderCustom();
    
    void drawMesh( const SharedPtr<Mesh> &mesh );
    void setBlendMode( BlendMode mode );
    void bindUniforms( GLuint index, const UniformBuffer &buffer ) {
        bindUniforms( index, buffer.getBuffer(), buffer.getOffset(), buffer.getSize() );
    }
    void bindUniforms( GLuint index, GLuint buffer, GLuint offset, GLuint size );
    void setViewportSize( glm::uvec2 size );
    
    void prepereShadowCasters();
    void renderPointLightShadowMap( unsigned int first, unsigned int last );
    
    void quaryForObjects( const Frustrum &frustrum );
    
private:
    struct EntityInfo {
        SharedPtr<Mesh> mesh;
        GLuint buffer, offset;
        
        DeferredMaterial material;
    };
    struct ShadowMeshInfo {
        SharedPtr<Mesh> mesh;
        UniformBuffer buffer;
    };
    
    struct PointLightInfo {
        UniformBuffer uniforms, shadowUniform;
        unsigned int firstShadowCaster, lastShadowCaster;
        
        glm::mat4 viewProjMatrix;
    };
    struct PointLightNoShadowInfo {
        UniformBuffer uniforms;
    };
    
private:
    Root *mRoot;
    UniformBufferAllocator *mAllocator;
    Scene *mCurrentScene = nullptr;
    Camera *mCurrentCamera = nullptr;
    
    std::vector<SceneObject*> mQuaryResult;
    std::vector<EntityInfo> mEntities;
    std::vector<CustomRenderableSettings> mCustomRenderable;
    
    std::vector<PointLightInfo> mPointLights;
    std::vector<PointLightNoShadowInfo> mPointLightsNoShadow;
    
    std::vector<ShadowMeshInfo> mShadowMeshes;
    
    UniformBuffer mSceneUniforms, 
                  mAmbientUniforms;
    
    struct { // GBuffer data
        SharedPtr<FrameBuffer> framebuffer;
        
        SharedPtr<Texture> diffuseTexture,
                           normalTexture,
                           depthTexture;
        
        glm::uvec2 frameBufferSize;
    } mGBuffer;
    struct { // SSAO data
        SharedPtr<GpuProgram> scenePassProgram,
                              ssaoProgram,
                              ssaoBlur;
                              
        SharedPtr<FrameBuffer> framebuffer,
                               ssaoFrameBuffer;
        
        SharedPtr<Texture> normalTexture,
                           depthTexture,
                           randTexture,
                           ssaoTexture,
                           ssaoBlured;
                           
        
                           
        glm::uvec2 frameBufferSize;
    } mSSAO;
    
    struct { // Deferred data
        SharedPtr<GpuProgram> entityDeferredProgram,
                              pointLightProgram,
                              pointLightNoShadowProgram,
                              ambientLightProgram,
                              copyDepthProgram;
                              
        SharedPtr<Mesh> sphereMesh;
    } mDeferred;
    
    struct {
        SharedPtr<GpuProgram> pointLightShadowCasterProgram;
        SharedPtr<Texture> pointLightShadowTexture;
        
        SharedPtr<FrameBuffer> pointLightShadowFrameBuffer;
        
        glm::uvec2 frameBufferSize;
    } mShadows;
    
    struct {
        SharedPtr<GpuProgram> skyboxProgram;
        
        SharedPtr<Mesh> cubeMesh;
    } mOther;
    
    glm::uvec2 mWindowSize;
    
    bool mRenderWireframe = false;
};