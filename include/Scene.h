#pragma once

#include <vector>
#include <deque>
#include <functional>

#include <glm/vec3.hpp>

#include "SharedPtr.h"
#include "UniquePtr.h"

class Root;
class Frustrum;
class SceneObject;
class SceneGraph;

class Scene {
public:
    static SharedPtr<Scene> LoadFromFile( Root *root, const std::string &filename );
    
public:
    Scene( Root *root );
    
    Scene( const Scene& ) = delete;
    Scene( Scene&& ) = delete;
    Scene& operator = ( const Scene& ) = delete;
    Scene& operator = ( Scene&& ) = delete;
    
    void addObject( SceneObject *object, bool takeOwnership );
    void removeObject( SceneObject *object );
    
    void update( float dt );
    
    void quarySceneObjects( const Frustrum &frustrum, std::vector<SceneObject*> &result );
    
    void forEachObject( const std::function<void(SceneObject*)> &callback );
    
    SceneGraph* getSceneGraph() {
        return mSceneGraph.get();
    }
    
private:
    Root *mRoot;
    UniquePtr<SceneGraph> mSceneGraph;
};