#include "DebugManager.h"
#include "GraphicsManager.h"
#include "FrameListener.h"
#include "Texture.h"
#include "Root.h"
#include "GpuBuffer.h"
#include "VertexArrayObject.h"
#include "GLinclude.h"
#include "GpuProgram.h"
#include "GpuShader.h"
#include "Config.h"
#include "ImGuiHelpers.h"
#include "StartupMesurements.h"
#include "Timer.h"
#include "Material.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Entity.h"
#include "Mesh.h"
#include "ComputeParticleSystem.h"
#include "LightObject.h"
#include "DebugDrawer.h"
#include "Camera.h"
#include "LowLevelRenderer.h"
#include "SceneGraph.h"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mouse.h>

#include <imgui.h>

#include <functional>
#include <iostream>

#ifdef __GNUC__

#include <cxxabi.h>

std::string demangleName( const char *name )
{
    char *demangledName = __cxxabiv1::__cxa_demangle( name, NULL, NULL, NULL );
    std::string tmp(demangledName);
    
    free(demangledName);
    
    return tmp;
}

#else

std::string demangleName( const char *name )
{
    return std::string(name);
}

#endif

static const glm::vec4 COLOR_WIREFRAME = glm::vec4(0.1f,1.f,0.5f,1.f),
                       COLOR_NORMAL = glm::vec4(0.5f,0.2f,0.2f,1.f),
                       COLOR_TANGENT = glm::vec4(0.2f,0.5f,0.2f,1.f),
                       COLOR_BITANGENT = glm::vec4(0.2f,0.2f,0.5f,1.f),
                       COLOR_BOUNDS = glm::vec4(0.5f,0.2f,0.2f,1.f),
                       COLOR_INNER_LIGHT_VOLUME = glm::vec4(0.2,0.2f,0.5f,1.f),
                       COLOR_OUTER_LIGHT_VOLUME = glm::vec4(0.2,0.5f,0.2f,1.f),
                       COLOR_PARENT_SCENENODES =  glm::vec4(0.5f,0.5f,0.2f,1.f),
                       COLOR_SAVED_FRUSTRUM =  glm::vec4(0.8f,0.3f,0.8f,1.f),
                       COLOR_FRUSTUM_TEST_OUTSIDE = glm::vec4(0.8f,0.3f,0.3f,1.f),
                       COLOR_FRUSTUM_TEST_INTERSECTING = glm::vec4(0.4f,0.2f,0.8f,1.f),
                       COLOR_FRUSTUM_TEST_INSIDE = glm::vec4(0.3f,0.8f,0.5f,1.f);

static const float NORMAL_LENGHT = 0.1f;
                       

class DebugManager::DebugCamera :
    public Camera
{
public:
    DebugManager *debugMgr;
    virtual void render( LowLevelRenderer &renderer ) {
        debugMgr->render( renderer );
    }
};

bool DebugManager::init( Root *root )
{
    Timer initTimer;
    
    mRoot = root;
    
    const Config *config = mRoot->getConfig();
    mKeyToogleDebug = config->keyBindings.toogleDebug;
    
    
    ResourceManager *resourceMgr = mRoot->getResourceManager();
    mMaterial = resourceMgr->getMaterialAutoPack("DebugGuiMaterial");
    
    if( !mMaterial ) {
        std::cerr << "Failed to load debug gui material!" << std::endl;
        return false;
    }
    
    mHeight = config->windowHeight;
    
    SharedPtr<GpuProgram> program = mMaterial->getProgram();
    
    GLint posLocation = program->getAttribLocation("Position"),
          uvLocation  = program->getAttribLocation("UV"),
          colourLocation = program->getAttribLocation("Colour");
          
    mUniformBlockLoc = program->getUniformBlockLocation("Debug");
    
    float width = config->windowWidth,
          height = config->windowHeight;
    
    mUniforms.projectionMatrix = glm::mat4(
         2.0f/width, 0.0f,         0.0f,   0.0f,
         0.0f,       2.0f/-height, 0.0f,   0.0f,
         0.0f,       0.0f,        -1.0f,   0.0f,
        -1.0f,       1.0f,         0.0f,   1.0f
    );
    
    mVertexBuffer = GpuBuffer::CreateBuffer( BufferType::Vertexes, 20000, BufferUsage::WriteOnly, BufferUpdate::Dynamic );
    
    mVAO = makeSharedPtr<VertexArrayObject>();
    mVAO->bindVAO();
    mVertexBuffer->bindBuffer();
    
    mVAO->setVertexAttribPointer( posLocation, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), offsetof(ImDrawVert, pos) );
    mVAO->setVertexAttribPointer( uvLocation, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), offsetof(ImDrawVert, uv) );
    mVAO->setVertexAttribPointer( colourLocation, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), offsetof(ImDrawVert, col)) ;
    
    mVAO->unbindVAO();
    mVertexBuffer->unbindBuffer();
    
    initImGui();
    
    mCamera = new DebugCamera;
    mCamera->debugMgr = this;
    
    GraphicsManager *graphicsMgr = mRoot->getGraphicsManager();
    graphicsMgr->addCamera( mCamera );
    
    mGBufferNormalMaterial = resourceMgr->getMaterialAutoPack( "GBufferDebugNormalMaterial" );
    mGBufferDepthMaterial = resourceMgr->getMaterialAutoPack( "GBufferDebugDepthMaterial" );

    StartupMesurements *mesurements = mRoot->getStartupMesurements();
    mesurements->debugStartup = initTimer.getTimeAsSeconds();
    
    return true;
}

