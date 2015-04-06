#pragma once

#include "DeferredEntity.h"

class PulsingObject :
    public DeferredEntity
{
public:
    PulsingObject( SceneObjectFactory* factory, Root* root, const SharedPtr< Mesh >& mesh, const DeferredMaterial& material );
    virtual void update( float dt ) override;
    
    virtual void submitRenderer( Renderer &renderer );
    virtual void submitShadowCasters( Renderer &renderer );
    
    void setMaxScale( float maxScale ) {
        mMaxScale = maxScale;
    }
    void setMinScale( float minScale ) {
        mMinScale = minScale;
    }
    
    float getMaxScale() {
        return mMaxScale;
    }
    float getMinScale() {
        return mMinScale;
    }
    
private:
    float mMaxScale = 2.f,
          mMinScale = 0.5f,
          mOffset = 0.f,
          mScale = 1.f;
};