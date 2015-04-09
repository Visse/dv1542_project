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
    glm::vec3 getLightPosition() {
        return mLightPosition;
    }
    glm::vec3 getLightColor() {
        return mLightColor;
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
        float currentTime, lodScale;
        float padding0[2];
        
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
                       mWaterTexture,
                       mNormalTexture;
    SharedPtr<GpuProgram> mWaterSimulation,
                          mWaterShader,
                          mWaterWireShader;
    
    bool mUseWireFrame = false;
    
    float mCurrentTime = 0.f;
    float mDepthFalloff = 0.03f,
          mHeightScale = 0.2f,
          mWaterSizeScale = 1.f,
          mLODScale = 1.0f;
          
    glm::vec3 mLightPosition,
              mLightColor;
}; 


