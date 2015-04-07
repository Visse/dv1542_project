#include "DebugManager.h"
#include "GraphicsManager.h"
#include "FrameListener.h"
#include "Texture.h"
#include "Root.h"
#include "GpuBuffer.h"
#include "VertexArrayObject.h"
#include "GLinclude.h"
#include "GpuProgram.h"
#include "Config.h"
#include "ImGuiHelpers.h"
#include "StartupMesurements.h"
#include "Timer.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "DebugDrawer.h"
#include "Camera.h"
#include "SceneGraph.h"
#include "Renderable.h"
#include "Renderer.h"
#include "SceneObject.h"
#include "LightObject.h"
#include "DebugLogListener.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mouse.h>

#include <imgui.h>

#include <functional>
#include <iostream>


static const glm::vec4 COLOR_FRUSTUM_TEST_OUTSIDE = glm::vec4(0.8f,0.3f,0.3f,1.f),
                       COLOR_FRUSTUM_TEST_INTERSECTING = glm::vec4(0.4f,0.2f,0.8f,1.f),
                       COLOR_FRUSTUM_TEST_INSIDE = glm::vec4(0.3f,0.8f,0.5f,1.f);

static const ImVec4 LOG_COLOR_DEBUG = ImVec4(0.2,0.8,0.2,1.0),
                    LOG_COLOR_INFORMATION = ImVec4(0.8,0.8,0.8,1.0),
                    LOG_COLOR_WARNING = ImVec4(0.8,0.8,0.2,1.0),
                    LOG_COLOR_ERROR = ImVec4(0.8,0.2,0.2,1.0),
                    LOG_COLOR_CRITICAL = ImVec4(0.8,0.2,0.8,1.0);
                       
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

                    
                    
class DebugManager::DebugFrameListener :
    public FrameListener
{
public:
    DebugFrameListener( DebugManager *debugMgr, const SharedPtr<Texture> &texture, 
                        const SharedPtr<GpuProgram> &shader, const SharedPtr<VertexArrayObject> &vao ) :
        mDebugMgr(debugMgr),
        mTexture(texture),
        mShader(shader),
        mVAO(vao)
    {
        mGraphicsMgr = mDebugMgr->mRoot->getGraphicsManager();
        mRenderer = mGraphicsMgr->getRenderer();
        mRenderable.debugMgr = debugMgr;
        
        mGraphicsMgr->addFrameListener( this );
    }
    
    virtual ~DebugFrameListener()
    {
        mGraphicsMgr->removeFrameListener( this );
    }
    
    virtual void onFrameBegun() override
    {
        CustomRenderableSettings settings;
            settings.vao = mVAO;
            settings.program = mShader;
            settings.textures[0] = mTexture;
            settings.uniforms[0] = mRenderer->aquireUniformBuffer( mDebugMgr->mUniforms );
            settings.blendMode = BlendMode::AlphaBlend;
            settings.renderable = &mRenderable;
            settings.queue = 10;
        
        mRenderer->addCustomRenderable( settings );
    }
    
private:
    struct :   
        public Renderable
    {
        DebugManager *debugMgr;
        
        virtual void render( Renderer &renderer )
        {
            debugMgr->render( );
        }
    } mRenderable;
    
    DebugManager *mDebugMgr;
    GraphicsManager *mGraphicsMgr;
    Renderer *mRenderer;
    
    SharedPtr<Texture> mTexture;
    SharedPtr<GpuProgram> mShader;
    SharedPtr<VertexArrayObject> mVAO;
};

bool DebugManager::init( Root *root )
{
    Timer initTimer;
    
    mRoot = root;
    
    mLogListener = new DebugLogListener;
    Log *log = mRoot->getDefaultLog();
    log->addListener( mLogListener );
    
    const Config *config = mRoot->getConfig();
    mKeyToogleDebug = config->keyBindings.toogleDebug;
    
    mHeight = config->windowHeight;
    
    float width = config->windowWidth,
          height = config->windowHeight;
    
    mUniforms.projectionMatrix = glm::mat4(
         2.0f/width, 0.0f,         0.0f,   0.0f,
         0.0f,       2.0f/-height, 0.0f,   0.0f,
         0.0f,       0.0f,        -1.0f,   0.0f,
        -1.0f,       1.0f,         0.0f,   1.0f
    );
    
    mVertexBuffer = GpuBuffer::CreateBuffer( BufferType::Vertexes, 20000, BufferUsage::WriteOnly, BufferUpdate::Dynamic );

    
    initImGui();

    
    StartupMesurements *mesurements = mRoot->getStartupMesurements();
    mesurements->debugStartup = initTimer.getTimeAsSeconds();
    
    return true;
}

