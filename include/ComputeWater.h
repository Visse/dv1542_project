#pragma once


#include "SceneObject.h"
#include "SharedPtr.h"

class Root;
class Texture;
class GpuBuffer;
class GpuProgram;
class VertexArrayObject;


class ComputeWater :
    public SceneObject
{
public:
    ComputeWater( SceneObjectFactory *factory, Root *root );
    virtual ~ComputeWater();
    
    virtual void update( float dt );
    virtual void submitRenderer( Renderer &renderer );
    
    SharedPtr<Texture> getSimTexture() {
        return mSimTexture;
    }
    
    glm::vec3 getWaterColor() {
        return mWaterColor;
    }
    bool getUseWireFrame() {
        return mUseWireFrame;
    }
    float getDepthFalloff() {
        return mDepthFalloff;
    }
    float getHeightScale() {
        return mHeightScale;
    }
    float getWaterSizeScale() {
        return mWaterSizeScale;
    }
    float getLODScale() {
        return mLODScale;
    }
    float getFrensel() {
        return mFrensel;
    }
    float getFrenselFalloff() {
        return mFrenselFalloff;
    }
    glm::vec3 getLightPosition() {
        return mLightPosition;
    }
    glm::vec3 getLightColor() {
        return mLightColor;
    }
    
    void setWaterColor( const glm::vec3 &color ) {
        mWaterColor = color;
    }
    void setUseWireFrame( bool useWireFrame ) {
        mUseWireFrame = useWireFrame;
    }
    void setDepthFalloff( float depthFalloff ) {
        mDepthFalloff = depthFalloff;
    }
    void setHeightScale( float heightScale ) {
        mHeightScale = heightScale;
    }
    void setWaterSizeScale( float waterSizeScale );
    void setLODScale( float lodScale ) {
        mLODScale = lodScale;
    }
    void setFrensel( float frensel ) {
        mFrensel = frensel;
    }
    void setFrenselFalloff( float frenselFalloff ) {
        mFrenselFalloff = frenselFalloff;
    }
    void setLightPosition( const glm::vec3 &lightPosition ) {
        mLightPosition = lightPosition;
    }
    void setLightColor( const glm::vec3 &lightColor ) {
        mLightColor = lightColor;
    }
    
private:
    void render( Renderer &renderer );
    void createSurface();
    
private:
    struct WaterRenderable;
    
    struct SimUniforms {
    };
    struct RenderingUniforms {
        glm::mat4 modelMatrix;
        float depthFalloff,
              heightScale;
        glm::vec2 scrollDirection;
        float currentTime, lodScale, 
              frensel, frenselFalloff;
        glm::vec3 waterColor;
        float padding0;
        glm::vec3 lightPos;
        float padding1;
        glm::vec3 lightColor;
        float padding2;
    };
    
private:
    Root *mRoot;
    WaterRenderable *mRenderable;
    
    SharedPtr<VertexArrayObject> mVAO;
    SharedPtr<GpuBuffer> mWaterSurface;
    SharedPtr<Texture> mSimTexture,
                       mNormalTexture;
    SharedPtr<GpuProgram> mWaterSimulation,
                          mWaterShader,
                          mWaterWireShader;
    
    bool mUseWireFrame = false;
    
    float mCurrentTime = 0.f;
    float mDepthFalloff = 0.03f,
          mHeightScale = 0.2f,
          mWaterSizeScale = 1.f,
          mLODScale = 1.0f,
          mFrensel = 0.8f,
          mFrenselFalloff = 5.0f;
    glm::vec3 mWaterColor = glm::vec3(0.098,0.145,0.243);
    glm::vec3 mLightPosition,
              mLightColor;
}; 