void DebugManager::postInit()
{    
    GraphicsManager *graphicsMgr = mRoot->getGraphicsManager();
    LowLevelRenderer &renderer = graphicsMgr->getLowLevelRenderer();
    if( mGBufferNormalMaterial ) {
        mGBufferNormalMaterial->setTexture( "NormalTexture", 0, renderer.getDeferredNormalTexture() );
    }
    if( mGBufferDepthMaterial ) {
        mGBufferDepthMaterial->setTexture( "DepthTexture", 0, renderer.getDeferredDepthTexture() );
    }
}

void DebugManager::destroy()
{
    destroyImGui();
    
    GraphicsManager *graphicsMgr = mRoot->getGraphicsManager();
    graphicsMgr->removeCamera( mCamera );
    
    delete mCamera;
    mCamera = nullptr;
    
    mRoot = nullptr;
}

void DebugManager::update( float dt )
{
    ImGui::NewFrame();
    if( mIsDebugVisible ) {
        ImGuiIO &io = ImGui::GetIO();
        io.DeltaTime = dt;
        
        if( ImGui::Begin("Debug") ) {
            if( ImGui::CollapsingHeader("Help") ) {
                ImGui::ShowUserGuide();
            }
            if( ImGui::CollapsingHeader("Startup Mesurements") ) {
                StartupMesurements *mesurements = mRoot->getStartupMesurements();
                ImGui::Value( "Total", mesurements->totalTime );
                ImGui::Value( "SDL", mesurements->sdlStartup);
                ImGui::Value( "Graphics", mesurements->graphicsStartup );
                ImGui::Value( "Debug", mesurements->debugStartup );
                ImGui::Value( "Resources", mesurements->resourceStartup );
                ImGui::Value( "Scene", mesurements->sceneStartup );
            }
            
            if( ImGui::CollapsingHeader("Mesurements") ) {
                GraphicsManager *graphicsMgr = mRoot->getGraphicsManager();
                
                ImGui::PlotLines( "Frame Time", mRoot->getFrameTimeHistory(), 0.1f, 0.1f, ImVec2(0,70) );
                ImGui::PlotLines( "Frame Rate", mRoot->getFrameRateHistory(), 10.f, 0.f, ImVec2(0,70) );
                ImGui::PlotLines( "Gpu Time", graphicsMgr->getGpuTimeHistory(), 10, 10, ImVec2(0,70) );
                ImGui::PlotLines( "Samples passed (*10k)", graphicsMgr->getSamplesPassed(), 100, 100, ImVec2(0,70) );
                
                LowLevelRenderer &renderer = graphicsMgr->getLowLevelRenderer();
                ImGui::PlotLines( "Draw Count", renderer.getDrawCountHistory(), 10, 10, ImVec2(0,70) );
                ImGui::PlotLines( "Vertex Count", renderer.getVertexCountHistory(), 10, 10, ImVec2(0,70) );
            }
            
            SceneManager *sceneMgr = mRoot->getSceneManager();
            Scene *scene = sceneMgr->getScene();
            
            if( scene ) {
                if( ImGui::CollapsingHeader("Scene") ) {
                    ImGui::Checkbox( "Show Bounds", &mShowSceneBounds );
                    
                    ImGui::Checkbox( "Show SceneGraph", &mShowSceneGraph );
                    ImGui::SameLine();
                    ImGui::Checkbox( "Color Test", &mShowBoundsInFrustrumColor );
                    
                    ImGui::Checkbox( "Nodes As Boxes", &mShowSceneNodesAsBoxes );
                    ImGui::SameLine();
                    ImGui::Checkbox( "Adjust Size", &mAdjustSceneNodesSize );
                    
                    ImGui::Checkbox( "Used Saved Frustrum", &mUseSavedFrustrum );
                    ImGui::SameLine();
                    if( ImGui::Button("Save Frustrum") ) {
                        Camera *camera = sceneMgr->getCamera();
                        if( camera ) {
                            mSavedViewProjMatrix = camera->getProjectionMatrix() * camera->getViewMatrix();
                            mSavedFrustrum = Frustrum::FromProjectionMatrix( mSavedViewProjMatrix );
                        }
                    }
                    ImGui::SameLine();
                    ImGui::Checkbox( "Show Saved Frustrum", &mShowSavedFrustrum );
                    
                    if( ImGui::TreeNode("Show GBuffer") ) {
                        if( ImGui::RadioButton("None", mGBufferDebug == GBufferDebug::None) ) {
                            mGBufferDebug = GBufferDebug::None;
                        }
                        if( ImGui::RadioButton("Normals", mGBufferDebug == GBufferDebug::Normal) ) {
                            mGBufferDebug = GBufferDebug::Normal;
                        }
                        if( ImGui::RadioButton("Depth", mGBufferDebug == GBufferDebug::Depth) ) {
                            mGBufferDebug = GBufferDebug::Depth;
                        }
                        ImGui::TreePop();
                    }
                }
                
                
                if( ImGui::CollapsingHeader("SceneObjects") ) {
                    scene->forEachObject(
                        std::bind( &DebugManager::showSceneObject, this, dt, std::placeholders::_1 )
                    );
                }
            }
        }
        ImGui::End();
    }
    
    if( mUseSavedFrustrum ) {
        mCurrentFrustrum = mSavedFrustrum;
    }
    else {
        SceneManager *sceneMgr = mRoot->getSceneManager();
        Camera *camera = sceneMgr->getCamera();
        if( camera ) {
            mCurrentFrustrum = camera->getFrustrum();
        }
    }
    
    submitDebugDraw();
}

