#pragma once

#include "SceneObject.h"
#include "SharedPtr.h"
#include "GLTypes.h"

#include <vector>

struct SubMesh;
class Mesh;
class Root;
class Material;

class Entity :
    public SceneObject
{
    friend class SubMeshRenderer;
    
public:
    Entity( Root *root, const SharedPtr<Mesh> &mesh );
    
    virtual void queueRenderable( LowLevelRenderer& renderer );
    
    SharedPtr<Mesh> getMesh() {
        return mMesh;
    }
    
private:
    struct SubMeshInfo {
        unsigned int blockLoc;
        size_t vertexStart, vertexCount;
        SharedPtr<Material> material;
    };
    
private:
    Root *mRoot;
    SharedPtr<Mesh> mMesh;
    size_t mModelMatrixOffset, mBlockIndex;
    
    std::vector<SubMeshInfo> mSubMeshes;
    
};