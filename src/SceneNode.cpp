#include "SceneNode.h"
#include "SceneObject.h"
#include "SceneGraph.h"

#include <algorithm>
#include <cassert>

SceneNode::~SceneNode()
{
    for( SceneObject *object : mObjects ) {
        if( object->_getAutoDelete() ) {
            delete object;
        }
    }
}

void SceneNode::addObject( SceneObject *object )
{
    auto iter = std::lower_bound( mObjects.begin(), mObjects.end(), object );
    mObjects.insert( iter, object );
}

void SceneNode::removeObject( SceneObject *object )
{
    auto iter = std::lower_bound( mObjects.begin(), mObjects.end(), object );
    assert( iter != mObjects.end() );
    mObjects.erase( iter );
}

void SceneNode::update( float dt )
{
    for( SceneObject *object : mObjects ) {
        if( object->isDirty() ) {
            mGraph->markObjectAsDirty( object );
        }
        object->update(dt);
    }
}
