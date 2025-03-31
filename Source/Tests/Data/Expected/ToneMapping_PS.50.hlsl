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
    float4 _43 = colorTex.Sample(pointSampler, in_var_TEXCOORD0);
    float3 _61 = (_43.xyz * (0.7200000286102294921875f / ((lumTex.Sample(pointSampler, 0.5f.xx).x * cbPS_lumStrength) + 0.001000000047497451305389404296875f))).xyz;
    float3 _65 = (_61 * (1.0f.xxx + (_61 * 0.666666686534881591796875f.xxx))).xyz;
    float3 _70 = (_65 / (1.0f.xxx + _65)).xyz + (bloomTex.Sample(linearSampler, in_var_TEXCOORD0).xyz * 0.60000002384185791015625f);
    float4 _71 = float4(_70.x, _70.y, _70.z, _43.w);
    _71.w = 1.0f;
    out_var_SV_Target = _71;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    in_var_TEXCOORD0 = stage_input.in_var_TEXCOORD0;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.out_var_SV_Target = out_var_SV_Target;
    return stage_output;
}
