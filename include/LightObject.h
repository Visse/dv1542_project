#pragma once

#include "SceneObject.h"
#include "Renderable.h"
#include "SceneObjectFactory.h"
#include "SharedPtr.h"
#include "GLTypes.h"

#include <glm/vec3.hpp>

class Mesh;
class Material;
class PointLight;

class PointLightRenderer :
    public Renderable
{
public:
    void init( PointLight *light );
    virtual void render();
    
private:
    PointLight *mLight;
    GLuint mModelMatrixLoc,
           mOuterRadiusLoc,
           mInnerRadiusLoc,
           mColorLoc;
};

class PointLight :
    public SceneObject
{
    friend class PointLightRenderer;
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
    PointLightRenderer mRenderer;
    
    SharedPtr<Mesh> mMesh;
    SharedPtr<Material> mMaterial;
    
    float mOuterRadius = 1.f,
          mInnerRadius = 0.f;
    glm::vec3 mColor;
};

