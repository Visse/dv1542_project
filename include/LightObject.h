#pragma once

#include "SceneObject.h"
#include "SceneObjectFactory.h"
#include "SharedPtr.h"
#include "GLTypes.h"

#include <glm/vec3.hpp>

class Mesh;
class Material;
class PointLight;

class PointLight :
    public SceneObject
{
public:
    PointLight( Root *root );
    virtual void queueRenderable( LowLevelRenderer& renderer );
    
    void setColor( const glm::vec3 &color ) {
        mColor = color;
    }
    void setOuterRadius( float radius ) {
        mOuterRadius = radius;
    }
    void setInnerRadius( float radius ) {
        mInnerRadius = radius;
    }
    
    const glm::vec3& getColor() {
        return mColor;
    }
    float getOuterRadius() {
        return mOuterRadius;
    }
    float getInnerRadius() {
        return mInnerRadius;
    }
    
private:
    Root *mRoot;
    
    SharedPtr<Mesh> mMesh;
    SharedPtr<Material> mMaterial;
    
    float mOuterRadius = 1.f,
          mInnerRadius = 0.f;
    glm::vec3 mColor;
    
    size_t mBlockLoc;
};