bool DebugManager::handleSDLEvent( const SDL_Event &event )
{
    if( event.type == SDL_KEYDOWN &&
        event.key.keysym.sym == mKeyToogleDebug ) 
    {
        mIsDebugVisible = !mIsDebugVisible;
        return false;
    }
    if( !mIsDebugVisible || SDL_GetRelativeMouseMode() == SDL_TRUE ) {
        return false;
    }
    
    bool mouseEvent = false, keyEvent = false;
    
    ImGuiIO &io = ImGui::GetIO();
    switch( event.type ) {
    case( SDL_MOUSEBUTTONDOWN ):
    case( SDL_MOUSEBUTTONUP ):
        if( event.button.button == SDL_BUTTON_LEFT ) {
            io.MouseDown[0] = event.button.state == SDL_PRESSED;
        }
        mouseEvent = true;
        break;
    case( SDL_MOUSEMOTION ):
        io.MousePos.x = event.motion.x;
        io.MousePos.y = event.motion.y;
        mouseEvent = true;
        break;
    case( SDL_MOUSEWHEEL ):
        io.MouseWheel = event.wheel.y;
        mouseEvent = true;
        break;
    case( SDL_KEYDOWN ):
    case( SDL_KEYUP ): {
        bool keydown = (event.key.state == SDL_PRESSED);
        bool wantsEvent = true;
        switch( event.key.keysym.sym ) {
        case( SDLK_LCTRL ):
        case( SDLK_RCTRL ):
            io.KeyCtrl = keydown;
            break;
        case( SDLK_LSHIFT ):
        case( SDLK_RSHIFT ):
            io.KeyShift = keydown;
            break;
        case( SDLK_TAB ):
            io.KeysDown[ImGuiKey_Tab] = keydown;
            break;
        case( SDLK_LEFT ):
            io.KeysDown[ImGuiKey_LeftArrow] = keydown;
            break;
        case( SDLK_RIGHT ):
            io.KeysDown[ImGuiKey_RightArrow] = keydown;
            break;
        case( SDLK_UP ):
            io.KeysDown[ImGuiKey_UpArrow] = keydown;
            break;
        case( SDLK_DOWN ):
            io.KeysDown[ImGuiKey_DownArrow] = keydown;
            break;
        case( SDLK_HOME ):
            io.KeysDown[ImGuiKey_Home] = keydown;
            break;
        case( SDLK_END ):
            io.KeysDown[ImGuiKey_End] = keydown;
            break;
        case( SDLK_DELETE ):
            io.KeysDown[ImGuiKey_Delete] = keydown;
            break;
        case( SDLK_BACKSPACE ):
            io.KeysDown[ImGuiKey_Backspace] = keydown;
            break;
        case( SDLK_RETURN ):
        case( SDLK_KP_ENTER ):
            io.KeysDown[ImGuiKey_Enter] = keydown;
            break;
        case( SDLK_ESCAPE ):
            io.KeysDown[ImGuiKey_Escape] = keydown;
            break;
        case( SDLK_a ):
            io.KeysDown[ImGuiKey_A] = keydown;
            break;
        case( SDLK_c ):
            io.KeysDown[ImGuiKey_C] = keydown;
            break;
        case( SDLK_v ):
            io.KeysDown[ImGuiKey_V] = keydown;
            break;
        case( SDLK_x ):
            io.KeysDown[ImGuiKey_X] = keydown;
            break;
        case( SDLK_y ):
            io.KeysDown[ImGuiKey_Y] = keydown;
            break;
        case( SDLK_z ):
            io.KeysDown[ImGuiKey_Z] = keydown;
            break;
        default:
            wantsEvent = false;
            break;
        }
        mouseEvent = wantsEvent;
        keyEvent = true;
      } break;
    case( SDL_TEXTINPUT ):
        // @todo FIXME this is a simple hack to get text input working.
        for( int i=0; i < SDL_TEXTINPUTEVENT_TEXT_SIZE && event.text.text[i] != '\0'; ++i ) {
            io.AddInputCharacter( event.text.text[i] );
        }
        keyEvent = true;
        break;
    }
    if( mouseEvent && io.WantCaptureMouse ) {
        return true;
    }
    if( keyEvent && io.WantCaptureKeyboard ) {
        return true;
    }
    
    return false;
}

