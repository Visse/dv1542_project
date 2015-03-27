#include "SceneNode.h"
#include "UniquePtr.h"

#include <vector>
#include <functional>

class Root;
class Frustrum;

class SceneGraph {
public:
    SceneGraph( Root *root, const BoundingSphere &rootBounds = BoundingSphere(glm::vec3(0,0,0),128.f) );
    
    void addObject( SceneObject *object );
    void removeObject( SceneObject *object );
    void markObjectAsDirty( SceneObject *object ) {
        mDirtyObjects.push_back( object );
    }
    
    void update( float dt );
    
    void forEachObject( const std::function<void(SceneObject*)> &callback );
    void quaryObjects( const Frustrum &frustrum, std::vector<SceneObject*> &result );
    
    SceneNode* getRootNode() {
        return mRootNode.get();
    }
    
private:
    SceneNode* getOrCreateNodeForBound( const BoundingSphere &bounds );
    SceneNode* createChildrenForNode( SceneNode *node );
    void cleanEmptyNodes( SceneNode *node );
    
    void quaryObjectsForNode( SceneNode *node, const Frustrum &frustrum, std::vector<SceneObject*> &result );
    void nodeFullyInsideFrustrum( SceneNode *node, std::vector<SceneObject*> &result );
    void nodePartalyInsideFrusturm( SceneNode *node, const Frustrum &frustrum, std::vector<SceneObject*> &result );
    
private:
    typedef UniquePtr<SceneNode> SceneNodePtr;
    
    struct SceneNodeBlockDelete {
        void operator () ( SceneNode *node ) {
            delete[] node;
        }
    };
    typedef std::unique_ptr<SceneNode,SceneNodeBlockDelete> SceneNodeBlockPtr;
    
private:
    Root *mRoot;
    SceneNodePtr mRootNode;
    glm::vec3 mRootPosition;
    std::vector<SceneNode*> mSceneNodes;
    std::vector<SceneNodeBlockPtr> mChildBlocks;
    
    std::vector<SceneObject*> mDirtyObjects;
    
    int mMinNodeLevel = -2,
        mMaxNodeLevel;
};