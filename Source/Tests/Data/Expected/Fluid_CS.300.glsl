#version 300
#extension GL_ARB_compute_shader : require
#extension GL_ARB_separate_shader_objects : require
layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

struct Particle
{
    vec2 position;
    vec2 velocity;
};

struct ParticleForces
{
    vec2 acceleration;
};

layout(std140) uniform type_cbSimulationConstants
{
    float timeStep;
    float wallStiffness;
    vec4 gravity;
    vec3 planes[4];
} cbSimulationConstants;

layout(std430) buffer type_RWStructuredBuffer_Particle
{
    Particle _m0[];
} particlesRW;

layout(std430) readonly buffer type_StructuredBuffer_Particle
{
    Particle _m0[];
} particlesRO;

layout(std430) readonly buffer type_StructuredBuffer_ParticleForces
{
    ParticleForces _m0[];
} particlesForcesRO;

void main()
{
    vec2 _52 = particlesRO._m0[gl_GlobalInvocationID.x].velocity;
    vec2 _54 = particlesForcesRO._m0[gl_GlobalInvocationID.x].acceleration;
    vec3 _57 = vec3(particlesRO._m0[gl_GlobalInvocationID.x].position, 1.0);
    float _60 = dot(_57, cbSimulationConstants.planes[0u]);
    float _65 = -cbSimulationConstants.wallStiffness;
    float _71 = dot(_57, cbSimulationConstants.planes[1u]);
    float _79 = dot(_57, cbSimulationConstants.planes[2u]);
    float _87 = dot(_57, cbSimulationConstants.planes[3u]);
    vec2 _100 = _52 + ((((((_54 + (cbSimulationConstants.planes[0u].xy * ((isnan(0.0) ? _60 : (isnan(_60) ? 0.0 : min(_60, 0.0))) * _65))) + (cbSimulationConstants.planes[1u].xy * ((isnan(0.0) ? _71 : (isnan(_71) ? 0.0 : min(_71, 0.0))) * _65))) + (cbSimulationConstants.planes[2u].xy * ((isnan(0.0) ? _79 : (isnan(_79) ? 0.0 : min(_79, 0.0))) * _65))) + (cbSimulationConstants.planes[3u].xy * ((isnan(0.0) ? _87 : (isnan(_87) ? 0.0 : min(_87, 0.0))) * _65))) + cbSimulationConstants.gravity.xy) * cbSimulationConstants.timeStep);
    particlesRW._m0[gl_GlobalInvocationID.x].position = particlesRO._m0[gl_GlobalInvocationID.x].position + (_100 * cbSimulationConstants.timeStep);
    particlesRW._m0[gl_GlobalInvocationID.x].velocity = _100;
}