void DebugManager::paintDebugOverlay()
{
    if( mIsDebugVisible ) {
        ImGui::Render();
    }
}

struct DrawInfo {
    glm::vec2 scissorPos, scissorSize;
    size_t vertexStart, vertexCount;
};
    
struct ImGuiRenderData {
    DebugManager *debugMgr;
    SharedPtr<GpuBuffer> vertexBuffer;
    std::vector<DrawInfo> draws;
};


void DebugManager::render( LowLevelRenderer &renderer )
{
    if( mIsDebugVisible ) {
        
        ImGui::Render();
        
        QueueOperationParams params;
            params.material = mMaterial.get();
            params.vao = mVAO.get();
            params.drawMode = DrawMode::Triangles;
            params.renderQueue = RQ_Overlay;
            params.scissorTest = true;
            params.faceCulling = false;
            
            
        auto uniforms = renderer.aquireUniformBuffer( sizeof(UniformBlock) );
        uniforms.setIndex( mUniformBlockLoc );
        uniforms.setRawContent( 0, &mUniforms, sizeof(UniformBlock) );
        params.uniforms[0] = uniforms;
            
        ImGuiIO &io = ImGui::GetIO();
        ImGuiRenderData *renderData = static_cast<ImGuiRenderData*>(io.UserData);
        
        for( const DrawInfo &draw : renderData->draws ) {
            params.vertexStart = draw.vertexStart;
            params.vertexCount = draw.vertexCount;
            
            params.scissorPos = glm::vec2( draw.scissorPos.x, mHeight-draw.scissorPos.y );
            params.scissorSize = draw.scissorSize;
            
            renderer.queueOperation( params );
        }
    }
    
    switch( mGBufferDebug ) {
    case( GBufferDebug::None ):
        break;
    case( GBufferDebug::Normal ): {
        QueueOperationParams params;
            params.material = mGBufferNormalMaterial.get();
            params.drawMode = DrawMode::Points;
            params.renderQueue = RQ_OverlayFirst;
            params.vertexCount = 1;
            
        renderer.queueOperation( params );
      } break;
    case( GBufferDebug::Depth ): {
        QueueOperationParams params;
            params.material = mGBufferDepthMaterial.get();
            params.drawMode = DrawMode::Points;
            params.renderQueue = RQ_OverlayFirst;
            params.vertexCount = 1;
        
        renderer.queueOperation( params );
      } break;
    }
}

void RenderImGuiDrawLists( ImDrawList** const draw_lists, int count );

