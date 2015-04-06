#pragma once

#include "SceneObject.h"
#include "SharedPtr.h"
#include "GLTypes.h"
#include "Renderer.h"
#include "Material.h"

#include <vector>

struct SubMesh;
class Mesh;
class Root;
class Material;

class DeferredEntity :
    public SceneObject
{
    friend class SubMeshRenderer;
    
public:
    DeferredEntity( SceneObjectFactory* factory, Root *root, const SharedPtr<Mesh> &mesh, const DeferredMaterial &material );
    
    virtual void submitRenderer( Renderer& renderer ) override;
    virtual void submitShadowCasters(Renderer& renderer) override;
    
    SharedPtr<Mesh> getMesh() {
        return mMesh;
    }
    const DeferredMaterial& getMaterial() {
        return mMaterial;
    }

    void setCastShadow( bool castShadow ) {
        mCastShadow = castShadow;
    }
    bool getCastShadow() {
        return mCastShadow;
    }
    
private:
    Root *mRoot;
    SharedPtr<Mesh> mMesh;
    DeferredMaterial mMaterial;
    
    bool mCastShadow = true;
};