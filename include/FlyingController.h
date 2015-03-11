#pragma once

#include "FixedSizeTypes.h"

#include "Controller.h"
#include "InputListener.h"

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>

struct FlyingControllConfig;
class InputManager;

class FlyingController :
    public Controller
{
public:
    enum KeyBinding {
        KB_Left    = 0,
        KB_Right,
        KB_Forward,
        KB_Backward,
        KB_Up,
        KB_Down,
        
        KB_Fast,
        KB_Slow,
        
        KB_TurnUp,
        KB_TurnDown,
        KB_TurnLeft,
        KB_TurnRight,
        KB_RollLeft,
        KB_RollRight,
        
        KB_ToogleMouseControll,
        
        KB_COUNT
    };
    
public:
    FlyingController( InputManager *inputMgr, int priority=0 );
    virtual ~FlyingController();
    
    virtual void update( float dt ) override;
    virtual glm::mat4 getTransformation() override;
    virtual glm::vec3 getPosition() override;
    
    void setKeyBinding( KeyBinding binding, Int32 key );
    void loadFromConfig( const FlyingControllConfig &config );
    
    void setMouseControll( bool mouseControll );
    
    void setPosition( const glm::vec3 &pos ) {
        mPosition = pos;
        mTargetPosition = pos;
    }
    
private:
    void updateTransform();
    
    void onKeyDown( const SDL_KeyboardEvent &event );
    void onKeyUp( const SDL_KeyboardEvent &event );
    void onMouseMove( const SDL_MouseMotionEvent &event );
    void onMouseScroll( const SDL_MouseWheelEvent &event );
    
private:
    struct FlyingInputListener :
        public InputListener
    {
        FlyingController *controller;
        virtual void onKeyDown( const SDL_KeyboardEvent &event ) override {
            controller->onKeyDown( event );
        }
        virtual void onKeyUp( const SDL_KeyboardEvent &event ) override {
            controller->onKeyUp( event );
        }
        virtual void onMouseMove( const SDL_MouseMotionEvent &event ) override {
            controller->onMouseMove( event );
        }
        virtual void onMouseScroll( const SDL_MouseWheelEvent &event ) override {
            controller->onMouseScroll( event );
        }
    };
    
private:
    InputManager *mInputMgr;
    FlyingInputListener mListener;
    
    glm::vec3 mPosition, mTargetPosition;
    glm::quat mOrientation, mTargetOrientation;
    glm::mat4 mTransform;
    
    glm::vec3 mMoveSpeed = glm::vec3(3.f, 3.f, 3.f),
              mFastMoveSpeed = glm::vec3(16.f, 16.f, 16.f), 
              mSlowMoveSpeed = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::vec3 mTurnSpeed = glm::vec3(2.0f, 2.0f, 1.5f),
              mFastTurnSpeed = glm::vec3(4.f, 4.f, 3.f ),
              mSlowTurnSpeed = glm::vec3(0.5f, 0.5f, 0.375f ),
              mMouseSense = glm::vec3(0.005f, 0.005f, 0.1f);
    
    float mPositionInterpolation    = 1.f / 0.2f,
          mOrientationInterpolation = 1.f / 0.05f;
    
    bool mMouseControll = false;
          
    Int32 mKeyBindings[KB_COUNT];
    
    bool mKeysDown[KB_COUNT];
};