void DebugManager::initImGui()
{
    ImGuiIO &io = ImGui::GetIO();
    
    for( int i=0; i < ImGuiKey_COUNT; ++i ) {
        io.KeyMap[i] = i;
    }
    
    const Config *config = mRoot->getConfig();
    
    io.RenderDrawListsFn = RenderImGuiDrawLists;
    io.ImeSetInputScreenPosFn = []( int x, int y ) -> void {
        ImGuiIO &io = ImGui::GetIO();
        DebugManager *debugMgr = static_cast<ImGuiRenderData*>(io.UserData)->debugMgr;
        debugMgr->setInputScreenPos( x, y );
    };
    ImGuiRenderData *renderData = new ImGuiRenderData;
    renderData->debugMgr = this;
    renderData->vertexBuffer = mVertexBuffer;
    io.UserData = renderData;
    io.DisplaySize.x = config->windowWidth;
    io.DisplaySize.y = config->windowHeight;

    unsigned char *pixels;
    int texWidth, texHeight;
    io.Fonts->GetTexDataAsRGBA32( &pixels, &texWidth, &texHeight );
    
    SharedPtr<Texture> texture = Texture::LoadTextureFromRawMemory( TextureType::RGBA, pixels, texWidth, texHeight );

    texture->bindTexture(0);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    texture->unbindTexture(0);
    
    mMaterial->setTexture( "Texture", 0, texture );
    
    ImGui::NewFrame();
}

void DebugManager::destroyImGui()
{
    ImGuiIO &io = ImGui::GetIO();
    delete static_cast<ImGuiRenderData*>(io.UserData);
    ImGui::Shutdown();
}

void DebugManager::setInputScreenPos( int x, int y )
{
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    SDL_SetTextInputRect( &rect );
}

