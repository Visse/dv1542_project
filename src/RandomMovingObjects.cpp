#include "RandomMovingObjects.h"
#include "Root.h"
#include "SceneNode.h"
#include "SceneGraph.h"
#include "SceneObjectFactory.h"

#include <glm/gtc/random.hpp>

RandomMovingObjects::RandomMovingObjects( SceneObjectFactory *factory, Root *root ) :
    SceneObject( factory ),
    mRoot(root)
{
    setBoundingSphere( BoundingSphere(glm::vec3(), mRadius) );
}

RandomMovingObjects::~RandomMovingObjects()
{
    setObjectCount( 0 );
    mObjectTemplate->getFactory()->destroyObject( mObjectTemplate );
}

void RandomMovingObjects::update( float dt )
{
    mCurrentTime += dt * mTimeMultiplier;
    
    float index = 0;
    for( ObjectInfo &info : mObjects ) {
        info.position = glm::vec3( 
            glm::sin( mCurrentTime*(index+2)*0.06f+index*0.2f)*glm::cos( mCurrentTime*(index+7)*0.04f ) * mRadius,
            glm::sin( mCurrentTime*(index+3)*0.04f+index*0.5f)*glm::sin( mCurrentTime*(index+5)*0.02f ) * mRadius,
            glm::cos( mCurrentTime*(index+5)*0.02f+index*0.8f)*glm::cos( mCurrentTime*(index+9)*0.06f ) * mRadius
        );
        
        index += 1.0;
    }
    glm::vec3 position = getPosition();
    for( ObjectInfo &info : mObjects ) {
        info.object->setPosition( info.position + position );
    }
}

void RandomMovingObjects::setObjectCount( unsigned int objectCount )
{
    removeObjectsFromGraph();
    
    unsigned int prevCount = mObjects.size();
    
    for( unsigned int i=objectCount; i < prevCount; ++i ) {
        SceneObject *object = mObjects[i].object;
        object->getFactory()->destroyObject( object );
    }
    mObjects.resize( objectCount );
    
    for( unsigned int i=prevCount; i < objectCount; ++i ) {
        SceneObject *object = mObjectTemplate->clone();
        mObjects[i].object= object;
    }
    
    mObjectCount = objectCount;
    addObjectsToGraph();
}

void RandomMovingObjects::_objectAddedToGraph( SceneGraph *graph )
{
    mGraph = graph;
    addObjectsToGraph();
}

void RandomMovingObjects::_objectRemovedFromGraph( SceneGraph *graph )
{
    removeObjectsFromGraph();
    mGraph = nullptr;
}

void RandomMovingObjects::addObjectsToGraph()
{
    if( mGraph ) {
        for( ObjectInfo &info : mObjects ) {
            mGraph->addObject( info.object );
        }
    }
}

void RandomMovingObjects::removeObjectsFromGraph()
{
    if( mGraph ) {
        for( ObjectInfo &info : mObjects ) {
            mGraph->removeObject( info.object );
        }
    }
}



