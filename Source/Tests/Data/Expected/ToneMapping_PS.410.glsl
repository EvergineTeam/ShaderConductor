#version 410

layout(std140) uniform type_cbPS
{
    float lumStrength;
} cbPS;

uniform sampler2D SPIRV_Cross_CombinedcolorTexpointSampler;
uniform sampler2D SPIRV_Cross_CombinedbloomTexlinearSampler;
uniform sampler2D SPIRV_Cross_CombinedlumTexpointSampler;

layout(location = 0) in vec2 in_var_TEXCOORD0;
layout(location = 0) out vec4 out_var_SV_Target;

void main()
{
    vec4 _44 = texture(SPIRV_Cross_CombinedcolorTexpointSampler, in_var_TEXCOORD0);
    vec3 _60 = (_44.xyz * (0.7200000286102294921875 / fma(texture(SPIRV_Cross_CombinedlumTexpointSampler, vec2(0.5)).x, cbPS.lumStrength, 0.001000000047497451305389404296875))).xyz;
    vec3 _63 = (_60 * fma(_60, vec3(0.666666686534881591796875), vec3(1.0))).xyz;
    vec3 _68 = (_63 / (vec3(1.0) + _63)).xyz + (texture(SPIRV_Cross_CombinedbloomTexlinearSampler, in_var_TEXCOORD0).xyz * 0.60000002384185791015625);
    vec4 _69 = vec4(_68.x, _68.y, _68.z, _44.w);
    _69.w = 1.0;
    out_var_SV_Target = _69;
}

