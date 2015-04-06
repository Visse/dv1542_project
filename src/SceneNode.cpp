#include "SceneNode.h"
#include "SceneObject.h"
#include "SceneGraph.h"
#include "SceneObjectFactory.h"

#include <algorithm>
#include <cassert>

void SceneNode::_destroy()
{
    for( ObjectInfo &info: mObjects ) {
        if( info.isDead ) continue;
        SceneObject *object = info.object;
        object->_setParent( nullptr );
        object->_objectRemovedFromGraph( mGraph );
        if( object->_getAutoDelete() ) {
            object->getFactory()->destroyObject( object );
        }
    }
    mObjects.clear();
}

void SceneNode::addObject( SceneObject *object )
{
    auto iter = std::lower_bound( mObjects.begin(), mObjects.end(), object );
    ObjectInfo info;
        info.object = object;
        info.isDead = false;
    mObjects.insert( iter, info );
}

void SceneNode::removeObject( SceneObject *object )
{
    auto iter = std::lower_bound( mObjects.begin(), mObjects.end(), object );
    assert( iter != mObjects.end() );
    assert( iter->isDead == false );
    
    iter->isDead = true;
    mDeadObjectCount++;
}

void SceneNode::update( float dt )
{
    for( ObjectInfo &info : mObjects ) {
        if( info.isDead ) continue;
        SceneObject *object = info.object;
        object->update(dt);
        if( object->isDirty() ) {
            mGraph->markObjectAsDirty( object );
        }
    }
    
    mObjects.erase( 
        std::remove_if( mObjects.begin(), mObjects.end(), [](const ObjectInfo &info ) {
            return info.isDead;
        } ),
        mObjects.end()
    );
    mDeadObjectCount = 0;
}
