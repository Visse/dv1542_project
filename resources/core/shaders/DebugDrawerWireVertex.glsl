#version 430

layout(std140) uniform SceneInfo {
    mat4 ViewMatrix, 
         ProjectionMatrix, 
         ViewProjMatrix;
};
uniform mat4 ModelMatrix;

in vec3 Position;

void main()
{
    gl_Position = ViewProjMatrix * ModelMatrix * vec4(Position,1.0);
}