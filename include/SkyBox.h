#pragma once

#include "SceneObject.h"
#include "SharedPtr.h"

class Root;
class Material;
class Mesh;

class SkyBox :
    public SceneObject
{
public:
    SkyBox( Root *root, const SharedPtr<Material> &material );
    
    virtual void queueRenderable( LowLevelRenderer &renderer );
    
private:
    Root *mRoot;
    SharedPtr<Mesh> mMesh;
    SharedPtr<Material> mMaterial;
};