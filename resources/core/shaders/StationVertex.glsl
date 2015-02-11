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

uniform mat4 ModelMatrix; 

in vec3 Position;
in vec3 Normal;
in vec3 Tangent;
in vec3 Bitangent;
in vec2 TexCoord;

out FragmentData {
    vec3 normal, tangent, bitangent;
    vec3 position;
    vec2 texCoord;
} frag;

void main()
{
    gl_Position = ViewProjMatrix * ModelMatrix * vec4(Position,1.0);
    
    mat3 modelMatrix = mat3(ModelMatrix);
    
    frag.normal    = modelMatrix * normalize(Normal);
    frag.tangent   = modelMatrix * normalize(Tangent);
    frag.bitangent = modelMatrix * normalize(Bitangent);
    frag.texCoord = TexCoord;
    frag.position =  vec3(ModelMatrix * vec4(Position,1.0));
}
