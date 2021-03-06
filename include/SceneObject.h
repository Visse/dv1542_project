#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include "BoundingSphere.h"

class SceneGraph;
class SceneObjectFactory;
class Renderer;
class SceneNode;
class LowLevelRenderer;
class Renderable;

class SceneObject {
public:
    SceneObject( SceneObjectFactory *factory ) :
        mFactory(factory)
    {}
    virtual ~SceneObject() = default;
   
    virtual void update( float dt ) {}
    
    virtual void submitRenderer( Renderer &renderer ) {}
    virtual void submitShadowCasters( Renderer &renderer ) {}
    
    SceneObject* clone();
    
    void setPosition( const glm::vec3 &position ) {
        mPosition = position;
        markDirty();
    }
    void setOrientation( const glm::quat &orientation ) {
        mOrientation = orientation;
        markDirty();
    }
    void setRenderQueue( unsigned int queue ) {
        mRenderQueue = queue;
    }
    const glm::vec3& getPosition() {
        return mPosition;
    }
    const glm::quat& getOrientation() {
        return mOrientation;
    }
    const glm::mat4& getTransform() {
        return mTransform;
    }
    unsigned int getRenderQueue() {
        return mRenderQueue;
    }
    
    void setBoundingSphere( const BoundingSphere &bounds ) {
        mBoundingSphere = bounds;
        mDirty = true;
    }
    const BoundingSphere& getBoundingSphere() {
        return mBoundingSphere;
    }
    const BoundingSphere& getTransformedBoundingSphere() {
        return mTransformedBoundingSphere;
    }
    
    bool isDirty() {
        return mDirty;
    }
    // updates the transform & takes the object from the dirty stage
    // don't call this manaly
    void _updateTransform();
    void _setParent( SceneNode *node ) {
        mParent = node;
    }
    SceneNode* _getParent() {
        return mParent;
    }
    void _setAutoDelete( bool autoDelete ) {
        mAutoDelete = autoDelete;
    }
    bool _getAutoDelete() {
        return mAutoDelete;
    }
    
    SceneObjectFactory* getFactory() {
        return mFactory;
    }
    
    virtual void _objectAddedToGraph( SceneGraph *graph ) {}
    virtual void _objectRemovedFromGraph( SceneGraph *graph ) {}
    
private:
    void markDirty() {
        mDirty = true;
    }
    
private:
    SceneNode *mParent = nullptr;
    SceneObjectFactory *mFactory;
    
    glm::vec3 mPosition;
    glm::quat mOrientation;
    
    glm::mat4 mTransform;
    
    unsigned int mRenderQueue = 0;
    bool mDirty = false, mAutoDelete = false;
    BoundingSphere mBoundingSphere,
                   mTransformedBoundingSphere;
};