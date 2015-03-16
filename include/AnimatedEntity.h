#include "SceneObject.h"


#include "Material.h"

class Mesh;

class AnimatedEntity :
    public SceneObject
{
public:
    AnimatedEntity( SceneObjectFactory* factory, const SharedPtr<Mesh> &mesh, const DeferredMaterial &material );
    
    virtual void submitRenderer( Renderer& renderer ) override;
    virtual void submitShadowCasters(Renderer& renderer) override;
    
    void setCastShadow( bool castShadow ) {
        mCastShadow = castShadow;
    }
    bool getCastShadow() {
        return mCastShadow;
    }
    
private:
    SharedPtr<Mesh> mMesh;
    DeferredMaterial mMaterial;
    
    bool mCastShadow = true;
    
};