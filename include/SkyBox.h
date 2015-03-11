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
    SkyBox( SceneObjectFactory *factory, Root *root, const SharedPtr<Material> &material );
    
//     virtual void queueRenderable( LowLevelRenderer &renderer ) override;
    
private:
    Root *mRoot;
    SharedPtr<Mesh> mMesh;
    SharedPtr<Material> mMaterial;
};