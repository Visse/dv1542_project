#pragma once

#include <vector>

#include "BoundingSphere.h"

class SceneGraph;
class SceneObject;

class SceneNode {
public:
    ~SceneNode();
    
    void _init( SceneGraph *graph, SceneNode *parent ) {
        mGraph = graph;
        mParent = parent;
    }
    
    void addObject( SceneObject *object );
    void removeObject( SceneObject *object );
    
    void update( float dt );
    
    SceneNode* getParent() {
        return mParent;
    }
    SceneNode* getChildren() {
        return mChildren;
    }
    const BoundingSphere& getBounds() {
        return mBounds;
    }
    
    const std::vector<SceneObject*>& getObjects() {
        return mObjects;
    }
    
    // internal
    void _setChildren( SceneNode *children ) {
        mChildren = children;
    }
    void _setBounds( const BoundingSphere &bounds ) {
        mBounds = bounds;
    }
    
    
private:
    SceneGraph *mGraph;
    // children must either be null, or pointing to an array with 8 entries
    SceneNode *mParent, *mChildren = nullptr;
    
    std::vector<SceneObject*> mObjects;
    BoundingSphere mBounds;
};