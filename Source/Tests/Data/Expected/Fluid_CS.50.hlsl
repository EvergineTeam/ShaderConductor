struct Particle
{
    float2 position;
    float2 velocity;
};

struct ParticleForces
{
    float2 acceleration;
};

cbuffer type_cbSimulationConstants : register(b0)
{
    float cbSimulationConstants_timeStep : packoffset(c0);
    float cbSimulationConstants_wallStiffness : packoffset(c0.y);
    float4 cbSimulationConstants_gravity : packoffset(c1);
    float3 cbSimulationConstants_planes[4] : packoffset(c2);
};

RWByteAddressBuffer particlesRW : register(u0);
ByteAddressBuffer particlesRO : register(t0);
ByteAddressBuffer particlesForcesRO : register(t2);

static uint3 gl_GlobalInvocationID;
struct SPIRV_Cross_Input
{
    uint3 gl_GlobalInvocationID : SV_DispatchThreadID;
};

void comp_main()
{
    float2 _50 = asfloat(particlesRO.Load2(gl_GlobalInvocationID.x * 16 + 0));
    float2 _52 = asfloat(particlesRO.Load2(gl_GlobalInvocationID.x * 16 + 8));
    float2 _54 = asfloat(particlesForcesRO.Load2(gl_GlobalInvocationID.x * 8 + 0));
    float3 _57 = float3(_50, 1.0f);
    float _60 = dot(_57, cbSimulationConstants_planes[0u]);
    float _65 = -cbSimulationConstants_wallStiffness;
    float _71 = dot(_57, cbSimulationConstants_planes[1u]);
    float _79 = dot(_57, cbSimulationConstants_planes[2u]);
    float _87 = dot(_57, cbSimulationConstants_planes[3u]);
    float2 _100 = _52 + ((((((_54 + (cbSimulationConstants_planes[0u].xy * ((isnan(0.0f) ? _60 : (isnan(_60) ? 0.0f : min(_60, 0.0f))) * _65))) + (cbSimulationConstants_planes[1u].xy * ((isnan(0.0f) ? _71 : (isnan(_71) ? 0.0f : min(_71, 0.0f))) * _65))) + (cbSimulationConstants_planes[2u].xy * ((isnan(0.0f) ? _79 : (isnan(_79) ? 0.0f : min(_79, 0.0f))) * _65))) + (cbSimulationConstants_planes[3u].xy * ((isnan(0.0f) ? _87 : (isnan(_87) ? 0.0f : min(_87, 0.0f))) * _65))) + cbSimulationConstants_gravity.xy) * cbSimulationConstants_timeStep);
    particlesRW.Store2(gl_GlobalInvocationID.x * 16 + 0, asuint(_50 + (_100 * cbSimulationConstants_timeStep)));
    particlesRW.Store2(gl_GlobalInvocationID.x * 16 + 8, asuint(_100));
}

[numthreads(256, 1, 1)]
void main(SPIRV_Cross_Input stage_input)
{
    gl_GlobalInvocationID = stage_input.gl_GlobalInvocationID;
    comp_main();
}
