#version 430

// @SceneInfo version 1
layout(std140) uniform SceneInfo {
    mat4 ViewMatrix, 
         ProjectionMatrix, 
         ViewProjMatrix,
         
         InvertViewMatrix,
         InvertProjectionMatrix,
         InvertViewProjMatrix;
         
    vec2 clipPlanes;
};

in FragmentData {
    vec3 normal, tangent, bitangent;
    vec3 position;
    vec2 texCoord;
} vert;

out vec4 Diffuse;
out vec3 Normal;
out vec3 Position;

uniform sampler2D DiffuseTexture;
uniform sampler2D NormalTexture;

void main()
{
    mat3 tangentToWorldSpace = (mat3(
        normalize(vert.tangent),  normalize(vert.bitangent), normalize(vert.normal)
    ));
    
    vec3 normalMap = (texture2D( NormalTexture, vert.texCoord ).xyz+1)/2;
    
    Diffuse = texture2D( DiffuseTexture, vert.texCoord );
    Normal  =  tangentToWorldSpace * normalMap;
    Position =  vert.position;
    
//     Normal  = (normalize(vert.tangent)+1) / 2;
//     Diffuse = texture2D( NormalTexture, vert.texCoord ) + 0.1;
//     Normal = (normalize(vert.normal)+1)/2;
}
