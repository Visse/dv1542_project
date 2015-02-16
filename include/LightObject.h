#pragma once

#include "SceneObject.h"
#include "SceneObjectFactory.h"
#include "SharedPtr.h"
#include "GLTypes.h"

#include <glm/vec3.hpp>

class Mesh;
class Material;
class PointLight;


class LightObject :
    public SceneObject
{
public:
    void setColor( const glm::vec3 &color ) {
        mColor = color;
    }
    const glm::vec3& getColor() {
        return mColor;
    }
    
private:
    glm::vec3 mColor;
};

class PointLight :
    public LightObject
{
public:
    PointLight( Root *root );
    virtual void queueRenderable( LowLevelRenderer& renderer );
    
    void setOuterRadius( float radius ) {
        mOuterRadius = radius;
    }
    void setInnerRadius( float radius ) {
        mInnerRadius = radius;
    }
    void setIntensity( float intensity ) {
        mIntensity = intensity;
    }
    
    float getOuterRadius() {
        return mOuterRadius;
    }
    float getInnerRadius() {
        return mInnerRadius;
    }
    float getIntensity() {
        return mIntensity;
    }
    
private:
    Root *mRoot;
    
    SharedPtr<Mesh> mMesh;
    SharedPtr<Material> mMaterial;
    
    float mOuterRadius = 1.f,
          mInnerRadius = 0.f;
    
    float mIntensity = 0.5f;
          
    size_t mBlockLoc;
};

class AmbientLight :
    public LightObject
{
public:
    AmbientLight( Root *root );
    virtual void queueRenderable(LowLevelRenderer& renderer);

private:
    Root *mRoot;
    SharedPtr<Material> mMaterial;
    
    size_t mBlockLoc;
};