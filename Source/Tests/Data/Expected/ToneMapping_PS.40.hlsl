cbuffer type_cbPS : register(b0)
{
    float cbPS_lumStrength : packoffset(c0);
};

SamplerState pointSampler : register(s0);
SamplerState linearSampler : register(s1);
Texture2D<float4> colorTex : register(t0);
Texture2D<float4> lumTex : register(t1);
Texture2D<float4> bloomTex : register(t2);

static float2 in_var_TEXCOORD0;
static float4 out_var_SV_Target;

struct SPIRV_Cross_Input
{
    float2 in_var_TEXCOORD0 : TEXCOORD0;
};

struct SPIRV_Cross_Output
{
    float4 out_var_SV_Target : SV_Target0;
};

void frag_main()
{
    float4 _44 = colorTex.Sample(pointSampler, in_var_TEXCOORD0);
    float3 _60 = (_44.xyz * (0.7200000286102294921875f / mad(lumTex.Sample(pointSampler, 0.5f.xx).x, cbPS_lumStrength, 0.001000000047497451305389404296875f))).xyz;
    float3 _63 = (_60 * mad(_60, 0.666666686534881591796875f.xxx, 1.0f.xxx)).xyz;
    float3 _68 = (_63 / (1.0f.xxx + _63)).xyz + (bloomTex.Sample(linearSampler, in_var_TEXCOORD0).xyz * 0.60000002384185791015625f);
    float4 _69 = float4(_68.x, _68.y, _68.z, _44.w);
    _69.w = 1.0f;
    out_var_SV_Target = _69;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    in_var_TEXCOORD0 = stage_input.in_var_TEXCOORD0;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.out_var_SV_Target = out_var_SV_Target;
    return stage_output;
}