void DebugManager::showSceneObject( float dt, SceneObject *object )
{
    ImGui::PushID( object );
    
    DebugDrawInfo &debugDrawInfo = mDebugDrawInfo[object];
    
    std::string objectType = demangleName(typeid(*object).name());
    
    if( ImGui::TreeNode(objectType.c_str()) ) {
        glm::vec3 position = object->getPosition();
        
        if( ImGui::InputFloat3("Position", glm::value_ptr(position)) ) {
            object->setPosition( position );
        }
        
        glm::vec3 deltaPos;
        
        if( ImGui::SliderFloat3("Move", glm::value_ptr(deltaPos),-1.f, 1.f) ) {
            position += deltaPos * dt;
            object->setPosition( position );
        }
        
        glm::quat orientation = object->getOrientation();
        glm::vec3 rotation;
        glm::vec3 eulerRotation = glm::degrees(glm::eulerAngles(orientation));
        
        if( ImGui::InputFloat3("Orientation", glm::value_ptr(eulerRotation)) ) {
            object->setOrientation( glm::quat(glm::radians(eulerRotation)) );
        }
        if( ImGui::SliderFloat3("Rotate", glm::value_ptr(rotation), -glm::half_pi<float>(), glm::half_pi<float>() ) ) {
            orientation *= glm::quat(rotation*dt);
            object->setOrientation( orientation );
        }
        ImGui::Checkbox( "Show BoundingSphere", &debugDrawInfo.bounds );
        ImGui::SameLine();
        ImGui::Checkbox( "Show Parent Nodes", &debugDrawInfo.parentSceneNodes );
        
        Entity *entity = dynamic_cast<Entity*>(object);
        if( entity ) {
            SharedPtr<Mesh> mesh = entity->getMesh();
            const std::string &meshName = mesh->getName();
            
            debugDrawInfo.mesh = mesh;
            
            if( !meshName.empty() ) {
                ImGui::LabelText( "Mesh", meshName.c_str() );
            }
            else {
                ImGui::LabelText( "Mesh", "Unknown" );
            }
            
            ImGui::Checkbox( "WireFrame", &debugDrawInfo.wireFrame );
            ImGui::SameLine();
            ImGui::Checkbox( "Normals", &debugDrawInfo.normals );
        }
        
        ComputeParticleSystem *particleSys = dynamic_cast<ComputeParticleSystem*>(object);
        if( particleSys ) {
            float speed = particleSys->getSpeed();
            if( ImGui::SliderFloat("Speed", &speed, 0.f, 4.f) ) {
                particleSys->setSpeed( speed );
            }
            
            float distMod = particleSys->getDistMod();
            if( ImGui::SliderFloat("DistMod", &distMod, 0.f, 10.f) ) {
                particleSys->setDistMod( distMod );
            }
            
            float weightMod = particleSys->getWeightMod();
            if( ImGui::SliderFloat("WeightMod", &weightMod, 0.f, 20.f) ) {
                particleSys->setWeightMod( weightMod );
            }
            
            float lifeTime = particleSys->getLifeTime();
            if( ImGui::SliderFloat("LifeTime", &lifeTime, 0.f, 30.f) ) {
                particleSys->setLifeTime( lifeTime );
            }
            
            float damping = particleSys->getDamping();
            if( ImGui::SliderFloat("Damping", &damping, 0.0f, 2.0f) ) {
                particleSys->setDamping( damping );
            }
            
            float intensity = particleSys->getIntensity();
            if( ImGui::SliderFloat("Intensity", &intensity, 0.f, 5.f) ) {
                particleSys->setIntensity( intensity );
            }
            
            float pointSize = particleSys->getPointSize();
            if( ImGui::SliderFloat("PointSize", &pointSize, 0.f, 15.f) ) {
                particleSys->setPointSize( pointSize );
            }
            
            int groupCount = particleSys->getParticleGroupCount();
            int maxGroups = particleSys->getMaxParticleGroupCount();
            if( ImGui::SliderInt("Amount", &groupCount, 1, maxGroups) ) {
                particleSys->setParticleGroupCount( groupCount );
            }
            
            int attractorCount = particleSys->getAttractorCount();
            int maxAttractors = particleSys->getMaxAttractorCount();
            if( ImGui::SliderInt("Attractors", &attractorCount, 0, maxAttractors ) ) {
                particleSys->setAttractorCount( attractorCount );
            }
            
            bool showAttractors = particleSys->getShowAttractors();
            if( ImGui::Checkbox( "Show Attractors", &showAttractors) ) {
                particleSys->setShowAttractors( showAttractors );
            }
            
            unsigned int particleCount = particleSys->getParticleCount();
            ImGui::Value( "Particle Count", particleCount );
        }
        
        LightObject *light = dynamic_cast<LightObject*>(object);
        if( light ) {
            glm::vec3 color = light->getColor();
            if( ImGui::ColorEdit3("Color", glm::value_ptr(color)) ) {
                light->setColor( color );
            }
            
            if( PointLight *pointLight = dynamic_cast<PointLight*>(light) ) {
                float outerRadius = pointLight->getOuterRadius(),
                      innerRadius = pointLight->getInnerRadius(),
                      intesity = pointLight->getIntensity();
                    
                if( ImGui::SliderFloat("OuterRadius", &outerRadius, 0.f, 20.f) ) {
                    pointLight->setOuterRadius( outerRadius );
                }
                if( ImGui::SliderFloat("InnerRadius", &innerRadius, 0.f, outerRadius) ) {
                    pointLight->setInnerRadius( innerRadius );
                }
                if( ImGui::SliderFloat("Intensity", &intesity, 0.f, 2.f) ) {
                    pointLight->setIntensity( intesity );
                }
            }
            
            if( SpotLight *spotLight = dynamic_cast<SpotLight*>(light) ) {
                float outerAngle = spotLight->getOuterAngle(),
                      innerAngle = spotLight->getInnerAngle(),
                      outerDist  = spotLight->getOuterDistance(),
                      innerDist  = spotLight->getInnerDistance(),
                      intensity = spotLight->getIntensity();
                      
                if( ImGui::SliderAngle("OuterAngle", &outerAngle, 0.f, 90.f) ) {
                    spotLight->setOuterAngle( outerAngle );
                }
                if( ImGui::SliderAngle("InnerAngle", &innerAngle, 0.f, glm::degrees(outerAngle)) ) {
                    spotLight->setInnerAngle( innerAngle );
                }
                if( ImGui::SliderFloat("OuterDistance", &outerDist, 0.f, 20.f) ) {
                    spotLight->setOuterDistance( outerDist );
                }
                if( ImGui::SliderFloat("InnerDistance", &innerDist, 0.f, outerDist) ) {
                    spotLight->setInnerDistance( innerDist );
                }
                if( ImGui::SliderFloat("Intensity", &intensity, 0.f, 2.f) ) {
                    spotLight->setIntensity( intensity );
                }
            }
        
            if( BoxLight *boxLight = dynamic_cast<BoxLight*>(light) ) {
                glm::vec3 innerSize = boxLight->getInnerSize(),
                          outerSize = boxLight->getOuterSize();
                float intensity = boxLight->getIntensity();
                
                if( ImGui::SliderFloat("OuterSizeX", &outerSize.x, 0.f, 20.f) ) {
                    boxLight->setOuterSize( outerSize );
                }              
                if( ImGui::SliderFloat("InnerSizeX", &innerSize.x, 0.f, outerSize.x) ) {
                    boxLight->setInnerSize( innerSize );
                }
                if( ImGui::SliderFloat("OuterSizeY", &outerSize.y, 0.f, 20.f) ) {
                    boxLight->setOuterSize( outerSize );
                }              
                if( ImGui::SliderFloat("InnerSizeY", &innerSize.y, 0.f, outerSize.y) ) {
                    boxLight->setInnerSize( innerSize );
                }
                if( ImGui::SliderFloat("OuterSizeZ", &outerSize.z, 0.f, 20.f) ) {
                    boxLight->setOuterSize( outerSize );
                }              
                if( ImGui::SliderFloat("InnerSizeZ", &innerSize.z, 0.f, outerSize.z) ) {
                    boxLight->setInnerSize( innerSize );
                }
                if( ImGui::SliderFloat("Intensity", &intensity, 0.f, 2.f) ) {
                    boxLight->setIntensity( intensity );
                }
            }
        
            ImGui::Checkbox( "Light Volume", &debugDrawInfo.debugLight );
        }
        ImGui::TreePop();
    }

    ImGui::PopID();
}

