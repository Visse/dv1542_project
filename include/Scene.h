#pragma once

#include <vector>
#include <deque>
#include <functional>

#include <glm/vec3.hpp>

#include "SharedPtr.h"
#include "UniquePtr.h"
#include "BoundingSphere.h"

class Texture;
class Root;
class Frustrum;
class SceneObject;
class SceneGraph;

struct AmbientUniforms;

class Scene {
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
    AmbientUniforms getAmbientUniforms();
    
    
    glm::vec3 getAmbientColor() {
        return mAmbientColor;
    }
    void setAmbientColor( const glm::vec3 &ambient ) {
        mAmbientColor = ambient;
    }
    
    void setSkyBox( const SharedPtr<Texture> &skybox ) {
        mSkyBox = skybox;
    }
    SharedPtr<Texture> getSkyBox() {
        return mSkyBox;
    }
    
    void setUseFrustrumCulling( bool useFrustrumCulling ) {
        mUseFrustumCulling = useFrustrumCulling;
    }
    void setPaused( bool paused ) {
        mPaused = paused;
    }
    bool getUseFrustrumCulling() {
        return mUseFrustumCulling;
    }
    bool getPaused() {
        return mPaused;
    }
    
private:
    typedef unsigned int ZoneID;
    struct Portal {
        BoundingSphere bounds;
        glm::vec3 pos;
        ZoneID targetZone;
    };
    struct ZoneInfo {
        std::vector<Portal> portals;
        UniquePtr<SceneGraph> sceneGraph;
    };
    
private:
    Root *mRoot;
    UniquePtr<SceneGraph> mSceneGraph;
    
    glm::vec3 mAmbientColor = glm::vec3(0.5);
    SharedPtr<Texture> mSkyBox;
    
    bool mUseFrustumCulling = true,
         mPaused = false;
};