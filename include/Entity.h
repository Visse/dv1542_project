#pragma once

#include "SceneObject.h"
#include "SharedPtr.h"
#include "Renderable.h"
#include "GLTypes.h"

#include <vector>

struct SubMesh;
class Mesh;
class Entity;

class SubMeshRenderer :
    public Renderable
{
public:
    SubMeshRenderer( SceneObject *object, const SharedPtr<Mesh> &mesh, const SubMesh *submesh );
    
    virtual void render();
    
    const SubMesh* getSubMesh();
    
private:
    SceneObject *mObject;
    SharedPtr<Mesh> mMesh;
    const SubMesh *mSubMesh;
    
    GLint mModelMatrixLoc;
};


class Entity :
    public SceneObject
{
    friend class SubMeshRenderer;
    
public:
    Entity( const SharedPtr<Mesh> &mesh );
    
    virtual void queueRenderable( LowLevelRenderer& renderer );
    
    SharedPtr<Mesh> getMesh() {
        return mMesh;
    }
    
private:
    SharedPtr<Mesh> mMesh;
    std::vector<SubMeshRenderer> mRenderers;
};