void DebugManager::submitDebugDraw()
{
    DebugDrawer *debugDrawer = mRoot->getDebugDrawer();
    
    for( const auto &entry : mDebugDrawInfo ) {
        const DebugDrawInfo &info = entry.second;
        SceneObject *object = entry.first;
        
        if( info.mesh && info.wireFrame ) {
            debugDrawer->drawWireFrame( info.mesh, object->getTransform(), COLOR_WIREFRAME );
        }
        if( info.mesh && info.normals ) {
            debugDrawer->drawVertexNormals( info.mesh, object->getTransform(), NORMAL_LENGHT, COLOR_NORMAL, COLOR_TANGENT, COLOR_BITANGENT );
        }
        if( info.bounds ) {
            showObjectBounds( object, COLOR_BOUNDS );
        }
        if( info.debugLight ) {
            LightObject *light = dynamic_cast<LightObject*>(object);
            if( light ) {
                debugDrawer->drawWireFrame( light->getInnerLightVolumeMesh(), light->getTransform()*light->getInnerLightVolumeMatrix(), COLOR_INNER_LIGHT_VOLUME );
                debugDrawer->drawWireFrame( light->getOuterLightVolumeMesh(), light->getTransform()*light->getOuterLightVolumeMatrix(), COLOR_OUTER_LIGHT_VOLUME );
            }
        }
        if( info.parentSceneNodes ) {
            SceneNode *node = object->_getParent();
            while( node ) {
                showSceneNode( node, COLOR_PARENT_SCENENODES );
                node  = node->getParent();
            }
        }
    }

    if( mShowSceneBounds ) {
        SceneManager *sceneMgr = mRoot->getSceneManager();
        Scene *scene = sceneMgr->getScene();
        
        if( scene ) {
            int index = 0;
            scene->forEachObject(
                [&]( SceneObject *object ) {
                    glm::vec4 color( glm::fract(index*0.13f+0.19f), 0.8f, 0.8f, 0.5f );
                    ImGui::ColorConvertHSVtoRGB( color.r, color.g, color.b, color.r, color.g, color.b );
                    
                    showObjectBounds( object, color );
                    index++;
                }
            );
        }
    }
    if( mShowSceneGraph ) {
        SceneManager *sceneMgr = mRoot->getSceneManager();
        Scene *scene = sceneMgr->getScene();
        if( scene ) {
            SceneGraph *graph = scene->getSceneGraph();
            
            std::function<void(int,int,SceneNode*)> visitor;
            
            visitor = [&]( int level, int child, SceneNode *node ) {
                glm::vec4 color( glm::fract(level*0.16f+child*0.05f), 0.8f, 0.8f, 0.5f );
                ImGui::ColorConvertHSVtoRGB( color.r, color.g, color.b, color.r, color.g, color.b );
                
                showSceneNode( node, color );
                
                SceneNode *children = node->getChildren();
                if( children ) {
                    for( int i=0; i < 8; ++i ) {
                        visitor( level+1, i, children+i ); 
                    }
                }
            };
            
            visitor( 0, 0, graph->getRootNode() );
        }
    }
    if( mShowSavedFrustrum ) {
        glm::mat4 modelMatrix = glm::inverse( mSavedViewProjMatrix );
        debugDrawer->drawWireBox( glm::vec3(1.f), modelMatrix, COLOR_SAVED_FRUSTRUM );
    }
}

