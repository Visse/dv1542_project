#pragma once

#include <vector>

#include "BoundingSphere.h"

class SceneGraph;
class SceneObject;

class SceneNode {
    friend class SceneGraph;
public:
    
    void _init( SceneGraph *graph, SceneNode *parent ) {
        mGraph = graph;
        mParent = parent;
    }
    void _destroy();
    
    void addObject( SceneObject *object );
    void removeObject( SceneObject *object );
    
    void update( float dt );
    
    SceneGraph* getSceneGraph() {
        return mGraph;
    }
    SceneNode* getParent() {
        return mParent;
    }
    SceneNode* getChildren() {
        return mChildren;
    }
    const BoundingSphere& getBounds() {
        return mBounds;
    }
    
    // internal
    void _setChildren( SceneNode *children ) {
        mChildren = children;
    }
    void _setBounds( const BoundingSphere &bounds ) {
        mBounds = bounds;
    }
    
private:
    struct ObjectInfo {
        SceneObject *object = nullptr;
        bool isDead = false;
        
        friend bool operator < ( const ObjectInfo &i1, const ObjectInfo &i2 ) {
            return i1.object < i2.object;
        }
        friend bool operator < ( const ObjectInfo &i1, SceneObject *i2 ) {
            return i1.object < i2;
        }
        friend bool operator < ( SceneObject *i1, const ObjectInfo &i2 ) {
            return i1 < i2.object;
        }
    };
    
    const std::vector<ObjectInfo>& getObjects() {
        return mObjects;
    }
private:
    SceneGraph *mGraph;
    // children must either be null, or pointing to an array with 8 entries
    SceneNode *mParent, *mChildren = nullptr;
    
    std::vector<ObjectInfo> mObjects;
    unsigned int mDeadObjectCount = 0;
    BoundingSphere mBounds;
};