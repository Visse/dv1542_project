#version 430


in vec3 Position;
in vec3 Normal;
in vec3 Tangent;
in vec3 Bitangent;

out vec3 wNormal,
         wTangent,
         wBitangent;


void main()
{
    gl_Position = vec4(Position,1.0);
    wNormal = Normal;
    wTangent = Tangent;
    wBitangent = Bitangent;
}