void DebugManager::showSceneNode( SceneNode *node, const glm::vec4 &color )
{
    const BoundingSphere &bounds = node->getBounds();
    DebugDrawer *debugDrawer = mRoot->getDebugDrawer();
    
    float radius = bounds.getRadius();
    if( mAdjustSceneNodesSize ) {
        radius *= 0.5f;
    }
    
    glm::vec4 realColor = color;
    if( mShowBoundsInFrustrumColor ) {
        realColor = getColorFromFrustrumTest( bounds );
    }
    
    if( mShowSceneNodesAsBoxes ) {
        debugDrawer->drawWireBox( glm::vec3(radius*glm::one_over_root_two<float>()), glm::translate(glm::mat4(),bounds.getCenter()), realColor );
    }
    else {
        debugDrawer->drawWireSphere( radius, glm::translate(glm::mat4(),bounds.getCenter()), realColor );
    }
}

void DebugManager::showObjectBounds( SceneObject *object, const glm::vec4 &color )
{
    const BoundingSphere &bounds = object->getTransformedBoundingSphere();
    DebugDrawer *debugDrawer = mRoot->getDebugDrawer();
    
    glm::vec4 realColor = color;
    if( mShowBoundsInFrustrumColor ) {
        realColor = getColorFromFrustrumTest( bounds );
    }
    
    debugDrawer->drawWireSphere( bounds.getRadius(), glm::translate(glm::mat4(),bounds.getCenter()), realColor );
}

glm::vec4 DebugManager::getColorFromFrustrumTest( const BoundingSphere &bounds )
{
    Frustrum::TestStatus result = mCurrentFrustrum.isInside( bounds );
    switch( result ) {
    case( Frustrum::TestStatus::Outside ):
        return COLOR_FRUSTUM_TEST_OUTSIDE;
    case( Frustrum::TestStatus::Intersecting ):
        return COLOR_FRUSTUM_TEST_INTERSECTING;
    case( Frustrum::TestStatus::Inside ):
        return COLOR_FRUSTUM_TEST_INSIDE;
    }
    
    return glm::vec4();
}

void RenderImGuiDrawLists( ImDrawList** const draw_lists, int count )
{
    /// mostly taken from the ImGui OpenGL3 example
    ImGuiIO &io = ImGui::GetIO();
    ImGuiRenderData *renderData = static_cast<ImGuiRenderData*>(io.UserData);
    
    size_t totalVertexCount = 0;
    for( int i=0; i < count; ++i ) {
        totalVertexCount += draw_lists[i]->vtx_buffer.size();
    }
    
    size_t neededBufferSize = totalVertexCount * sizeof(ImDrawVert);
    size_t bufferSize = renderData->vertexBuffer->getSize();
    if( neededBufferSize > bufferSize ) {
        bufferSize = neededBufferSize + 5000;
        renderData->vertexBuffer->setSize( bufferSize );
    }
    
    renderData->draws.clear();
    
    ImDrawVert *bufferData = renderData->vertexBuffer->mapBuffer<ImDrawVert>(BufferUsage::WriteOnly ) ;
    for( int i=0; i < count; ++i ) {
        const ImDrawList *drawList = draw_lists[i];
        memcpy( bufferData, &drawList->vtx_buffer[0], drawList->vtx_buffer.size()*sizeof(ImDrawVert) );
        bufferData += drawList->vtx_buffer.size();
    }
    renderData->vertexBuffer->unmapBuffer();
    
    size_t vertexStart = 0;
    for( int i=0; i < count; ++i ) {
        const ImDrawList *cmdList = draw_lists[i];
        
        for( const ImDrawCmd &command : cmdList->commands ) {
            int x = command.clip_rect.x,
                y = command.clip_rect.w, 
                width  = command.clip_rect.z - command.clip_rect.x, 
                height = command.clip_rect.w - command.clip_rect.y;
                
            DrawInfo draw;
                draw.scissorPos = glm::vec2(x,y);
                draw.scissorSize = glm::vec2(width,height);
                draw.vertexStart = vertexStart;
                draw.vertexCount = command.vtx_count;
                
            renderData->draws.push_back( draw );
            
            vertexStart += command.vtx_count;
        }
    }
}

