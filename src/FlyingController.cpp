#include "FlyingController.h"
#include "InputManager.h"
#include "Config.h"

#include <SDL2/SDL_events.h>
#include <glm/gtx/transform.hpp>
#include <cassert>

FlyingController::FlyingController( InputManager *inputMgr, int priority ) :
    mInputMgr(inputMgr)
{
    std::fill( std::begin(mKeyBindings), std::end(mKeyBindings), SDLK_UNKNOWN );
    std::fill( std::begin(mKeysDown), std::end(mKeysDown), false );
    
    mListener.controller = this;
    mInputMgr->addListener( &mListener, priority );
}

FlyingController::~FlyingController()
{
    mInputMgr->removeListener( &mListener );
}

void FlyingController::update( float dt )
{
    // Find how much we should turn / roll
    glm::vec3 angleSpeed;
    if( mKeysDown[KB_TurnUp] ) {
        angleSpeed.x += 1.f;
    }
    if( mKeysDown[KB_TurnDown] ) {
        angleSpeed.x -= 1.f;
    }
    if( mKeysDown[KB_TurnLeft] ) {
        angleSpeed.y += 1.f;
    }
    if( mKeysDown[KB_TurnRight] ) {
        angleSpeed.y -= 1.f;
    }
    if( mKeysDown[KB_RollLeft] ) {
        angleSpeed.z += 1.f;
    }
    if( mKeysDown[KB_RollRight] ) {
        angleSpeed.z -= 1.f;
    }
    
    if( mKeysDown[KB_Fast] ) {
        angleSpeed *= mFastTurnSpeed;
    }
    if( mKeysDown[KB_Slow] ) {
        angleSpeed *= mSlowTurnSpeed;
    }
    
    angleSpeed *= mTurnSpeed;
    
    // Update our target orientation
    mTargetOrientation *= glm::quat(angleSpeed*dt);
    
    // Find the distance that we should move
    glm::vec3 distance;
    if( mKeysDown[KB_Right] ) {
        distance.x += 1.f;
    }
    if( mKeysDown[KB_Left] ) {
        distance.x -= 1.f;
    }
    if( mKeysDown[KB_Forward] ) {
        distance.z -= 1.f;
    }
    if( mKeysDown[KB_Backward] ) {
        distance.z += 1.f;
    }
    if( mKeysDown[KB_Up] ) {
        distance.y += 1.f;
    }
    if( mKeysDown[KB_Down] ) {
        distance.y -= 1.f;
    }

    // Find our speed
    glm::vec3 speed = mMoveSpeed;
    if( mKeysDown[KB_Fast] ) {
        speed = mFastMoveSpeed;
    }
    if( mKeysDown[KB_Slow] ) {
        speed = mSlowMoveSpeed;
    }
    
    distance.x *= speed.x;
    distance.z *= speed.y;
    
    // Calculate translation based on orientation
    distance = mOrientation * distance;
    
    // Update target our position
    mTargetPosition += distance * dt;
    
    
    // Interpolate
    mPosition += (mTargetPosition-mPosition) * mPositionInterpolation * dt;
    
    mOrientation = glm::mix( mOrientation, mTargetOrientation, mOrientationInterpolation*dt);
    
    updateTransform();
}

glm::mat4 FlyingController::getTransformation()
{
    return mTransform;
}

glm::vec3 FlyingController::getPosition()
{
    return mPosition;
}

void FlyingController::onKeyDown( const SDL_KeyboardEvent &event )
{
    for( unsigned int i=0; i < KB_COUNT; ++i ) {
        if( event.keysym.sym == mKeyBindings[i] ) {
            mKeysDown[i] = true;
        }
    }
    
    if( event.keysym.sym == mKeyBindings[KB_ToogleMouseControll] ) {
        mMouseControll = !mMouseControll;
    }
}

void FlyingController::onKeyUp( const SDL_KeyboardEvent &event )
{
    for( unsigned int i=0; i < KB_COUNT; ++i ) {
        if( event.keysym.sym == mKeyBindings[i] ) {
            mKeysDown[i] = false;
        }
    }
}

void FlyingController::onMouseMove( const SDL_MouseMotionEvent &event )
{
    if( !mMouseControll ) return;
    
    glm::vec3 motion( event.yrel, event.xrel, 0.f );
    motion *= mMouseSense;
    
//     if( mKeysDown[KB_Fast] ) {
//         motion *= mFastTurnSpeed;
//     }
//     if( mKeysDown[KB_Slow] ) {
//         motion *= mSlowTurnSpeed;
//     }
    
    mTargetOrientation *= glm::quat(-motion);
}

void FlyingController::onMouseScroll( const SDL_MouseWheelEvent &event )
{
    if( !mMouseControll ) return;

    float roll = event.y * mMouseSense.z;
    
    if( mKeysDown[KB_Fast] ) {
        roll *= mFastTurnSpeed.z;
    }
    if( mKeysDown[KB_Slow] ) {
        roll *= mSlowTurnSpeed.z;
    }
    
    mTargetOrientation *= glm::quat(glm::vec3(0.f,0.f,-roll) );
}

void FlyingController::updateTransform()
{
    mTransform = glm::translate(mPosition) * glm::mat4_cast(mOrientation);
}

void FlyingController::setKeyBinding( FlyingController::KeyBinding binding, Int32 key )
{
    int index = static_cast<int>(binding);
    assert( index >= 0 && index < KB_COUNT );
    mKeyBindings[binding] = key;
}

void FlyingController::loadFromConfig( const FlyingControllConfig &config )
{
    setKeyBinding( KB_Left,  config.left );
    setKeyBinding( KB_Right, config.right);
    setKeyBinding( KB_Forward,  config.forward );
    setKeyBinding( KB_Backward, config.backward );
    setKeyBinding( KB_Up, config.up );
    setKeyBinding( KB_Down, config.down );
    
    setKeyBinding( KB_Fast, config.fast );
    setKeyBinding( KB_Slow, config.slow );
    
    setKeyBinding( KB_TurnUp,    config.turnUp );
    setKeyBinding( KB_TurnDown,  config.turnDown );
    setKeyBinding( KB_TurnLeft,  config.turnLeft );
    setKeyBinding( KB_TurnRight, config.turnRight );
    setKeyBinding( KB_RollLeft,  config.rollLeft );
    setKeyBinding( KB_RollRight, config.rollRight );
    
    setKeyBinding( KB_ToogleMouseControll, config.toogleMouseControll );
}

void FlyingController::setMouseControll( bool mouseControll )
{
    mMouseControll = mouseControll;
}

