#version 430

layout(points) in;
layout(line_strip, max_vertices = 6) out;


layout(std140) uniform SceneInfo {
    mat4 ViewMatrix, 
         ProjectionMatrix, 
         ViewProjMatrix;
};
uniform mat4 ModelMatrix;

uniform vec3 NormalColor,
             TangentColor,
             BitangentColor;

uniform float Length;

in vec3 wNormal[],
        wTangent[],
        wBitangent[];
         
out vec3 color;        
        

void main()
{
    mat4 modelViewProj = ViewProjMatrix * ModelMatrix;
    
    { // Normal
        gl_Position =  modelViewProj * gl_in[0].gl_Position;
        color = NormalColor;
        EmitVertex();
        
        gl_Position = modelViewProj * (gl_in[0].gl_Position + vec4(wNormal[0] * Length,0));
        color = NormalColor;
        EmitVertex();
        
        EndPrimitive(); 
    }
    
    { // Tangent
        gl_Position =  modelViewProj * gl_in[0].gl_Position;
        color = TangentColor;
        EmitVertex();
        
        gl_Position = modelViewProj * (gl_in[0].gl_Position + vec4(wTangent[0] * Length,0));
        color = TangentColor;
        EmitVertex();
        
        EndPrimitive(); 
    }
    
    { // Bitangent
        gl_Position =  modelViewProj * gl_in[0].gl_Position;
        color = BitangentColor;
        EmitVertex();
        
        gl_Position = modelViewProj * (gl_in[0].gl_Position + vec4(wBitangent[0] * Length,0));
        color = BitangentColor;
        EmitVertex();

        EndPrimitive(); 
    }
}