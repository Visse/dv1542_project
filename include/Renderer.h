#pragma once

#include "SharedPtr.h"
#include "UniformBuffer.h"
#include "GLTypes.h"
#include "SharedEnums.h"
#include "Material.h"

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <vector>

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
};

class Renderer {
public:
    Renderer( Root *root );
    ~Renderer();
    
    void renderScene( Scene *scene, Camera *camera );
    
    void addMesh( const SharedPtr<Mesh> &mesh, const DeferredMaterial &material, const glm::mat4 &modelMatrix );
    void addPointLight( UniformBuffer uniforms, const glm::vec3 &pos, float radius, bool shadows );
    
    void render();
    
    
    void addCustomRenderable( const CustomRenderableSettings &settings, Renderable *renderable );
    
    
    UniformBuffer aquireUniformBuffer( size_t size );
    
    template< typename Type >
    UniformBuffer aquireUniformBuffer( const Type &content ) {
        UniformBuffer buffer = aquireUniformBuffer( sizeof(Type) );
        buffer.setRawContent( 0, &content, sizeof(Type) );
        return buffer;
    }
private:
    struct EntityInfo {
        SharedPtr<Mesh> mesh;
        GLuint buffer, offset;
        
        DeferredMaterial material;
    };
    
private:
    void initGBuffer();
    void initSSAO();
    void initDeferred();
    
    void renderDeferred();
    void renderSSAO();
    void renderLights();
    void renderCustom();
    
    void drawMesh( const SharedPtr<Mesh> &mesh );
    void setBlendMode( BlendMode mode );
    void bindUniforms( GLuint index, GLuint buffer, GLuint offset, GLuint size );
    void setViewportSize( glm::uvec2 size );
    
private:
    struct CustomRenderable {
        SharedPtr<GpuProgram> program;
        SharedPtr<Texture> textures[8];
        SharedPtr<VertexArrayObject> vao;
        SharedPtr<GpuBuffer> indexbuffer;
        
        struct {
            GLuint buffer = 0, offset, size;
        } uniforms[8];
        
        BlendMode blendMode;
        
        Renderable *renderable;
    };
    
    struct PointLightInfo {
        UniformBuffer uniforms;
        
        glm::vec3 pos;
        float radius;
    };
    
private:
    Root *mRoot;
    UniformBufferAllocator *mAllocator;
    Scene *mCurrentScene;
    
    std::vector<SceneObject*> mVisibleObjects;
    std::vector<EntityInfo> mEntities;
    std::vector<CustomRenderable> mCustomRenderable;
    
    std::vector<PointLightInfo> mPointLights,
                                mPointLightsNoShadow;
    
    
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
    
    glm::uvec2 mWindowSize;
};