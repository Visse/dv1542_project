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

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mouse.h>

#include <imgui.h>

#include <functional>

class DebugFrameListener :
    public FrameListener
{
public:
    virtual void onFrameBegun() override {
    }
    virtual void onFrameEnded() override {
        debugMgr->paintDebugOverlay();
    }
    DebugManager *debugMgr;
};

bool DebugManager::init( Root *root )
{
    Timer initTimer;
    
    mRoot = root;
    
    const Config *config = mRoot->getConfig();
    mKeyToogleDebug = config->keyBindings.toogleDebug;
    
    initImGui();
    
    mFrameListener = new DebugFrameListener;
    mFrameListener->debugMgr = this;
    
    GraphicsManager *graphicsMgr = mRoot->getGraphicsManager();
    graphicsMgr->addFrameListener( mFrameListener );
    
    StartupMesurements *mesurements = mRoot->getStartupMesurements();
    mesurements->debugStartup = initTimer.getTimeAsSeconds();
    
    return true;
}

void DebugManager::destroy()
{
    destroyImGui();
    
    GraphicsManager *graphicsMgr = mRoot->getGraphicsManager();
    graphicsMgr->removeFrameListener( mFrameListener );
    
    delete mFrameListener;
    mFrameListener = nullptr;
    
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
                ImGui::PlotLines( "Frame Time", mRoot->getFrameTimeHistory(), 0, 0.1f, 0.1f, ImVec2(0,70) );
                ImGui::PlotLines( "Frame Rate", mRoot->getFrameRateHistory(), 0, 10.f, -0.f, ImVec2(0,70) );
            }
            
            SceneManager *sceneMgr = mRoot->getSceneManager();
            Scene *scene = sceneMgr->getScene();
            
            if( scene ) {
                if( ImGui::CollapsingHeader("Scene") ) {
                    glm::vec3 ambientColor = scene->getAmbientColor();
                    
                    if( ImGui::ColorEdit3("Ambient", glm::value_ptr(ambientColor)) ) {
                        scene->setAmbientColor( ambientColor );
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

struct ImGuiRenderData {
    DebugManager *debugMgr;
    VertexArrayObject vao;
    GpuBuffer buffer;
    SharedPtr<Material> material;
    float height;
};
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
    io.UserData = renderData;
    io.DisplaySize.x = config->windowWidth;
    io.DisplaySize.y = config->windowHeight;

    unsigned char *pixels;
    int texWidth, texHeight;
    io.Fonts->GetTexDataAsRGBA32( &pixels, &texWidth, &texHeight );
    
    SharedPtr<Texture> texture = Texture::LoadTextureFromRawMemory( TextureType::RGBA, pixels, texWidth, texHeight );

    ResourceManager *resourceMgr = mRoot->getResourceManager();
    renderData->material = resourceMgr->getMaterialAutoPack("DebugGuiMaterial");
    
    
    texture->bindTexture(0);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    texture->unbindTexture(0);
    
    renderData->material->setTexture( "Texture", 0, texture );
    
    renderData->height = config->windowHeight;
    
    SharedPtr<GpuProgram> program = renderData->material->getProgram();
    
    GLint posLocation = program->getAttribLocation("Position"),
          uvLocation  = program->getAttribLocation("UV"),
          colourLocation = program->getAttribLocation("Colour"),
          textureLocation = program->getUniformLocation("Texture"),
          othoLocation = program->getUniformLocation("ortho");
    
    program->bindProgram();
    float width = config->windowWidth,
          height = config->windowHeight;
    const float ortho_projection[4][4] =
    {
        { 2.0f/width,   0.0f,           0.0f,       0.0f },
        { 0.0f,         2.0f/-height,   0.0f,       0.0f },
        { 0.0f,         0.0f,           -1.0f,      0.0f },
        { -1.0f,        1.0f,           0.0f,       1.0f },
    };
    glUniform1i( textureLocation, 0 );
    glUniformMatrix4fv( othoLocation, 1, GL_FALSE, &ortho_projection[0][0] );
    
    renderData->buffer.setUsage( BufferUsage::WriteOnly );
    renderData->buffer.setUpdate( BufferUpdate::Stream );
    renderData->buffer.setSize( 20000 );
    
    renderData->vao.bindVAO();
    renderData->buffer.bindBuffer();
    
    renderData->vao.setVertexAttribPointer( posLocation, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), offsetof(ImDrawVert, pos) );
    renderData->vao.setVertexAttribPointer( uvLocation, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), offsetof(ImDrawVert, uv) );
    renderData->vao.setVertexAttribPointer( colourLocation, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), offsetof(ImDrawVert, col)) ;
    
    renderData->vao.unbindVAO();
    renderData->buffer.unbindBuffer();
    
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
    
    if( ImGui::TreeNode(typeid(*object).name()) ) {
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
        
        PointLight *pointLight = dynamic_cast<PointLight*>(object);
        if( pointLight ) {
            float outerRadius = pointLight->getOuterRadius(),
                  innerRadius = pointLight->getInnerRadius();
            glm::vec3 color = pointLight->getColor();
                  
            if( ImGui::SliderFloat("OuterRadius", &outerRadius, 0.f, 20.f) ) {
                pointLight->setOuterRadius( outerRadius );
            }
            if( ImGui::SliderFloat("InnerRadius", &innerRadius, 0.f, outerRadius) ) {
                pointLight->setInnerRadius( innerRadius );
            }
            
            if( ImGui::ColorEdit3("Color", glm::value_ptr(color)) ) {
                pointLight->setColor( color );
            }
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
        if( info.mesh && info.wireFrame ) {
            debugDrawer->drawWireFrame( info.mesh, entry.first->getTransform() );
        }
        if( info.mesh && info.normals ) {
            debugDrawer->drawVertexNormals( info.mesh, entry.first->getTransform() );
        }
    }
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
    size_t bufferSize = renderData->buffer.getSize();
    if( neededBufferSize > bufferSize ) {
        bufferSize = neededBufferSize + 5000;
        renderData->buffer.setSize( bufferSize );
    }
    
    ImDrawVert *bufferData = static_cast<ImDrawVert*>(renderData->buffer.mapBuffer(BufferUsage::WriteOnly));
    for( int i=0; i < count; ++i ) {
        const ImDrawList *drawList = draw_lists[i];
        memcpy( bufferData, &drawList->vtx_buffer[0], drawList->vtx_buffer.size()*sizeof(ImDrawVert) );
        bufferData += drawList->vtx_buffer.size();
    }
    renderData->buffer.unmapBuffer();
    
    glDisable( GL_CULL_FACE );
    glEnable( GL_SCISSOR_TEST );
    
    renderData->material->bindMaterial();
    renderData->vao.bindVAO();
    
    int cmdOffset = 0;
    float height = renderData->height;
    for( int i=0; i < count; ++i ) {
        const ImDrawList *cmdList = draw_lists[i];
        int vertexOffset = cmdOffset;
        
        for( const ImDrawCmd &command : cmdList->commands ) {
            int x = command.clip_rect.x,
                y = height - command.clip_rect.w, 
                width  = command.clip_rect.z - command.clip_rect.x, 
                height = command.clip_rect.w - command.clip_rect.y;
            glScissor( x, y, width, height );
            glDrawArrays( GL_TRIANGLES, vertexOffset, command.vtx_count );
            vertexOffset += command.vtx_count;
        }
        cmdOffset = vertexOffset;
    }
    
    renderData->vao.unbindVAO();
    
    glDisable( GL_SCISSOR_TEST );
    glEnable( GL_CULL_FACE );
}

