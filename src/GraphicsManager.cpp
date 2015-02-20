#include "GraphicsManager.h"
#include "Root.h"
#include "Config.h"
#include "FrameListener.h"
#include "GLinclude.h"
#include "Timer.h"
#include "StartupMesurements.h"
#include "LowLevelRenderer.h"
#include "Camera.h"

#include <SDL2/SDL.h>

#include <algorithm>
#include <cassert>


void GLAPIENTRY debugCallback( GLenum source, GLenum type, unsigned int id, GLuint severity, GLsizei length, const char *message, void *userParam );
// seems like there some variations for the definition of GLDEBUGPROC, well I'm not using the userParam so it shouldn't matter
void GLAPIENTRY debugCallback( GLenum source, GLenum type, unsigned int id, GLuint severity, GLsizei length, const char *message, const void *userParam ) {
    debugCallback( source, type, id, severity, length, message, (void*)0 );
}

bool GraphicsManager::init( Root *root )
{
    Timer initTimer;
    
    mRoot = root;
    const Config *config = mRoot->getConfig();
    
    mWindow = SDL_CreateWindow( config->windowTitle.c_str(), 
                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                      config->windowWidth, config->windowHeight,
                      SDL_WINDOW_OPENGL
                    );
    if( !mWindow ) {
        return false;
    }
    
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
    if( config->openglDebug ) {
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG );
    }
    
    mGLContext = SDL_GL_CreateContext( mWindow );
    if( !mGLContext ) {
        SDL_DestroyWindow( mWindow );
        mWindow = nullptr;
        return false;
    }
    
    // important!!
    glewExperimental = true;
    GLenum status = glewInit();
    if( status != GLEW_OK ) {
        SDL_GL_DeleteContext( mGLContext );
        SDL_DestroyWindow( mWindow );
        mGLContext = nullptr;
        mWindow = nullptr;
        
        return false;
    }
    
    glEnable( GL_BLEND );
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );
    
    if( config->openglDebug && glDebugMessageCallback ) {
        glDebugMessageCallback( debugCallback, NULL );
        glEnable( GL_DEBUG_OUTPUT );
        glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
    }
    
    StartupMesurements *mesurements = mRoot->getStartupMesurements();
    mesurements->graphicsStartup = initTimer.getTimeAsSeconds();
    
    mGpuTimes.setSize( config->valueHistoryLenght );
    mSamplePassed.setSize( config->valueHistoryLenght );
    
    
    mNumOfQuaryObjects = 3;
    
    mTimeQuaryObjects.resize( mNumOfQuaryObjects );
    mSamplesQuaryObjects.resize( mNumOfQuaryObjects );
    
    glGenQueries( mNumOfQuaryObjects, &mTimeQuaryObjects[0] );
    glGenQueries( mNumOfQuaryObjects, &mSamplesQuaryObjects[0] );
    
    // this is to suppress warnings from the graphics driver - 
    // for trying to quary a object that doesn't have a value get,
    // this is easier that to keep track of how many of them I've used
    for( size_t i=0; i < mNumOfQuaryObjects; ++i ) {
        glBeginQuery( GL_TIME_ELAPSED, mTimeQuaryObjects[i] );
        glEndQuery( GL_TIME_ELAPSED );
    }
    for( size_t i=0; i < mNumOfQuaryObjects; ++i ) {
        glBeginQuery( GL_SAMPLES_PASSED, mSamplesQuaryObjects[i] );
        glEndQuery( GL_SAMPLES_PASSED );
    }
    
    return true;
}

void GraphicsManager::destroy()
{
    glDeleteQueries( mNumOfQuaryObjects, &mTimeQuaryObjects[0] );
    glDeleteQueries( mNumOfQuaryObjects, &mSamplesQuaryObjects[0] );
    
    delete mRenderer;
    mRenderer = nullptr;
    
    SDL_GL_DeleteContext( mGLContext );
    SDL_DestroyWindow( mWindow );
    
    mGLContext = nullptr;
    mWindow = nullptr;
}

void GraphicsManager::postInit()
{
    mRenderer = new LowLevelRenderer( mRoot );
}

