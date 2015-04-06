#pragma once

#include "SceneObject.h"

#include <vector>

class Root;
class Texture;

class RandomMovingObjects :
    public SceneObject
{
public:
    RandomMovingObjects( SceneObjectFactory *factory, Root *root );
    virtual ~RandomMovingObjects();
    
    virtual void update( float dt ) override;
    
    unsigned int getObjectCount() {
        return mObjectCount;
    }
    float getTimeMultipler() {
        return mTimeMultiplier;
    }
    float getRadius() {
        return mRadius;
    }
    
    void setObjectCount( unsigned int objectCount );
    void setTimeMultipler( float timeMultiplier ) {
        mTimeMultiplier = timeMultiplier;
    }
    void setRadius( float radius ) {
        mRadius = radius;
    }
    
    void setTemplate( SceneObject *object ) {
        mObjectTemplate = object;
    }
    
    SceneObject* getTemplate() {
        return mObjectTemplate;
    }
    
    virtual void _objectAddedToGraph( SceneGraph *graph );
    virtual void _objectRemovedFromGraph( SceneGraph *graph );
    
private:
    void addObjectsToGraph();
    void removeObjectsFromGraph();
    
private:
    
    struct ObjectInfo {
        glm::vec3 position;
        SceneObject *object;
    };
    
private:
    Root *mRoot;
    SceneGraph *mGraph = nullptr;
    SceneObject *mObjectTemplate = nullptr;
    
    unsigned int mObjectCount = 0;
    
    std::vector<ObjectInfo> mObjects;
                       
    float mCurrentTime = 10.f,
          mTimeMultiplier = 1.0f,
          mRadius = 1.0f;
};