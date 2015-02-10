#version 430 core

out vec4 color;

void main()
{
    color = vec4(0.5,1.0,0.2,1.0) * 0.4;
    // make the point less square and more round :)
    color *= min( (1-length(vec2(1)-gl_PointCoord*2))*4, 1.0 );
}