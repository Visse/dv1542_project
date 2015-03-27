#include "SceneGraph.h"
#include "SceneObject.h"
#include "Frustrum.h"

#include <glm/exponential.hpp>

#include <cassert>

SceneGraph::SceneGraph( Root *root, const BoundingSphere &rootBounds ) :
    mRoot(root)
{
    mRootNode.reset( new SceneNode );
    mRootNode->_init( this, nullptr );
   
    
    glm::vec3 center = rootBounds.getCenter();
    float radius = rootBounds.getRadius();
    
    // we need to be a power of 2
    float maxNodeLevel = glm::ceil( glm::log2(radius) );
    radius = glm::pow( 2.f, maxNodeLevel );
    
    mRootNode->_setBounds( BoundingSphere(center, radius) );
    mSceneNodes.push_back( mRootNode.get() );
    
    mMaxNodeLevel = ((int)maxNodeLevel) - 1;
    mRootPosition = rootBounds.getCenter();
}

void SceneGraph::addObject( SceneObject *object )
{
    assert( object->_getParent() == nullptr );
    
    SceneNode *parent = getOrCreateNodeForBound( object->getTransformedBoundingSphere() );
    parent->addObject( object );
    object->_setParent( parent );
}

void SceneGraph::removeObject( SceneObject *object )
{
    assert( object->_getParent() != nullptr );
    
    SceneNode *parent = object->_getParent();
    parent->removeObject( object );

    cleanEmptyNodes( parent );
}

void SceneGraph::update( float dt )
{
    for( SceneObject *object : mDirtyObjects ) {
        object->_updateTransform();
        const BoundingSphere &bounds = object->getTransformedBoundingSphere();
        
        SceneNode *parent = object->_getParent();
        SceneNode *newParent = getOrCreateNodeForBound( bounds );
        
        if( parent != newParent ) {
            parent->removeObject( object );
            newParent->addObject( object );
            object->_setParent( newParent );
            
            cleanEmptyNodes( parent );
        }
    }
    mDirtyObjects.clear();
    
    for( SceneNode *node : mSceneNodes ) {
        node->update( dt );
    }
}

void SceneGraph::forEachObject( const std::function<void(SceneObject*)> &callback )
{
    for( SceneNode *node : mSceneNodes ) {
        const auto &objects = node->getObjects();
        for( SceneObject *obj : objects ) {
            callback(obj);
        }
    }
}

void SceneGraph::quaryObjects( const Frustrum &frustrum, std::vector<SceneObject*> &result )
{
    quaryObjectsForNode( mRootNode.get(), frustrum, result );
}

void SceneGraph::quaryObjectsForNode( SceneNode *node, const Frustrum &frustrum, std::vector<SceneObject*> &output )
{
    auto result = frustrum.isInside(node->getBounds());
    
    switch( result ) {
    case( Frustrum::TestStatus::Outside ):
        return;
    case( Frustrum::TestStatus::Inside ):
        nodeFullyInsideFrustrum( node, output );
        break;
    case( Frustrum::TestStatus::Intersecting ):
        nodePartalyInsideFrusturm( node, frustrum, output );
        break;
    }
}

void SceneGraph::nodeFullyInsideFrustrum( SceneNode *node, std::vector<SceneObject*> &result )
{
    const auto &objects = node->getObjects();
    result.insert( result.end(), objects.begin(), objects.end() );
    
    SceneNode *children = node->getChildren();
    if( children ) {
        for( int i=0; i < 8; ++i ) {
            nodeFullyInsideFrustrum( &children[i], result );
        }
    }
}

void SceneGraph::nodePartalyInsideFrusturm( SceneNode *node, const Frustrum &frustrum, std::vector<SceneObject*> &result )
{
    const auto &objects = node->getObjects();
    for( SceneObject *object : objects ) {
        if( frustrum.isInside(object->getTransformedBoundingSphere()) != Frustrum::TestStatus::Outside ) {
            result.push_back( object );
        }
    }
    
    SceneNode *children = node->getChildren();
    if( children ) {
        for( int i=0; i < 8; ++i ) {
            quaryObjectsForNode( &children[i], frustrum, result );
        }
    }
}


SceneNode* SceneGraph::getOrCreateNodeForBound( const BoundingSphere &bounds )
{
    float radius = bounds.getRadius();
    glm::vec3 position = bounds.getCenter() - mRootPosition;
    
    int level = mMaxNodeLevel - mMinNodeLevel;
    if( radius > 0 ) {
        level = mMaxNodeLevel - glm::max<int>( glm::ceil(glm::log2(radius)), mMinNodeLevel );
    }
    if( radius == std::numeric_limits<float>::infinity() ) {
        level = 0;
    }
    
    SceneNode *root = mRootNode.get();
    
    while( level > 0 ) {
        glm::bvec3 greaterThan0 = glm::greaterThan( glm::sign(position), glm::vec3(0.f) );
        int index = greaterThan0.x | greaterThan0.y<<1 | greaterThan0.z<<2;
        
        SceneNode *children = root->getChildren();
        if( !children ) {
            children = createChildrenForNode( root );
        }
        
        position -= children[index].getBounds().getCenter() - root->getBounds().getCenter();
//         position /= 2.f;
        
        root = &children[index];
        level--;
    }
    
    return root;
}

SceneNode* SceneGraph::createChildrenForNode( SceneNode *node )
{
    assert( node->getChildren() == nullptr );
    
    BoundingSphere bounds = node->getBounds();
    
    /// In order to fast determenate that children is releative to its parent,
    /// we are expecting a certan order to then:
    
    glm::vec3 offets[8] {
        {-1.f,-1.f,-1.f},
        { 1.f,-1.f,-1.f},
        {-1.f, 1.f,-1.f},
        { 1.f, 1.f,-1.f},
        {-1.f,-1.f, 1.f},
        { 1.f,-1.f, 1.f},
        {-1.f, 1.f, 1.f},
        { 1.f, 1.f, 1.f}
    };
    
    
    glm::vec3 position = bounds.getCenter();
    float hradius = bounds.getRadius() / 2.f;

    float hsize = hradius*glm::one_over_root_two<float>()/2.f;
    
    
    SceneNode *children = new SceneNode[8];
    mChildBlocks.emplace_back( children );
    
    
    for( int i=0; i < 8; ++i ) {
        children[i]._init( this, node );
        children[i]._setBounds( BoundingSphere(position+offets[i]*hsize, hradius) );
        mSceneNodes.push_back( &children[i] );
    }
    node->_setChildren( children );
    
    return children;
}


void SceneGraph::cleanEmptyNodes( SceneNode *node )
{
    SceneNode *parent = node->getParent();

    if( parent ) {
        SceneNode *children = parent->getChildren();
        assert( children != nullptr );

        bool isEmpty = true;
        for( int i=0; i < 8; ++i ) {
            if( children[i].getChildren() != nullptr) {
                isEmpty = false;
                break;
            }
            if( children[i].getObjects().empty() == false ) {
                isEmpty = false;
                break;
            }
        }

        if( isEmpty ) {
            parent->_setChildren( nullptr );
            cleanEmptyNodes( parent );
           // delete[] children;
        }
    }
}
