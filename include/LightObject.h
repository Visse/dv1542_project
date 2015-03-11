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
    void setCastShadow( bool castShadow ) {
        mCastShadow = castShadow;
    }
    
    const glm::vec3& getColor() {
        return mColor;
    }
    bool getCastShadow() {
        return mCastShadow;
    }
    
private:
    glm::vec3 mColor;
    bool mCastShadow = false;
};

class PointLight :
    public LightObject
{
public:
    PointLight( Root *root );
    
    void setOuterRadius( float radius ) {
        BoundingSphere bounds( glm::vec3(), radius );
        setBoundingSphere( bounds );
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
    
    virtual void submitRenderer( Renderer &renderer );
    
private:
    Root *mRoot;
    
    float mOuterRadius = 1.f,
          mInnerRadius = 0.f;
    
    float mIntensity = 0.5f;
    
};

class SpotLight :
    public LightObject
{
public:
    SpotLight( Root *root );
    
    float getIntensity() {
        return mIntensity;
    }
    float getInnerAngle() {
        return mInnerAngle;
    }
    float getOuterAngle() {
        return mOuterAngle;
    }
    float getInnerDistance() {
        return mInnerDistance;
    }
    float getOuterDistance() {
        return mOuterDistance;
    }
    
    void setIntensity( float intensity ) {
        mIntensity = intensity;
    }
    void setInnerAngle( float angle ) {
        mInnerAngle = angle;
    }
    void setOuterAngle( float angle ) {
        mOuterAngle = angle;
        updateBounds();
    }
    void setInnerDistance( float dist ) {
        mInnerDistance = dist;
    }
    void setOuterDistance( float dist ) {
        mOuterDistance = dist;
        updateBounds();
    }
    
private:
    void updateBounds();
    
private:
    Root *mRoot;
    
    SharedPtr<Mesh> mMesh;
    SharedPtr<Material> mMaterial;
    
    size_t mBlockLoc;
    
    float mIntensity = 0.5f;
    // inner & outer angle dictates how the light falls of towards the 'edge' 
    // inner & outer distance dictates how the light falls of with distance
    float mInnerAngle = 0.392699082, mOuterAngle = 0.785398163, // 22.5 & 45 degrees
          mInnerDistance = 0.5f, mOuterDistance = 1.f;
};
