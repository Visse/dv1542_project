#pragma once

#include <vector>
#include <functional>

#include <glm/vec3.hpp>

#include "SharedPtr.h"

class Root;
class Frustrum;
class SceneObject;

class Scene {
public:
    static SharedPtr<Scene> LoadFromFile( Root *root, const std::string &filename );
    
public:
    Scene( Root *root );
    ~Scene();
    
    Scene( const Scene& ) = delete;
    Scene( Scene&& ) = delete;
    Scene& operator = ( const Scene& ) = delete;
    Scene& operator = ( Scene&& ) = delete;
    
    void addObject( SceneObject *object, bool takeOwnership );
    void removeObject( SceneObject *object );
    
    void update( float dt );
    
    void quarySceneObjects( const Frustrum &frustrum, std::vector<SceneObject*> &result );
    
    void forEachObject( std::function<void(SceneObject*)> callback );
    
private:
    struct ObjectInfo {
        SceneObject *object;
        
        bool ownsObject;
    };
    struct FindSceneInfo;
    
private:
    Root *mRoot;
    std::vector<ObjectInfo> mObjects;
};