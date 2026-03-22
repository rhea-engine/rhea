#version 330 core

#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2

struct Light
{
    vec4 DirectionType;   // xyz = direction, w = type
    vec4 Position;
    vec4 Color;
    vec4 Diffuse;
    vec4 Specular;
    vec4 SpotParams;      // x = SpotAngle, y = Constant, z = Linear, w = Quadratic
};

layout(std140) uniform cbPerFrame
{
    mat4 V;
    mat4 P;
    mat4 VP;
    vec4 cameraPosition;
};

layout(std140) uniform cbLight
{
    Light lights[8];
    vec4 ambient;
    float lightCount;
    vec3 _cbLightPad0;
};

layout(std140) uniform cbMaterial
{
    vec4 mat_diffuse;
    vec4 mat_ambient;
    vec4 mat_specular;
    vec4 mat_emissive;
    float mat_shininess;
    float has_normal_map;
    vec2 _cbMaterialPad0;
};

uniform sampler2D BaseColorTexture;
uniform sampler2D NormalMap;

in vec3 v_normal;
in vec2 v_tex;
in vec4 v_positionWS;
in vec3 v_tangent;
in vec3 v_bitangent;

out vec4 FragColor;

vec4 DoSpecularPhong(Light light, vec3 V, vec3 L, vec3 N)
{
    vec3 R = normalize(reflect(-L, N));
    float RdotV = max(0.0, dot(R, V));
    return light.Color * pow(RdotV, mat_shininess);
}

vec4 DoDiffuse(Light light, vec3 L, vec3 N)
{
    float NdotL = max(0.0, dot(N, L));
    return light.Color * NdotL;
}

struct LightingResult
{
    vec4 Diffuse;
    vec4 Specular;
};

LightingResult DoDirectionalLight(Light light, vec3 View, vec4 P, vec3 N)
{
    LightingResult result;
    vec3 L = -light.DirectionType.xyz;
    result.Diffuse = DoDiffuse(light, L, N);
    result.Specular = DoSpecularPhong(light, View, L, N);
    return result;
}

float DoAttenuation(Light light, float distanceValue)
{
    float att = light.SpotParams.y
              + light.SpotParams.z * distanceValue
              + light.SpotParams.w * distanceValue * distanceValue;

    if (att == 0.0)
        return 0.0;

    return 1.0 / att;
}

LightingResult DoPointLight(Light light, vec3 V, vec4 P, vec3 N)
{
    LightingResult result;

    vec3 L = (light.Position - P).xyz;
    float distanceValue = length(L);
    L = L / distanceValue;

    float attenuation = DoAttenuation(light, distanceValue);

    result.Diffuse = DoDiffuse(light, L, N) * attenuation;
    result.Specular = DoSpecularPhong(light, V, L, N) * attenuation;

    return result;
}

float DoSpotCone(Light light, vec3 L)
{
    float minCos = cos(light.SpotParams.x);
    float maxCos = (minCos + 1.0) / 2.0;
    float cosAngle = dot(light.DirectionType.xyz, -L);
    return smoothstep(minCos, maxCos, cosAngle);
}

LightingResult DoSpotLight(Light light, vec3 V, vec4 P, vec3 N)
{
    LightingResult result;

    vec3 L = (light.Position - P).xyz;
    float distanceValue = length(L);
    L = L / distanceValue;

    float attenuation = DoAttenuation(light, distanceValue);
    float spotIntensity = DoSpotCone(light, L);

    result.Diffuse = DoDiffuse(light, L, N) * attenuation * spotIntensity;
    result.Specular = DoSpecularPhong(light, V, L, N) * attenuation * spotIntensity;

    return result;
}

LightingResult ComputeLighting(vec4 P, vec3 N)
{
    LightingResult result;
    result.Diffuse = vec4(0.0);
    result.Specular = vec4(0.0);

    vec3 View = normalize((cameraPosition - P).xyz);

    for (int i = 0; i < int(lightCount); ++i)
    {
        LightingResult singleResult;
        singleResult.Diffuse = vec4(0.0);
        singleResult.Specular = vec4(0.0);

        int type = int(lights[i].DirectionType.w);

        if (type == DIRECTIONAL_LIGHT)
            singleResult = DoDirectionalLight(lights[i], View, P, N);
        else if (type == POINT_LIGHT)
            singleResult = DoPointLight(lights[i], View, P, N);
        else if (type == SPOT_LIGHT)
            singleResult = DoSpotLight(lights[i], View, P, N);

        result.Diffuse += singleResult.Diffuse;
        result.Specular += singleResult.Specular;
    }

    result.Diffuse = clamp(result.Diffuse, 0.0, 1.0);
    result.Specular = clamp(result.Specular, 0.0, 1.0);

    return result;
}

void main()
{
    vec3 finalNormal = normalize(v_normal);

    vec4 diffuseSample = texture(BaseColorTexture, v_tex);

    if (has_normal_map > 0.0)
    {
        vec3 normalMapSample = texture(NormalMap, v_tex).xyz * 2.0 - 1.0;
        mat3 TBN = mat3(v_tangent, v_bitangent, v_normal);
        vec3 normalMapSampleTangentSpace = normalMapSample * TBN;
        finalNormal = normalize(normalMapSampleTangentSpace);
    }

    LightingResult lit = ComputeLighting(v_positionWS, finalNormal);

    vec3 finalColor =
        (mat_emissive.rgb
        + mat_ambient.rgb * ambient.rgb
        + mat_diffuse.rgb * lit.Diffuse.rgb
        + mat_specular.rgb * lit.Specular.rgb)
        * diffuseSample.rgb;

    FragColor = vec4(finalColor, diffuseSample.a);
}