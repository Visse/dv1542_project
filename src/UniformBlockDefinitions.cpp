#include "UniformBlockDefinitions.h"
#include "UniformBlock.h"

#include <cstddef>

#define SetUniform( name, type, member ) \
            uniforms.setUniform( name, UniformType::type, offsetof(UNIFORM_BLOCK,member) )

const UniformBlockLayout& SceneRenderUniforms::GetUniformBlockLayout()
{
    struct Block {
        UniformBlockLayout uniforms;
        Block() 
        {
            static const size_t ExpectedSize = 
                                    4*4 * 6 + // we have 6 mat4
                                    2*1 + // 1 vec2
                                    3*1 + // 1 vec3
                                    3; // allignment dummies
                                        
            // just to make sure we don't forget to update this function
            static_assert( sizeof(SceneRenderUniforms) == ExpectedSize *4, "Fix me!" );
            
            uniforms.setSize( sizeof(SceneRenderUniforms) );
                
#define UNIFORM_BLOCK SceneRenderUniforms
            SetUniform( "ViewMatrix", Mat4, viewMatrix );
            SetUniform( "ProjectionMatrix", Mat4, projectionMatrix );
            SetUniform( "ViewProjMatrix", Mat4, viewProjMatrix );
            
            SetUniform( "InverseViewMatrix", Mat4, inverseViewMatrix );
            SetUniform( "InverseProjectionMatrix", Mat4, inverseProjectionMatrix );
            SetUniform( "InverseViewProjMatrix", Mat4, inverseViewProjMatrix );
            
            SetUniform( "ClippingPlanes", Vec2, clippingPlanes );
            SetUniform( "CameraPosition", Vec3, cameraPosition );
#undef UNIFORM_BLOCK
            
        }
    };

    static Block block;
    return block.uniforms;
}

const UniformBlockLayout& EntityUniforms::GetUniformBlockLayout()
{
    struct Block {
        UniformBlockLayout uniforms;
        Block() 
        {
            static const size_t ExpectedSize = 4*4;
                                        
            // just to make sure we don't forget to update this function
            static_assert( sizeof(EntityUniforms) == ExpectedSize *4, "Fix me!" );
            
            uniforms.setSize( sizeof(EntityUniforms) );
            
                
#define UNIFORM_BLOCK EntityUniforms
            SetUniform( "ModelMatrix", Mat4, modelMatrix );
#undef UNIFORM_BLOCK
        }
    };

    static Block block;
    return block.uniforms;
}

const UniformBlockLayout &PointLightUniforms::GetUniformBlockLayout()
{
    struct Block {
        UniformBlockLayout uniforms;
        Block() 
        {
            static const size_t ExpectedSize = 
                                          4*4 + // modelMatrix
                                          4*2; // color & radius
                                        
            // just to make sure we don't forget to update this function
            static_assert( sizeof(PointLightUniforms) == ExpectedSize *4, "Fix me!" );
            
            uniforms.setSize( sizeof(PointLightUniforms) );
            
                
#define UNIFORM_BLOCK PointLightUniforms
            SetUniform( "ModelMatrix", Mat4, modelMatrix );
            SetUniform( "Color", Vec4, color );
            SetUniform( "Radius", Vec2, radius );
#undef UNIFORM_BLOCK
        }
    };

    static Block block;
    return block.uniforms;
}
