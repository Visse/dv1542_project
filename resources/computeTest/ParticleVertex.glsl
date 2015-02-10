#version 430 core

layout(std140) uniform SceneInfo {
    mat4 ViewMatrix, 
         ProjectionMatrix, 
         ViewProjMatrix;
};

uniform mat4 ModelMatrix; 

uniform float PointSize;

in vec4 particle;
out float lifeTime;

void main()
{
    gl_Position = ViewProjMatrix * ModelMatrix * vec4(particle.xyz,1.0);
    lifeTime = particle.w;
    gl_PointSize = PointSize/gl_Position.w;
}

