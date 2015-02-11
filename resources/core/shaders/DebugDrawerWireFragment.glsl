#version 430

uniform vec3 Color;

out vec3 color;


uniform sampler2D DepthTexture;

void main()
{
//     vec3 pos = gl_FragCoord.xyz / gl_FragCoord.w;
//     vec2 texcoord = (gl_FragCoord.xy+1)/2;
    
    float depth = texture2D( DepthTexture, gl_SamplePosition );
//     if( depth < gl_FragCoord.z/gl_FragColor.w ) discard;
    
    color = Color;
}