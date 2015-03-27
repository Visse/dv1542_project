#pragma once

#include "SharedPtr.h"

#include <string>

class Root;
class Scene;

class SceneLoader {
public:
    SceneLoader( Root *root );
    
    void loadFile( const std::string &filename );
    
    SharedPtr<Scene> getScene() {
        return mScene;
    }
    
private:
    Root *mRoot;
    SharedPtr<Scene> mScene;
};