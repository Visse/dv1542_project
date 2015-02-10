#version 430 core


layout (local_size_x=64) in;

struct Particle {
    vec4 position;
    vec4 velosity;
};

layout(std140, binding=0) buffer ParticleBuffer {
    Particle particles[];
};
layout(std140, binding=1) buffer StartPositionsBuffer {
    vec3 startPositions[];
};
layout(std140, binding=2) buffer AttractorBuffer {
    vec4 attractors[];
};

uniform float dt;

uniform float weightMod, distMod, lifeTimeMod, dampingMod;

uniform mat4 ModelMatrix;

uniform uint AttractorCount;

void main()
{
//     uint index = gl_GlobalInvocationID.z * gl_WorkGroupSize.x * gl_WorkGroupSize.x 
    uint index = gl_GlobalInvocationID.x;
    Particle particle = particles[index];
    
    vec4 vel = particle.velosity;
    vec4 pos = particle.position;
    
    
    pos.xyz += vel.xyz * dt;
    pos.w -= lifeTimeMod * dt;
    
    for( int i=0; i < AttractorCount; ++i ) {
        
        vec3 dist = attractors[i].xyz - pos.xyz;
        
        // dt² * (mass * 1/r²)
        vel.xyz += dt * dt *
                   attractors[i].w * weightMod *
                   normalize(dist) / (dot(dist,dist) + distMod);
    }
    
    vel -= vel * clamp(dt*dampingMod,0.0,1.0);
//     pos.xyz = attractors[gl_GlobalInvocationID.x%AttractorCount].xyz;
    
    if (pos.w <= 0.0 ) {
        pos.xyz = startPositions[index].xyz;
        vel.xyz = pos.xyz*-0.02;
        pos.w += 1.0;
    }
    
    particle.velosity = vel;
    particle.position = pos;
    
    particles[index] = particle;
}