void DebugManager::postInit()
{    
    SharedPtr<VertexArrayObject> vao = makeSharedPtr<VertexArrayObject>();
    vao->bindVAO();
    mVertexBuffer->bindBuffer();
    
    vao->setVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), offsetof(ImDrawVert, pos) );
    vao->setVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), offsetof(ImDrawVert, uv) );
    vao->setVertexAttribPointer( 2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), offsetof(ImDrawVert, col)) ;
    
    vao->unbindVAO();
    mVertexBuffer->unbindBuffer();

    unsigned char *pixels;
    int texWidth, texHeight;
    
    ImGuiIO &io = ImGui::GetIO();
    io.Fonts->GetTexDataAsRGBA32( &pixels, &texWidth, &texHeight );
    
    SharedPtr<Texture> texture = Texture::LoadTextureFromRawMemory( TextureType::RGBA, pixels, texWidth, texHeight );

    texture->bindTexture(0);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    texture->unbindTexture(0);
    
    ResourceManager *resourceMgr = mRoot->getResourceManager();
    SharedPtr<GpuProgram> shader = resourceMgr->getGpuProgramAutoPack("DebugGuiShader");
    
    mFrameListener = new DebugFrameListener( this, texture, shader, vao );
}

void DebugManager::destroy()
{
    delete mLogListener;
    mLogListener = nullptr;
    
    delete mFrameListener;
    mFrameListener = nullptr;
    
    destroyImGui();
    
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
                Renderer *renderer = graphicsMgr->getRenderer();
                
                ImGui::PlotLines( "Frame Time", mRoot->getFrameTimeHistory(), 1.f, 1.f, ImVec2(0,70) );
                ImGui::PlotLines( "Frame Rate", mRoot->getFrameRateHistory(), 10.f, 0.f, ImVec2(0,70) );
                ImGui::PlotLines( "Gpu Time", graphicsMgr->getGpuTimeHistory(), 1.0f, 1.0f, ImVec2(0,70) );
                
                const auto &memUsage = renderer->getMemoryUsageHistory();
                ImGui::PlotLines( "Mem Usage", [&]( int i ){ return memUsage.getValue(i) / 1024.f; }, memUsage.getSize(), 1.0f, 1.0f, ImVec2(0,70) );

                Renderer::RendererStatistics statistics = renderer->getStatistics();
                
                ImGui::Value( "Drawn Meshes", (int)statistics.totDrawnMeshes );
                ImGui::Value( "Drawn Entities", (int)statistics.drawnEntities );
                ImGui::Value( "Drawn PointLights", (int)statistics.drawnPointLights );
                ImGui::SameLine();
                ImGui::Value( "Shadow meshes", (int)statistics.drawnPointShadowMap );
                ImGui::Value( "Drawn PointLights/WoS", (int)statistics.drawnPointLightsNoShadow );
                ImGui::Value( "Custom Rendereables", (int)statistics.customRenderables );
            }
            
            if( ImGui::CollapsingHeader("Logs") ) {
                ImGui::BeginChild( "DefaultLog", ImVec2(0,200), true );
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4,1));
                const auto logEntries = mLogListener->getMessages();
                for( const auto &message : logEntries ) {
                    ImVec4 color;
                    switch( message.severity ) {
                    case( LogSeverity::Debug ):
                        color = LOG_COLOR_DEBUG;
                        break;
                    case( LogSeverity::Information ):
                        color = LOG_COLOR_INFORMATION;
                        break;
                    case( LogSeverity::Warning ):
                        color = LOG_COLOR_WARNING;
                        break;
                    case( LogSeverity::Error ):
                        color = LOG_COLOR_ERROR;
                        break;
                    case( LogSeverity::Critical ):
                        color = LOG_COLOR_CRITICAL;
                        break;
                    case( LogSeverity::COUNT ):
                        break;
                    }
                    
                    std::string severity = logSeverityToString(message.severity);
                    ImGui::TextColored( color, "[%s][%s]\t%s", severity.c_str(), message.module, message.msg.c_str() );
                    if( ImGui::IsItemHovered() ) {
                        ImGui::SetTooltip( "%s", message.msg.c_str() );;
                    }
                }
                
                ImGui::PopStyleVar();
                ImGui::EndChild();
            }
            
            SceneManager *sceneMgr = mRoot->getSceneManager();
            Scene *scene = sceneMgr->getScene();
            
            if( scene ) {
                if( ImGui::CollapsingHeader("Scene") ) {
                    bool showBounds = mShowSceneBounds | mShowSceneLightsBounds;
                    if( ImGui::Checkbox("Show Bounds", &showBounds) ) {
                        mShowSceneBounds = showBounds;
                        mShowSceneLightsBounds = showBounds;
                    }
                    ImGui::SameLine();
                    ImGui::PushID("ShowBounds");
                    if( ImGui::CollapsingHeader( "[more]", "", false ) ) {
                        ImGui::TreePush("ShowBoundsj");
                        ImGui::Checkbox("Entities", &mShowSceneBounds );
                        ImGui::Checkbox("Lights", &mShowSceneLightsBounds );
                        ImGui::TreePop();
                    }
                    ImGui::PopID();
                    
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
                        ImGui::SliderFloat( "Alpha", &mGBufferAlpha, 0.f, 1.f );
                        ImGui::Checkbox( "FullScreen", &mShowGBufferFullScreen );
                        ImGui::Checkbox( "Normal", &mShowGBufferNormal );
                        ImGui::Checkbox( "Depth", &mShowGBufferDepth );
                        ImGui::Checkbox( "Diffuse", &mShowGBufferDiffuse );
                        ImGui::TreePop();
                    }
                    
                    bool paused = scene->getPaused();
                    if( ImGui::Checkbox("Paused", &paused) ) {
                        scene->setPaused( paused );
                    }
                    GraphicsManager *graphicsMgr = mRoot->getGraphicsManager();
                    Renderer *renderer = graphicsMgr->getRenderer();
                    bool renderWireFrame = renderer->getRenderWireFrame();
                    if( ImGui::Checkbox("RenderWireFrame", &renderWireFrame) ) {
                        renderer->setRenderWireFrame( renderWireFrame );
                    }
                    
                    bool useOcclusionQuarries = renderer->getUseOcclusionQuarries();
                    if( ImGui::Checkbox("Use Occlusion Quarries", &useOcclusionQuarries) ) {
                        renderer->setUseOcclusionQuarries( useOcclusionQuarries );
                    }
                    
                    bool useFrustrumCulling = scene->getUseFrustrumCulling();
                    if( ImGui::Checkbox("Use Frustrum Culling", &useFrustrumCulling) ) {
                        scene->setUseFrustrumCulling( useFrustrumCulling );
                    }
                    
                    glm::vec3 ambientColor = scene->getAmbientColor();
                    if( ImGui::ColorEdit3("Ambient", glm::value_ptr(ambientColor)) ) {
                        scene->setAmbientColor( ambientColor );
                    }
                }
                
                
                if( ImGui::CollapsingHeader("SceneObjects") ) {
                    scene->forEachObject(
                        [&]( SceneObject *object ) {
                            std::string objectType = demangleName(typeid(*object).name());
                            
                            if( ImGui::TreeNode(object, "%s", objectType.c_str()) ) {
                                showSceneObject( dt, object );
                                ImGui::TreePop();
                            }
                        }
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


void DebugManager::render()
{
    if( mIsDebugVisible ) {
        ImGui::Render();
        
        ImGuiIO &io = ImGui::GetIO();
        ImGuiRenderData *renderData = static_cast<ImGuiRenderData*>(io.UserData);
        
        glEnable( GL_SCISSOR_TEST );
        glDisable( GL_CULL_FACE );
        glDisable( GL_DEPTH_TEST );
        
        for( const DrawInfo &draw : renderData->draws )
        {
            glScissor( draw.scissorPos.x, mHeight-draw.scissorPos.y, draw.scissorSize.x, draw.scissorSize.y );
            glDrawArrays( GL_TRIANGLES, draw.vertexStart, draw.vertexCount );
        }
        
        glDisable( GL_SCISSOR_TEST );
        glEnable( GL_CULL_FACE );
        glEnable( GL_DEPTH_TEST );
    }
    /*
    switch( mGBufferDebug ) {
    case( GBufferDebug::None ):
        break;
    case( GBufferDebug::Normal ): {
        LowLevelOperationParams params;
            params.material = mGBufferNormalMaterial.get();
            params.drawMode = DrawMode::Points;
            params.renderQueue = RQ_OverlayFirst;
            params.vertexCount = 1;
            
        renderer.queueOperation( params );
      } break;
    case( GBufferDebug::Depth ): {
        LowLevelOperationParams params;
            params.material = mGBufferDepthMaterial.get();
            params.drawMode = DrawMode::Points;
            params.renderQueue = RQ_OverlayFirst;
            params.vertexCount = 1;
        
        renderer.queueOperation( params );
      } break;
    }
    */
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