void GraphicsManager::update( float dt )
{
}
#include <iostream>
void GraphicsManager::render()
{
    glBeginQuery( GL_TIME_ELAPSED, mTimeQuaryObjects[mCurrentQuary] );
    glBeginQuery( GL_SAMPLES_PASSED, mSamplesQuaryObjects[mCurrentQuary] );
    
    glDepthMask( GL_TRUE );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    fireFrameBegun();
    
    for( Camera *camera : mCameras ) {
        firePreCamera( camera );
        camera->render( *mRenderer );
        firePostCamera( camera );
    }
    mRenderer->flush();
    
    fireFrameEnded();
    
    {
        glEndQuery( GL_TIME_ELAPSED );
        glEndQuery( GL_SAMPLES_PASSED );
        
        GLint resultAvailable = -1;
        int index = (mCurrentQuary+1)%mNumOfQuaryObjects;
        glGetQueryObjectiv( mTimeQuaryObjects[index], GL_QUERY_RESULT_NO_WAIT, &resultAvailable );
        
        if( resultAvailable != -1 ) {
            mGpuTimes.pushValue( (double)(resultAvailable) / 1000000.0 );
        }
        else {
            std::cout << "Need more quary objects!";
        }
        
        resultAvailable = -1;
        glGetQueryObjectiv( mSamplesQuaryObjects[index], GL_QUERY_RESULT_NO_WAIT, &resultAvailable );
        
        if( resultAvailable != -1 ) {
            mSamplePassed.pushValue( (double)(resultAvailable)/10000.0 );
        }
        else {
            std::cout << "Need more quary objects!";
        }
        
        mCurrentQuary++;
        mCurrentQuary %= mNumOfQuaryObjects;
    }
    
    SDL_GL_SwapWindow(mWindow);
}

bool GraphicsManager::handleSDLEvent( const SDL_Event &event )
{
    if( event.type == SDL_WINDOWEVENT ) 
    {
        if( event.window.event == SDL_WINDOWEVENT_CLOSE ) {
            mRoot->quit();
            return true;
        }
    }
    return false;
}

void GraphicsManager::addFrameListener( FrameListener *listener )
{
    mFrameListeners.push_back( listener );
}

void GraphicsManager::removeFrameListener( FrameListener *listener )
{
    auto iter = std::find( mFrameListeners.begin(), mFrameListeners.end(), listener );
    
    assert( iter != mFrameListeners.end() );
    mFrameListeners.erase( iter );
}

void GraphicsManager::addCamera( Camera *camera )
{
    mCameras.push_back( camera );
}

void GraphicsManager::removeCamera( Camera *camera )
{
    auto iter = std::find( mCameras.begin(), mCameras.end(), camera );
    
    assert( iter != mCameras.end() );
    mCameras.erase( iter );
}

void GraphicsManager::fireFrameBegun()
{
    for( FrameListener *listener : mFrameListeners ) {
        listener->onFrameBegun();
    }
}

void GraphicsManager::fireFrameEnded()
{
    for( FrameListener *listener : mFrameListeners ) {
        listener->onFrameEnded();
    }
}

void GraphicsManager::firePreCamera( Camera *camera )
{
    for( FrameListener *listener : mFrameListeners ) {
        listener->onPreCameraDraw( camera );
    }

}

void GraphicsManager::firePostCamera( Camera *camera )
{
    for( FrameListener *listener : mFrameListeners ) {
        listener->onPostCameraDraw( camera );
    }

}


#include <cstdio>
void GLAPIENTRY debugCallback( GLenum source, GLenum type, unsigned int id, GLuint severity, GLsizei length, const char *message, void *userParam )
{
    (void)length;
    (void)userParam;
    
    /// modified code, originaly: https://sites.google.com/site/opengltutorialsbyaks/introduction-to-opengl-4-1---tutorial-05
    
    const char *sourceStr = "", *typeStr = "", *severityStr = "";
    
    if( source == GL_DEBUG_SOURCE_API_ARB ) {
        sourceStr = "OpenGL";
    } else if( source == GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB ) {
        sourceStr = "Windows";
    } else if( source == GL_DEBUG_SOURCE_SHADER_COMPILER_ARB ) {
        sourceStr = "Shader Compiler";
    } else if( source == GL_DEBUG_SOURCE_THIRD_PARTY_ARB ) {
        sourceStr = "Third Party";
    } else if( source == GL_DEBUG_SOURCE_APPLICATION_ARB ) {
        sourceStr = "Application";
    } else if( source == GL_DEBUG_SOURCE_OTHER_ARB ) {
        sourceStr = "Other";
    } 
    
    if( type == GL_DEBUG_TYPE_ERROR_ARB ) {
        typeStr = "Error";
    } else if( type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB ) {
        typeStr = "Deprecated behavior";
    } else if( type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB ) {
        typeStr = "Undefined behavior";
    } else if( type == GL_DEBUG_TYPE_PORTABILITY_ARB ) {
        typeStr = "Portability";
    } else if( type == GL_DEBUG_TYPE_PERFORMANCE_ARB ) {
        typeStr = "Performance";
    } else if( type == GL_DEBUG_TYPE_OTHER_ARB ) {
        typeStr = "Other";
    }

    if( severity == GL_DEBUG_SEVERITY_HIGH_ARB ) {
        severityStr = "High";
    } else if( severity == GL_DEBUG_SEVERITY_MEDIUM_ARB ) {
        severityStr = "Medium";
    } else if( severity == GL_DEBUG_SEVERITY_LOW_ARB ) {
        severityStr = "Low";
    } 
    
    std::fprintf( stderr, "[OpenGL]\tSource: %s\tType: %s\tID: %d\tSeverity: %s\tMessage: %s\n", sourceStr, typeStr, id, severityStr, message );
}
