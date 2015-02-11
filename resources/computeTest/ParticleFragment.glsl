#version 430 core

uniform float Intensity;

out vec4 color;

in float lifeTime;

void main()
{
    float fade = min(2-abs(lifeTime*2-1)*2,1);
    color = mix( 
        vec4(0.0,  0.1, 0.2, 1.0),
        vec4(0.2, 0.05, 0.0, 1.0),
        lifeTime
    ) * fade * Intensity;
    // make the point less square and more round :)
    color *= min( (1-length(vec2(1)-gl_PointCoord*2))*4, 1.0 );
}